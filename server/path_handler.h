#ifndef __PATH_H
#define __PATH_H

#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10

// 使用动态数组构造栈来存储client看到的虚拟文件路径
typedef struct {
    char **data;
    int top;
    int capacity;
} path_stack;

// 创建一个新的栈
void init_stack(path_stack *stack);
// 释放栈的内存
void free_stack(path_stack *stack);
// 扩展栈的容量
void resize_stack(path_stack *stack);
// 入栈
void push_stack(path_stack *stack, const char *path);
// 出栈
char* pop_stack(path_stack *stack);
// 查看栈顶元素
char* peek_stack(path_stack *stack);
// 将路径按目录分割并入栈
void pushPath(path_stack *s, const char *path);
char* pathFromStack(const path_stack *s);

/* ***************************************************************************** */

// 使用动态数组构造队列来存储client输入的虚拟文件路径
typedef struct {
    char **data;
    int front;
    int rear;
    int size;
    int capacity;
} path_que;

// 初始化队列
void init_queue(path_que *q);
// 重新分配队列的容量
void resize_queue(path_que *q);
// 入队
void enqueue(path_que *q, const char *item);
// 出队
char *dequeue(path_que *q);
// 查看队头元素
char *peek_que(path_que *q);
// 释放队列的内存
void freeQueue(path_que *q);
// 将路径按目录分割并入队
void enqueuePath(path_que *q, const char *path);
char* pathFromQueue(const path_que *q);

int path_handler(char *pwd[], char *input_path[]);

char *get_whole_path(const char *, const char *);

#endif
