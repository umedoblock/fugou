#include <stdio.h>
#include <stdarg.h>
#include <time.h>
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

#if 0
       int gettimeofday(struct timeval *tv, struct timezone *tz);

       int settimeofday(const struct timeval *tv, const struct timezone *tz);

   glibc 向けの機能検査マクロの要件 (feature_test_macros(7) 参照):

       settimeofday(): _BSD_SOURCE

説明
       関数  gettimeofday() と settimeofday() は時刻とタイムゾーンを取得または
       設定する。 tv 引き数は struct timeval である (<sys/time.h> で定義されて
       いる):

           struct timeval {
               time_t      tv_sec;     /* 秒 */
               suseconds_t tv_usec;    /* マイクロ秒 */
           };

       これにより紀元 (the Epoch: time(2) を参照) からの秒とマイクロ秒が取得で
       きる。 tz 引き数は struct timezone である:

#endif

#define FORMAT_TIMESTAMP "%04d-%02d-%02dT%02d:%02d:%02d.000000"

/* ~/repos/hg/p2p/umatobi/trunk/sim/basic/log.c p2p_CmnLog() */
size_t current_isoformat_time(char *ts, size_t ts_size)
{
    time_t timer;
    struct tm *t_st;
    int len = 0;

    //現在の時刻を取得
    time(&timer);
    /* windows に gettimeofday() がなさそうなので。。。
    struct timeval tv_, *tv = tv_;
    gettimeofday(tv, NULL);
    */
    //現在の時刻を構造体用に変換
    t_st = localtime(&timer);

    /* '2012-11-02T23:22:27.002481' */
    len += snprintf(ts, ts_size, FORMAT_TIMESTAMP,
        t_st->tm_year + 1900,
        t_st->tm_mon + 1,
        t_st->tm_mday,
        t_st->tm_hour,
        t_st->tm_min,
        t_st->tm_sec
    );

    return len;
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

#define SS_SIZE (80)

int main(void)
{
    int i = 1000;
    char ss[SS_SIZE];

    set_logger(stderr);
    set_logger_level(INFO);

    SLOT_LOGGER(INFO, "hello world!\n");
    SLOT_LOGGER(INFO, "I am logger.\n");
    SLOT_LOGGER(INFO, "i=%d.\n", i);

    current_isoformat_time(ss, SS_SIZE);
    fprintf(stdout, "current_isoformat_time()=\"%s\"\n", ss);

    return 0;
}
