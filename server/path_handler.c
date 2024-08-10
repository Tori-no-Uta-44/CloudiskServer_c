#include "path_handler.h"
#include <stdio.h>

// 创建一个新的栈
void init_stack(path_stack *stack) {
    stack->capacity = INITIAL_CAPACITY;
    stack->top = -1;
    stack->data = calloc(stack->capacity, sizeof(char*));
}

// 释放栈的内存
void free_stack(path_stack *stack) {
    for (int i = 0; i <= stack->top; i++) {
        free(stack->data[i]);
    }
    free(stack->data);
}

// 扩展栈的容量
void resize_stack(path_stack *stack) {
    stack->capacity *= 2;
    stack->data = (char**)realloc(stack->data, stack->capacity * sizeof(char*));
}

// 入栈
void push_stack(path_stack *stack, const char *path) {
    if (stack->top == stack->capacity - 1) {
        resize_stack(stack);
    }
    stack->data[++stack->top] = strdup(path);
}

// 出栈
char* pop_stack(path_stack *stack) {
    if (stack->top == -1) {
        return NULL;
    }
    return stack->data[stack->top--];
}

// 查看栈顶元素
char* peek_stack(path_stack *stack) {
    if (stack->top == -1) {
        return NULL;
    }
    return stack->data[stack->top];
}

// 将路径按目录分割并入栈
void pushPath(path_stack *s, const char *path) {
    char *pathCopy = strdup(path);
    char *token = strtok(pathCopy, "/");
    while (token != NULL) {
        push_stack(s, token);
        token = strtok(NULL, "/");
    }
    free(pathCopy);
}

char* pathFromStack(const path_stack *s) {
    if (s->top == -1) 
        return strdup("");  // 空栈返回空字符串

    // 计算总长度，包括分隔符
    int totalLength = 0;
    for (int i = 0; i <= s->top; i++) {
        totalLength += strlen(s->data[i]) + 1; // +1 for '/' or null terminator
    }

    char *path = calloc(totalLength + 1, sizeof(char)); // +1 for null terminator
    path[0] = '/';

    for (int i = 0; i <= s->top; i++) {
        strcat(path, s->data[i]);
        if (i < s->top) {
            strcat(path, "/");
        }
    }
    return path;
}


// 测试 虚拟路径
// int main() {
//     path_stack s;
//     init_stack(&s);

//     const char *filePath = "/usr/local/bin/example.exe";
//     pushPath(&s, filePath);
//     printf("peek() = %s\n", peek_stack(&s));
//     push_stack(&s, "aaa");

//     char *rest = pathFromStack(&s);

//     // 输出栈中的内容
//     printf("Stack content (top to bottom):\n");
//     char *dir;
//     while ((dir = pop_stack(&s)) != NULL) {
//         printf("/%s\n", dir);
//         free(dir);
//     }

//     printf("result: %s\n", rest);
//     free_stack(&s);
//     return 0;
// }


// 初始化队列
void init_queue(path_que *q) {
    q->data = calloc(INITIAL_CAPACITY, sizeof(char *));
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    q->capacity = INITIAL_CAPACITY;
}

// 重新分配队列的容量
void resize_queue(path_que *q) {
    int newCapacity = q->capacity * 2;
    char **newData = malloc(newCapacity * sizeof(char *));
    
    for (int i = 0; i < q->size; i++) {
        newData[i] = q->data[(q->front + i) % q->capacity];
    }

    free(q->data);
    q->data = newData;
    q->front = 0;
    q->rear = q->size - 1;
    q->capacity = newCapacity;
}

// 入队
void enqueue(path_que *q, const char *item) {
    if (q->size == q->capacity) {
        resize_queue(q);
    }
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = strdup(item);
    q->size++;
}

// 出队
char *dequeue(path_que *q) {
    if (q->size == 0) {
        return NULL;
    }
    char *item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return item;
}

// 查看队头元素
char *peek_que(path_que *q) {
    if (q->size == 0) {
        return NULL;
    }
    return q->data[q->front];
}

// 释放队列的内存
void free_queue(path_que *q) {
    for (int i = 0; i < q->size; i++) {
        free(q->data[(q->front + i) % q->capacity]);
    }
    free(q->data);
}

// 将路径按目录分割并入队(不会存/)
void enqueuePath(path_que *q, const char *path) {
    char *pathCopy = strdup(path);
    char *token = strtok(pathCopy, "/");
    while (token != NULL) {
        enqueue(q, token);
        token = strtok(NULL, "/");
    }
    free(pathCopy);
}

