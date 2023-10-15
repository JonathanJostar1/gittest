#include <stdlib.h>
#include <stdio.h>

typedef struct node
{
    int id;            /* 节点id，id = 0, 1, ... */ 
    int data;          /* 节点数据 */
    struct node *next; /* 下一个节点 */
} Node_t;

/**
 * @brief 创建链表
 * 
 * @param len 
 * @return Node_t* 
 */
Node_t *create_list(int len);

/**
 * @brief 打印链表各id
 * 
 * @param list 
 * @return int 
 */
int printf_list(Node_t *list);

/**
 * @brief 获取链表长度
 *
 * @param list
 * @return int
 */
int get_list_length(Node_t *list);

/**
 * @brief 搜索 list 中第 id 个节点
 *
 * @param list
 * @param id 节点id
 * @return Node_t*
 */
Node_t *search_node(Node_t *list, int id);

/**
 * @brief 删除 list 中第 id 个节点
 *
 * @param list
 * @param id 节点id
 * @return Node_t*
 */
Node_t *delete_node(Node_t *list, int id);

/**
 * @brief 在链表结尾处插入节点，当链表不存在时创建新链表并返回
 *
 * @param list
 * @return Node_t*
 */
Node_t *add_node_to_end(Node_t *list);

/**
 * @brief 在链表开始处插入节点，当链表不存在时创建新链表并返回
 *
 * @param list
 * @return node
 */
Node_t *add_node_to_first(Node_t *list);

/**
 * @brief 在第id出插入一个节点，当链表不存在时创建新链表并返回
 * 
 * @param list 
 * @param id 
 * @return Node_t* 
 */
Node_t *insert_node(Node_t *list, int id);