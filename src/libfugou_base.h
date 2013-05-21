#ifndef __LIBFUGOU_BASE_H__
#define __LIBFUGOU_BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef unsigned int unt;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long int ulonglong;
typedef unsigned long long int long_size_t;
/* signed: LL, unsigned: ULL
 * format: "%lld", "%ulld"
 */

enum _log_levels {
    DUMP, DEBUG_, INFO, WARN, ERROR, FATAL, BUG
};

#ifdef DEBUG
    #ifndef __LOG__
    #define __LOG__
    extern FILE *_log;
    extern int _log_level;
    extern const char *_log_level_names[];
    #endif /* #ifndef __LOG__ */
#endif /*#ifdef DEBUG */

#define EIGHT (8ULL)
#define OCTET EIGHT

#define BUFFER_SIZE 80

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

void logger(char *log_name, int level, char *fmt, ...);

/* copied camellia/pycamellia.c */
typedef struct {
    size_t text_size;
    size_t cipher_size;
    size_t snip_size;
    size_t padding_size;
    size_t block_size;
} _cipher_size_brother;

#endif /* __LIBFUGOU_BASE_H__ */
