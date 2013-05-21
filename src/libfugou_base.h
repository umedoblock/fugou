#ifndef __LIBFUGOU_BASE_H__
#define __LIBFUGOU_BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int unt;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long int ulonglong;
typedef unsigned long long int long_size_t;
/* signed: LL, unsigned: ULL
 * format: "%lld", "%ulld"
 */

enum _log_levels {
    DEBUG_, INFO, WARN, ERROR, FATAL, BUG
};

#define EIGHT (8ULL)
#define OCTET EIGHT

#define BUFFER_SIZE 80

size_t _encrypt_cbc(
    uchar *c,
    uchar *m,
    uchar *iv,
    void *key,
    size_t text_size,
    size_t block_size,
    void (*encrypt)(uchar *,uchar *,void *)
);

/* copied camellia/pycamellia.c */
typedef struct {
    size_t text_size;
    size_t cipher_size;
    size_t snip_size;
    size_t padding_size;
    size_t block_size;
} _cipher_size_brother;

#endif /* __LIBFUGOU_BASE_H__ */
