#ifndef  __CMN_RINGBUF_H_
#define  __CMN_RINGBUF_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

/* 最大缓冲区大小 */
#define MAX_RINGBUF_SIZE  1024

typedef enum
{
    RINGBUF_STATE_EMPTY = 0,
    RINGBUF_STATE_FULL = 1,
    RINGBUF_STATE_NORMAL = 2,
}ringbuf_state_t;

typedef struct 
{
    uint8_t *buffer_ptr;    /* 缓冲区指针 */

    uint16_t buffer_size;    /* 缓冲区大小 */

    int32_t read_index;     /* 读索引位置 */

    int32_t write_index;    /* 写索引位置 */

    ringbuf_state_t is_full;

}ringbuf_t;

/* 以申请内存方式初始化ringbuf */
int ringbuf_init(ringbuf_t *rb, uint16_t size);

/* 释放ringbuf申请的内存 */
int ringbuf_deinit(ringbuf_t *rb);

/* 以静态数组方式初始化ringbuf */
int ringbuf_static_init(ringbuf_t *rb, uint8_t *buff, uint16_t size);

/* 释放ringbuf申请的内存 */
int ringbuf_static_deinit(ringbuf_t *rb);

/* 写入指定大小数据, 过长的数据只会写入最大长度 */
int ringbuf_write(ringbuf_t *rb, uint8_t *data, uint16_t length);

/* 读出指定大小数据 */
int ringbuf_read(ringbuf_t *rb, uint8_t *data, uint16_t size);

/* 获取缓冲区状态 */
ringbuf_state_t ringbuf_state_get(ringbuf_t *rb);

/* 缓冲区内未读数据大小 */
int ringbuf_unread_byte(ringbuf_t *rb);

/* 缓冲区可写数据大小 */
int ringbuf_unwrite_byte(ringbuf_t *rb);


#endif  //__CMN_RINGBUF_H_
