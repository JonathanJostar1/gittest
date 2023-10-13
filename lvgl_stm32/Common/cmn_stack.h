#ifndef  __COMMON_H_
#define  __COMMON_H_

#include "stm32f10x.h"

#define  STARCK_ARRAY_SIZE  10

// �����ջ�ṹ
typedef struct Stack {
    int top;
    unsigned capacity;
    int array[STARCK_ARRAY_SIZE];
}cmn_stack_t;

// ����һ���¶�ջ
int createStack(struct Stack* stack);

// �ж϶�ջ�Ƿ�����
int isFull(struct Stack* stack);

// �ж϶�ջ�Ƿ�Ϊ��
int isEmpty(struct Stack* stack);

// ��Ԫ�������ջ
void push(struct Stack* stack, int item);

// �Ӷ�ջ�е���Ԫ��
int pop(struct Stack* stack);

// ��ȡ��ջ����Ԫ�أ�������
int peek(struct Stack* stack);

#endif
