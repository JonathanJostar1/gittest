#ifndef __STACK_LIST_H_
#define __STACK_LIST_H_


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

/* 判断空栈 */
int stack_is_empty(link_stack_t *link_stack);

/* 入栈 */
int push_data(link_stack_t *link_stack, int data);

/* 出栈 */
int pop_data(link_stack_t *link_stack, int *data);

/* 创建空栈 */
void stack_init(link_stack_t *link_stack);

/* 销毁栈 */
void stack_destroy(link_stack_t *link_stack);

/* 获取栈顶数据 */
void get_top_stack(link_stack_t *link_stack, data_struct_t *data_s);


#endif // __STACK_LIST_H_