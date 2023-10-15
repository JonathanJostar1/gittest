#include <stdlib.h>
#include <stdio.h>
#include "list.h"

Node_t *insert_node(Node_t *list, int id)
{
    Node_t *p_new = (Node_t *)malloc(sizeof(Node_t));
    if (NULL == list)
    {
        /* 链表不存在时创建新链表 */
        return p_new;  
    }
    Node_t *p_list = list;

    if (id == 0)
    {
        return add_node_to_first(p_list);
    }

    if (id >= get_list_length(p_list))
    {
        return add_node_to_end(p_list);
    }

    /* 查找节点 */
    for (int i = 1; i < id && p_list != NULL; i++)
    {
        p_list = p_list->next;
    }
    p_new->id = id;
    p_new->next = p_list->next;
    p_list->next = p_new;

    /* 更新后续节点id */
    for (p_list = p_list->next; p_list != NULL; p_list = p_list->next)
    {
        p_list->id += 1;
    }

    return list;
}

Node_t *add_node_to_first(Node_t *list)
{
    Node_t *p_new = (Node_t *)malloc(sizeof(Node_t));
    if (NULL == p_new)
    {
        printf("malloc new node failed");
        return NULL;
    }

    if (NULL == list)
    {
        return p_new;
    }

    Node_t *p_list = list;
    p_new->id = 0;
    p_new->next = p_list;

    /* 更新后续节点ID*/
    for (; p_list != NULL; p_list = p_list->next)
    {
        p_list->id += 1;
    }
    return p_new;
}

Node_t *add_node_to_end(Node_t *list)
{
    Node_t *p_new = (Node_t *)malloc(sizeof(Node_t));
    if (NULL == p_new)
    {
        printf("malloc new node failed");
        return NULL;
    }

    if (NULL == list)
    {
        return p_new;
    }

    Node_t *p_list = list;
    while (p_list->next)
    {
        p_list = p_list->next;
    }

    p_new->id = p_list->id + 1;
    p_new->next = NULL;

    p_list->next = p_new;
    p_list = p_new;
    return list;
}

Node_t *delete_node(Node_t *list, int id)
{
    if (NULL == list)
    {
        printf("List is NULL");
        return NULL;
    }

    int len = get_list_length(list);
    if(id > len || id < 0)
    {
        return list;
    }

    Node_t *p_pre, *p_list = list;
    if (0 == id)    // 第一个节点
    {
        list = list->next;
        free(p_list);
        return list;
    }

    if(len == (id - 1)) // 最后一个节点
    {
        while(p_list->next != NULL)
        {
            p_list = p_list->next;
        }
        free(p_list);
        return list;
    }

    for (int i = 1; i <= id && p_list != NULL; i++)
    {
        p_pre = p_list;
        p_list = p_list->next;
    }
    p_pre->next = p_list->next;
    free(p_list);
    return list;
}

Node_t *search_node(Node_t *list, int id)
{
    if (NULL == list)
    {
        printf("List is NULL");
        return NULL;
    }
    for (; list != NULL; list = list->next)
    {
        if (list->id == id)
            return list;
    }
    return NULL;
}

int get_list_length(Node_t *list)
{
    if (NULL == list)
    {
        printf("list is NULL\n");
        return -1;
    }
    int length = 0;
    while (list != NULL)
    {
        length++;
        list = list->next;
    }
    return length;
}

int printf_list(Node_t *list)
{
    if (NULL == list)
    {
        printf("list is NULL\n");
        return -1;
    }
    while (list != NULL)
    {
        printf("id = %d\n", list->id);
        list = list->next;
    }

    return 0;
}

Node_t *create_list(int len)
{
    if (len < 1)
    {
        return NULL;
    }
    Node_t *p_head, *p_tail;
    int i;

    p_head = NULL;
    for (int i = 0; i < len; i++)
    {
        Node_t *p_new = (Node_t *)malloc(sizeof(Node_t));
        p_new->id = i;
        p_new->next = NULL;
        if (NULL == p_head)
        {
            p_head = p_new;
            p_tail = p_head;
        }
        else
        {
            p_tail->next = p_new;
            p_tail = p_new;
        }
    }
    p_tail->next = NULL;
    return p_head;
}

