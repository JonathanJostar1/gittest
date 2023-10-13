#include "cmn_stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// ����һ���¶�ջ
int createStack(struct Stack* stack) 
{
    stack->capacity = STARCK_ARRAY_SIZE;
    stack->top = -1;
    return 0;
}

// �ж϶�ջ�Ƿ�����
int isFull(struct Stack* stack) 
{
    return stack->top == stack->capacity - 1;
}

// �ж϶�ջ�Ƿ�Ϊ��
int isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}

// ��Ԫ�������ջ
void push(struct Stack* stack, int item) 
{
    if (isFull(stack))
    {
       return; 
    } 
    stack->array[++stack->top] = item;
}

// �Ӷ�ջ�е���Ԫ��
int pop(struct Stack* stack) 
{
    if (isEmpty(stack))
    {
       return -1; 
    } 
    return stack->array[stack->top--];
}

// ��ȡ��ջ����Ԫ�أ�������
int peek(struct Stack* stack) 
{
    if (isEmpty(stack))
    {
        return -1;
    }
    return stack->array[stack->top];
}