char* pathFromQueue(const path_que *q) {
    if (q->size == 0) {
        return strdup("");  // 空队列返回空字符串
    }

    // 计算总长度，包括分隔符
    int totalLength = 0;
    for (int i = 0; i < q->size; i++) {
        totalLength += strlen(q->data[(q->front + i) % q->capacity]) + 1; // +1 for '/' or null terminator
    }

    char *path = malloc(totalLength + 1); // +1 for null terminator
    path[0] = '\0';

    for (int i = 0; i < q->size; i++) {
        strcat(path, q->data[(q->front + i) % q->capacity]);
        if (i < q->size - 1) {
            strcat(path, "/");
        }
    }

    return path;
}

// 测试 用户输入的路径
// int main() {
//     path_que q;
//     init_queue(&q);

//     // const char *filePath = "/usr/local/bin/example.exe";
//     const char *filePath = "../../bin/example.exe";
//     enqueuePath(&q, filePath);
//     printf("peek_que: %s\n", peek_que(&q));
//     enqueue(&q, "bbb");

//     char *rest = pathFromQueue(&q);

//     // 输出队列中的内容
//     printf("Queue content (front to rear):\n");
//     char *dir;
//     while ((dir = dequeue(&q)) != NULL) {
//         printf("/%s\n", dir);
//         free(dir);
//     }

//     printf("result: %s\n", rest);

//     free_queue(&q);
//     return 0;
// }

// 处理用户输入的路径 & 参数校验
int path_handler(char *pwd[], char *input_path[]) {
    
    // pwd[strcspn(pwd, "\n")] = '\0';
    // input_path[strcspn(input_path, "\n")] = '\0';

    // 创建1个栈，将pwd根据/切割成一个个节点，节点的值为目录
    path_stack s_path;
    init_stack(&s_path);
    pushPath(&s_path, *pwd);

    // 创建1个队列，将del_path根据/切割成一个个节点，节点值为目录（最后一个可能为文件）
    path_que q_path;
    init_queue(&q_path);
    enqueuePath(&q_path, *input_path);

    // del_path 这里无需判断是文件还是目录，下面查询数据库的时候会比较
    // 也无需考虑中间的路径，数据库中有完整虚拟路径

    // del_path，如果有.., 则pwd栈就出栈，同时del_path出队
    // 如果pwd 已经到栈底了，还有.. 就退出报错
    // TODO： 还没考虑 del_path 是 will/../1.txt，即..在中间的情况
    char *dir = peek_que(&q_path); 
    while (dir && strcmp(dir, "..") == 0) {
        dequeue(&q_path);
        char *tmp;
        if ((tmp = pop_stack(&s_path)) == NULL) {
            fprintf(stderr, "bad path.\n");
            return -1;
        }
        free(tmp);
        dir = peek_que(&q_path);
    }

    *pwd = pathFromStack(&s_path);
    *input_path = pathFromQueue(&q_path);

    return 0;
}

// int main(int argc, char **argv) {
//     // // 格式： 当前路径，要操作的路径
//     // printf("str1: pwd, str2: operate path\n");
//     char *str1 = calloc(128, 1);
//     // fgets(str1, 128, stdin);
//     // str1[strcspn(str1, "\n")] = '\0';
//     char *str2 = calloc(128,1);
//     // fgets(str2, 128, stdin);
//     // str2[strcspn(str2, "\n")] = '\0';

//     strcpy(str1, argv[1]);
//     str2[0] = '/';
//     strcat(str2, argv[2]);

//     int ret = path_handler(&str1, &str2);
//     printf("ret = %d\n", ret);
//     puts(str1);
//     puts(str2);
//     return 0;
// }
// /will/aaa/bbb/ccc
// /../../1.txt


char *get_whole_path(const char *del_path, const char *path) {
    char *whole_path = calloc(512,1);

    char *pwd = calloc(128, 1);
    strcpy(pwd, path);
    char *input_path = calloc(128,1);

    if (del_path[0] == '/') {
        // 绝对路径, 无需和pwd拼接
        strcpy(whole_path, del_path);

    }else {
        // 相对路径
        input_path[0] = '/';
        strcat(input_path, del_path);

        // 处理用户输入的路径(含..)
        int ret = path_handler(&pwd, &input_path);
        if (ret == -1)
            return NULL;
        // puts(pwd);
        // puts(input_path);

        // 与pwd 进行拼接
        sprintf(whole_path, "%s/%s", pwd, input_path);
    }
    // printf("%s\n", whole_path);
    return whole_path;
}
