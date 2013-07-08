#include <stdio.h>
#include <stdarg.h>
/*
#include <varargs.h>
*/

enum _log_levels {
    DUMP, DEBUG_, INFO, WARN, ERROR, FATAL, BUG
};

const char *_log_level_names[] = {
    "DUMP", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "BUG"
};

FILE *_log = NULL;
int _log_level = DEBUG_;

void set_logger(FILE *log)
{
    _log = log;
}

void set_logger_level(int log_level)
{
    _log_level = log_level;
}

void logger(char *log_name, int level, char *fmt, ...)
{
    va_list ap;

    if (_log != NULL && level >= _log_level) {
        fprintf(_log, "[%s] [%s] ", log_name, _log_level_names[level]);
        /*
        n = vsnprintf(p, size, fmt, ap);
        */
        va_start(ap, fmt);
        vfprintf(_log, fmt, ap);
        va_end(ap);
    }
}

#define LOG_FORMAT ("[%s:%d:in %s()] [%s] ")

void vlogger(char *__file__, int __line__, const char *_func_, int level, char *fmt, va_list ap)
{
    /* like a vfprintf(), vsnprintf() */

    if (_log != NULL && level >= _log_level) {
        fprintf(_log, LOG_FORMAT, __file__, __line__, _func_,
                                        _log_level_names[level]);
        vfprintf(_log, fmt, ap);
    }
}

void slot_logger(char *__file__, int __line__, const char *_func_, int level, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vlogger(__file__, __line__, _func_, level, fmt, ap);
    va_end(ap);
}

#define SLOT_LOGGER(level, ...) (slot_logger(__FILE__, __LINE__, __func__, level, __VA_ARGS__))

#if 0
static void info(const char *filename, const int lineno, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vlogger("slot", level, fmt, ap);
    va_end(ap);
}
#endif

int main(void)
{
    int i = 1000;

    set_logger(stderr);
    set_logger_level(INFO);

    SLOT_LOGGER(INFO, "hello world!\n");
    SLOT_LOGGER(INFO, "I am logger.\n");
    SLOT_LOGGER(INFO, "i=%d.\n", i);

    return 0;
}
