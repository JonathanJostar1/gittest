#include "cmn_stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// 创建一个新堆栈
int createStack(struct Stack* stack) 
{
    stack->capacity = STARCK_ARRAY_SIZE;
    stack->top = -1;
    return 0;
}

// 判断堆栈是否已满
int isFull(struct Stack* stack) 
{
    return stack->top == stack->capacity - 1;
}

// 判断堆栈是否为空
int isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}

// 将元素推入堆栈
void push(struct Stack* stack, int item) 
{
    if (isFull(stack))
    {
       return; 
    } 
    stack->array[++stack->top] = item;
}

// 从堆栈中弹出元素
int pop(struct Stack* stack) 
{
    if (isEmpty(stack))
    {
       return -1; 
    } 
    return stack->array[stack->top--];
}

// 获取堆栈顶部元素，不弹出
int peek(struct Stack* stack) 
{
    if (isEmpty(stack))
    {
        return -1;
    }
    return stack->array[stack->top];
}

