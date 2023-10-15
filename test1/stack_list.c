#include "stack_list.h"


typedef struct data_struct
{
    int data;
    struct data_struct *next;
}data_struct_t;

typedef struct
{
    data_struct_t *top;  // top指针
    int count;
}link_stack_t;


int stack_is_empty(link_stack_t *link_stack)
{
    return link_stack->count;
}

/* 入栈 */
int push_data(link_stack_t *link_stack, int data)
{
    data_struct_t *data_s = (data_struct_t *)malloc(sizeof(data_struct_t));
    data_s->data = data;
    data_s->next = link_stack->top;
    link_stack->top = data_s;
    link_stack->count++;

    return 0;
}

/* 出栈 */
int pop_data(link_stack_t *link_stack, int *data)
{
    if(stack_is_empty(link_stack) < 0)
    {
        return -1;
    }

    data_struct_t *data_s;
    *data = link_stack->top->data;

    data_s->next = link_stack->top;
    link_stack->top = link_stack->top->next;
    free(data_s);
    link_stack->count--;

    return 0;
}

/* 创建空栈 */
void stack_init(link_stack_t *link_stack)
{
    link_stack = (link_stack_t *)malloc(sizeof(link_stack_t));
    link_stack->count = -1;
}

/* 销毁栈 */
void stack_destroy(link_stack_t *link_stack)
{
    free(link_stack);
}

/* 获取栈顶数据 */
void get_top_stack(link_stack_t *link_stack, data_struct_t *data_s)
{
    if(stack_is_empty(link_stack) < 0)
    {
        return -1;
    }

    data_s = link_stack->top;
}