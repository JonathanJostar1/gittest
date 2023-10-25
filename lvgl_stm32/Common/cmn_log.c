#include "cmn_log.h"
#include "time.h"

static log_print_cb_t custom_print_cb = NULL;

static char log_buffer[LOG_BUFF_SIZE] = {0};     /* 日志缓冲buffer */
static const char *log_level_str[] = {
    [LOG_LEVEL_ERROR]   = "[E]",
    [LOG_LEVEL_WARN]    = "[W]",
    [LOG_LEVEL_INFO]    = "[I]",
    [LOG_LEVEL_DEBUG]   = "[D]",
};

#if TIMESTAMP_ADD_LOG
static char *log_timestamp_get(void)
{
    static char time_buf[20] = {0};
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(time_buf, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);

    return time_buf;
}
#endif
void log_write(uint8_t level, const char *tag, const char *file, int line, const char *format, ...)
{
    va_list arg;
    va_start (arg, format);


#if TIMESTAMP_ADD_LOG
    snprintf(log_buffer, LOG_BUFF_SIZE, "%s %s (%s:%d) %s: ", log_level_str[level], log_timestamp_get(), file, line, tag);
#else
    snprintf(log_buffer, LOG_BUFF_SIZE, "%s (%s:%d) %s: ", log_level_str[level], file, line, tag);
#endif
    int size = strlen(log_buffer);
    vsnprintf(&log_buffer[size], LOG_BUFF_SIZE - size, format, arg);
    strncat(log_buffer, "\r\n", LOG_BUFF_SIZE - 2);
    fflush(stdout);

#if LOG_TO_PRINTF
    printf("%s", log_buffer);
#endif

#if LOG_TO_USER_CUSTOM
    if(custom_print_cb != NULL)
    {
        custom_print_cb(log_buffer);
    }
#endif

    va_end(arg);
}

/* 注册自定义日志处理函数 */
void log_register_print_cb(log_print_cb_t print_cb)
{
    custom_print_cb = print_cb;
}
