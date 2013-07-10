#ifndef __LIBFUGOU_BASE_H__
#define __LIBFUGOU_BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#endif

typedef unsigned int unt;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long int ulonglong;
typedef unsigned long long int long_size_t;
/* signed: LL, unsigned: ULL
 * format: "%lld", "%ulld"
 */

#define LOG_FORMAT ("[%s] [%s:%d:%s()] [%s] ")
#define FORMAT_TIMESTAMP "%04d-%02d-%02dT%02d:%02d:%02d.%06ld"
#define LOGGER2(level, ...) (logger2(__FILE__, __LINE__, __FUNCTION__, level, __VA_ARGS__))
#define DEBUG2(...) (_debug2(__FILE__, __LINE__, __FUNCTION__, DEBUG_, __VA_ARGS__))

enum _log_levels {
    DUMP, DEBUG_, INFO, WARN, ERROR, FATAL, BUG
};

void set_logger(FILE *log);
void vlogger(char *log_name, int level, const char *fmt, va_list ap);

#define EIGHT (8ULL)
#define OCTET EIGHT

#define SS_SIZE 80

typedef void (*code_function)(uchar *, uchar *, void *);
/*
void (*decrypt)(uchar *,uchar *,void *)
*/

size_t _encrypt_cbc(
    uchar *c,
    uchar *m,
    uchar *iv,
    void *key,
    size_t text_size,
    size_t block_size,
    code_function encrypt
);

size_t _decrypt_cbc(
    uchar *d,
    uchar *c,
    void *key,
    size_t cipher_size,
    size_t block_size,
    code_function decrypt
);

/* copied camellia/pycamellia.c */
typedef struct {
    size_t text_size;
    size_t cipher_size;
    size_t snip_size;
    size_t padding_size;
    size_t block_size;
} _cipher_size_brother_t;

#endif /* __LIBFUGOU_BASE_H__ */
