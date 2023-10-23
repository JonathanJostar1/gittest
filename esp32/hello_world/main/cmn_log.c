#include "cmn_log.h"
#include "ringbuf.h"
#include "time.h"

static uint8_t *log_timestamp_get(void)
{
    static char time_buf[20] = {0};
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(time_buf, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);

    return time_buf;
}

#define ESP_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==LOG_LEVEL_ERROR )          { log_write(LOG_LEVEL_ERROR, tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==LOG_LEVEL_WARN )      { log_write(LOG_LEVEL_WARN,  tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==LOG_LEVEL_DEBUG )     { log_write(LOG_LEVEL_DEBUG, tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
        else                                  { log_write(LOG_LEVEL_DEBUG, tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); } \
    } while(0)

void log_write(cmn_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list list;
    va_start(list, format);



    va_end(list);
}
