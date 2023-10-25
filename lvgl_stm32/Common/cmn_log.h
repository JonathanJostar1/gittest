#ifndef  __CMN_LOG_H_
#define  __CMN_LOG_H_

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include <stdint.h>

#define LOG_LEVEL_ERROR         0
#define LOG_LEVEL_WARN          1
#define LOG_LEVEL_INFO          2
#define LOG_LEVEL_DEBUG         3

#define LOG_BUFF_SIZE           64 /* 写一次日志缓冲区大小 */
#define TIMESTAMP_ADD_LOG       0   /* 是否开启时间戳 */
#define LOG_TO_PRINTF           1   /* 输出到串口 */
#define LOG_TO_USER_CUSTOM      0   /* 用户自定义输出，log_print_cb实现 */

/* 日志处理函数 */
typedef void (*log_print_cb_t)(const char * buff);
void log_write(uint8_t level, const char *tag, const char *file, int line, const char *format, ...);

#define CMN_LOGE(tag, ...) log_write(LOG_LEVEL_ERROR, tag, __FILE_, __LINE__, __VA_ARGS__)
#define CMN_LOGW(tag, ...) log_write(LOG_LEVEL_WARN,  tag, __FILE_, __LINE__, __VA_ARGS__)
#define CMN_LOGI(tag, ...) log_write(LOG_LEVEL_INFO,  tag, __FILE_, __LINE__, __VA_ARGS__)
#define CMN_LOGD(tag, ...) log_write(LOG_LEVEL_DEBUG, tag, __FILE_, __LINE__, __VA_ARGS__)

#endif // __CMN_LOG_H_