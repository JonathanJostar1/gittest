#include "ringbuf.h"
#include <string.h>

/* 以申请内存方式初始化ringbuf */
int ringbuf_init(ringbuf_t *rb, uint16_t size)
{
    if (rb == NULL)
    {
        return -1;
    }

    rb->buffer_size = size;
    if (size > MAX_RINGBUF_SIZE)
    {
        rb->buffer_size = MAX_RINGBUF_SIZE;
    }

    rb->buffer_ptr = (uint8_t *)malloc(rb->buffer_size);
    if(rb->buffer_ptr == NULL)
    {
        return -2;
    }

    rb->read_index = 0;
    rb->write_index = 0;
    rb->is_full = RINGBUF_STATE_EMPTY;

    return 0;
}

/* 释放ringbuf申请的内存 */
int ringbuf_deinit(ringbuf_t *rb)
{
    if(rb == NULL || rb->buffer_ptr == NULL)
    {
        return 0;
    }
    free(rb->buffer_ptr);
    rb->buffer_ptr = NULL;
    rb->buffer_size = 0;
    rb->is_full = RINGBUF_STATE_EMPTY;
    rb->read_index = 0;
    rb->write_index = 0;
    return 0;
}

/* 写入指定大小数据, 过长的数据只会写入最大长度 */
int ringbuf_write(ringbuf_t *rb, const uint8_t *data, uint16_t length)
{
    /* 获取剩余可写大小 */
    uint16_t unwrite_size = ringbuf_unwrite_byte(rb);
    if(unwrite_size == 0 || rb == NULL)
    {
        /* 缓冲区已满 */
        return 0;
    }

    if(unwrite_size < length)
    {
        /* 丢弃过长数据 */
        rb->is_full = RINGBUF_STATE_FULL;
        length = unwrite_size;
    }

    /* 不需回环写入 */
    if((rb->buffer_size - rb->write_index) > length)
    {
        /* 剩余可写长读足够写入 */
        memcpy(&rb->buffer_ptr[rb->write_index], data, length);
        rb->write_index += length;
        return length;
    }

    /* 需要回环写入 */
    /* 先把write_index到尾部位置填满 */
    memcpy(&rb->buffer_ptr[rb->write_index], &data[0], rb->buffer_size - rb->write_index);
    /* 再把头部到read_index位置填满 */
    memcpy(&rb->buffer_ptr[0], &data[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));
    rb->write_index = length - (rb->buffer_size - rb->write_index);
    return length;
}

/* 读出指定大小数据 */
int ringbuf_read(ringbuf_t *rb, const uint8_t *data, uint16_t length)
{
    uint16_t unread_size = ringbuf_unread_byte(rb);
    if(rb == NULL || unread_size == 0)
    {
        return 0;
    }

    if(rb->is_full == RINGBUF_STATE_FULL)
    {
        rb->is_full = RINGBUF_STATE_EMPTY;
    }

    if(unread_size < length)
    {
        length = unread_size; 
    }

    /* 不需回环读出 */
    if((rb->buffer_size - rb->read_index) > length)
    {
        memcpy(data, &rb->buffer_ptr[rb->read_index], length);
        rb->read_index += length;
        return length;
    }

    /* 需要回环读出 */
    memcpy(&data[0], &rb->buffer_ptr[rb->read_index], rb->buffer_size - rb->read_index);
    memcpy(&data[rb->buffer_size - rb->read_index], &rb->buffer_ptr[0], length - (rb->buffer_size - rb->read_index));
    rb->read_index = length - (rb->buffer_size - rb->read_index);
    return length;
}

/* 缓冲区是否为空 */
ringbuf_state_t ringbuf_state_get(ringbuf_t *rb)
{
    return rb->is_full;
}

/* 缓冲区内未读数据大小 */
int ringbuf_unread_byte(ringbuf_t *rb)
{
    if(rb == NULL)
    {
        return 0;
    }

    if(ringbuf_state_get(rb) == RINGBUF_STATE_FULL)
    {
        return rb->buffer_size;
    }

    if (rb->write_index < rb->read_index)
    {
        return (rb->buffer_size - rb->write_index + rb->read_index);
    }
    else
    {
        return (rb->write_index - rb->read_index);
    }
}

/* 缓冲区可写数据大小 */
int ringbuf_unwrite_byte(ringbuf_t *rb)
{
    if(rb == NULL)
    {
        return 0;
    }

    if(ringbuf_state_get(rb) == RINGBUF_STATE_FULL)
    {
        return 0;
    }

    if (rb->write_index >= rb->read_index)
    {
        return (rb->buffer_size - rb->write_index + rb->read_index);
    }
    else
    {
        return (rb->read_index - rb->write_index);
    }
}