#include <stdio.h>
#include <stdarg.h>

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

void vlogger(char *log_name, int level, const char *fmt, va_list ap)
{
    /* like a vfprintf(), vsnprintf() */

    if (_log != NULL && level >= _log_level) {
        fprintf(_log, "[%s] [%s] ", log_name, _log_level_names[level]);
        vfprintf(_log, fmt, ap);
    }
}

static void slot_logger(const int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vlogger("slot", level, fmt, ap);
    va_end(ap);
}

int main(void)
{
    set_logger(stderr);
    set_logger_level(INFO);

    slot_logger(INFO, "hello world!\n");
    slot_logger(INFO, "I am logger.\n");
    return 0;
}
