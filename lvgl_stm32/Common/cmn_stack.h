#ifndef  __COMMON_H_
#define  __COMMON_H_

#include "stm32f10x.h"

#define  STARCK_ARRAY_SIZE  10

// 定义堆栈结构
typedef struct Stack {
    int top;
    unsigned capacity;
    int array[STARCK_ARRAY_SIZE];
}cmn_stack_t;

// 创建一个新堆栈
int createStack(struct Stack* stack);

// 判断堆栈是否已满
int isFull(struct Stack* stack);

// 判断堆栈是否为空
int isEmpty(struct Stack* stack);

// 将元素推入堆栈
void push(struct Stack* stack, int item);

// 从堆栈中弹出元素
int pop(struct Stack* stack);

// 获取堆栈顶部元素，不弹出
int peek(struct Stack* stack);

#endif
