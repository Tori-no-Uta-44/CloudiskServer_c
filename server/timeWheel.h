//
// Created by 李勃鋆 on 24-8-9.
//

#ifndef TIME_WHEEL_H
#define TIME_WHEEL_H

#define TIME_WHEEL_SIZE 60  // 时间轮的大小
#define INITIAL_HASH_SIZE 128
#define LOAD_FACTOR 0.75    // 当哈希表的使用率超过这个比例时，进行扩容

typedef struct TimeWheel_ListNode {
	int peerfd;
	struct TimeWheel_ListNode *prev;
	struct TimeWheel_ListNode *next;
} TimeWheel_ListNode;

typedef struct HashNode {
	int peerfd;
	int index;
	struct HashNode *next;
} HashNode;

typedef struct TimeWheel {
	TimeWheel_ListNode *slots[TIME_WHEEL_SIZE];
	HashNode **hashTable;
	int hashSize;
	int elementCount; // 当前哈希表中的元素个数
	int currentIndex;

	// 方法指针
	void (*addUser)(struct TimeWheel*, int, int);
	void (*updateUserTimeout)(struct TimeWheel*, int, int);
	void (*removeUser)(struct TimeWheel*, int);
	void (*tick)(struct TimeWheel*);
} TimeWheel;

extern TimeWheel* wheel;

// 函数声明
TimeWheel* createTimeWheel();
void destroyTimeWheel(TimeWheel* wheel);

#endif


