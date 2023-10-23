#ifndef  __CMN_LOG_H_
#define  __CMN_LOG_H_

#include "stdio.h"
#include "stdarg.h"
#include "string.h"

typedef enum {
    LOG_LEVEL_NONE,       /*!< No log output */
    LOG_LEVEL_ERROR,      /*!< Critical errors, software module can not recover on its own */
    LOG_LEVEL_WARN,       /*!< Error conditions from which recovery measures have been taken */
    LOG_LEVEL_INFO,       /*!< Information messages which describe normal flow of events */
    LOG_LEVEL_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
} cmn_log_level_t;

void log_write(cmn_log_level_t level, const char *tag, const char *format, ...);


#define CMN_LOGE(tag, ...) log_write(ESP_LEVEL_ERROR, tag, format, ##__VA_ARGS__)
#define CMN_LOGW(tag, ...) log_write(ESP_LEVEL_WARN,  tag, format, ##__VA_ARGS__)
#define CMN_LOGI(tag, ...) log_write(ESP_LEVEL_INFO,  tag, format, ##__VA_ARGS__)
#define CMN_LOGD( ag, ...) log_write(ESP_LEVEL_DEBUG, tag, format, ##__VA_ARGS__)

#endif // __CMN_LOG_H_