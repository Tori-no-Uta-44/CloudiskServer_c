//
// Created by 李勃鋆 on 24-8-9.
//
#include "thread_pool.h"
#include "timeWheel.h"

TimeWheel* wheel = NULL;

// 安全的内存分配函数
static void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		exit(1);
	}
	return ptr;
}

// 哈希函数
static int hashFunction(int peerfd, int hashSize) {
	return peerfd % hashSize;
}

// 创建链表节点
static TimeWheel_ListNode *createListNode(int peerfd) {
	TimeWheel_ListNode *node = (TimeWheel_ListNode *) safe_malloc(sizeof(TimeWheel_ListNode));
	node->peerfd = peerfd;
	node->prev = node->next = node;
	return node;
}

// 动态扩展哈希表
static void resizeHashTable(TimeWheel *wheel) {
	int oldHashSize = wheel->hashSize;
	HashNode **oldHashTable = wheel->hashTable;

	int newHashSize = oldHashSize * 2;
	HashNode **newHashTable = (HashNode **) calloc(newHashSize, sizeof(HashNode *));

	for (int i = 0; i < oldHashSize; i++) {
		HashNode *current = oldHashTable[i];
		while (current) {
			HashNode *next = current->next;
			int newIndex = hashFunction(current->peerfd, newHashSize);
			current->next = newHashTable[newIndex];
			newHashTable[newIndex] = current;
			current = next;
		}
	}

	free(oldHashTable);
	wheel->hashTable = newHashTable;
	wheel->hashSize = newHashSize;
}

// 向哈希表中插入节点
static void insertToHashTable(TimeWheel *wheel, int peerfd, int index) {
	if (wheel->elementCount >= wheel->hashSize * LOAD_FACTOR) {
		resizeHashTable(wheel);
	}

	int hashIndex = hashFunction(peerfd, wheel->hashSize);
	HashNode *node = (HashNode *) safe_malloc(sizeof(HashNode));
	node->peerfd = peerfd;
	node->index = index;
	node->next = wheel->hashTable[hashIndex];
	wheel->hashTable[hashIndex] = node;
	wheel->elementCount++;
}

// 从哈希表中删除节点
static void deleteFromHashTable(TimeWheel *wheel, int peerfd) {
	int hashIndex = hashFunction(peerfd, wheel->hashSize);
	HashNode *prev = NULL, *current = wheel->hashTable[hashIndex];
	while (current) {
		if (current->peerfd == peerfd) {
			if (prev) {
				prev->next = current->next;
			} else {
				wheel->hashTable[hashIndex] = current->next;
			}
			free(current);
			wheel->elementCount--;
			return;
		}
		prev = current;
		current = current->next;
	}
}

// 从时间轮的链表中移除用户
static void removeUserFromList(TimeWheel *wheel, int peerfd, int index) {
	TimeWheel_ListNode *list = wheel->slots[index];

	if (list == NULL) {
		return;
	}

	TimeWheel_ListNode *current = list;
	do {
		assert(current != NULL); // 确保当前指针有效
		if (current->peerfd == peerfd) {
			printf("Removing user with peerfd: %d from index %d (address: %p)\n", peerfd, index, (void *) current);
			if (current->next == current) {
				wheel->slots[index] = NULL;
			} else {
				current->prev->next = current->next;
				current->next->prev = current->prev;
				if (wheel->slots[index] == current) {
					wheel->slots[index] = current->next;
				}
			}
			// 将当前节点的指针设置为NULL，以防后续访问
			current->prev = current->next = NULL;
			free(current);
			return;
		}
		current = current->next;
	} while (current != list);
}

// 添加用户到时间轮
void addUser(TimeWheel *wheel, int peerfd, int timeout) {
	int index = (wheel->currentIndex + timeout) % TIME_WHEEL_SIZE;
	TimeWheel_ListNode *node = createListNode(peerfd);

	printf("Adding user with peerfd: %d at index %d (address: %p)\n", peerfd, index, (void *) node);
	if (wheel->slots[index] == NULL) {
		wheel->slots[index] = node;
	} else {
		TimeWheel_ListNode *head = wheel->slots[index];
		assert(head != NULL); // 确保头节点有效
		node->next = head;
		node->prev = head->prev;
		head->prev->next = node;
		head->prev = node;
	}

	insertToHashTable(wheel, peerfd, index);
}

// 更新用户的超时时间
void updateUserTimeout(TimeWheel *wheel, int peerfd, int timeout) {
	int hashIndex = hashFunction(peerfd, wheel->hashSize);
	HashNode *hashNode = wheel->hashTable[hashIndex];

	if (hashNode == NULL) {
		return;
	}

	int oldIndex = hashNode->index;
	printf("Updating user timeout with peerfd: %d, from index %d\n", peerfd, oldIndex);
	removeUserFromList(wheel, peerfd, oldIndex);
	deleteFromHashTable(wheel, peerfd);

	// 重新添加到新的超时槽位
	addUser(wheel, peerfd, timeout);
}

// 从时间轮中移除用户
void removeUser(TimeWheel *wheel, int peerfd) {
	int hashIndex = hashFunction(peerfd, wheel->hashSize);
	HashNode *hashNode = wheel->hashTable[hashIndex];

	if (hashNode == NULL) {
		return;
	}

	int index = hashNode->index;
	removeUserFromList(wheel, peerfd, index);
	deleteFromHashTable(wheel, peerfd);
}

// 处理时间轮的滴答操作
void tick(TimeWheel *wheel) {
	printf("Tick at index %d:\n", wheel->currentIndex);
	TimeWheel_ListNode *list = wheel->slots[wheel->currentIndex];
	if (list) {
		TimeWheel_ListNode *current = list;
		do {
			assert(current != NULL); // 确保当前指针有效
			printf("  User with peerfd: %d found at index %d (address: %p)\n", current->peerfd, wheel->currentIndex,
			       (void *) current);
            int peerfd = current->peerfd;
			TimeWheel_ListNode *next = current->next;
			removeUser(wheel, peerfd);
            close(peerfd);
			current = next;
		} while (current != list && current != NULL);
	}

	wheel->slots[wheel->currentIndex] = NULL;
	wheel->currentIndex = (wheel->currentIndex + 1) % TIME_WHEEL_SIZE;
}

// 初始化时间轮结构
TimeWheel *createTimeWheel() {
	TimeWheel *wheel = (TimeWheel *) safe_malloc(sizeof(TimeWheel));
	memset(wheel->slots, 0, sizeof(wheel->slots));
	wheel->hashSize = INITIAL_HASH_SIZE;
	wheel->hashTable = (HashNode **) calloc(INITIAL_HASH_SIZE, sizeof(HashNode *));
	wheel->elementCount = 0;
	wheel->currentIndex = 0;

	wheel->addUser = addUser;
	wheel->updateUserTimeout = updateUserTimeout;
	wheel->removeUser = removeUser;
	wheel->tick = tick;

	return wheel;
}

// 释放时间轮结构
void destroyTimeWheel(TimeWheel *wheel) {
	for (int i = 0; i < TIME_WHEEL_SIZE; i++) {
		TimeWheel_ListNode *list = wheel->slots[i];
		if (list) {
			TimeWheel_ListNode *current = list->next;
			while (current != list) {
				TimeWheel_ListNode *next = current->next;
				free(current);
				current = next;
			}
			free(list);
		}
	}

	for (int i = 0; i < wheel->hashSize; i++) {
		HashNode *current = wheel->hashTable[i];
		while (current) {
			HashNode *next = current->next;
			free(current);
			current = next;
		}
	}

	free(wheel->hashTable);
	free(wheel);
}

