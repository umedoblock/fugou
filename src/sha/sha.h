/* libsha.h
 * Copyright (C) 平成25(2013)年 梅どぶろく(umedoblock)
 * FIP180 SECURE HASH STANDARD
 * refs: http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

/* sha1.c が糞過ぎるため再実装する事にした。
 * 本当に終わっている、再利用なんて出来たもんじゃない。
 * 手入れするのも憚られるぐらいだ。
 * 触りたくない、だってウンコだもの。
 * まあ、過去頑張っていたって事で残しはしますが。。。
 */

#include "../libfugou_base.h"
#include "sha_private.h"

#ifndef SHA_H
#ifdef __cplusplus
extern "C" {
#endif

#define SHA1SUM_HASH_SIZE (SHA1SUM_HASH_BITS / OCTET)
#define SHA1SUM_BLOCK_SIZE (SHA1SUM_BLOCK_BITS / OCTET)

typedef struct {
    uchar value[SHA1SUM_HASH_SIZE]; /* convert H[5] to octets stream. */
    uint H[5];  /* see fip180-1.html */
    uint W[80]; /* see fip180-1.html */
    ulonglong message_size; /* sha1 hash text_size */
    ulonglong length; /* length = message_size * OCTET */
} sha1sum_t;

/*****************************************************************************/
/* below for library user ****************************************************/
/*****************************************************************************/
long_size_t sha1(sha1sum_t *sha1sum, uchar *text, long_size_t text_size);
long_size_t sha1_as_file(sha1sum_t *sha1sum, FILE *fp);
void sha1_get_digest(uchar *digest, sha1sum_t *sha1sum);
void sha1_get_hex(char *hex, sha1sum_t *sha1sum);

/*****************************************************************************/
/* below for developer using *************************************************/
/*****************************************************************************/
void _sha1_init(sha1sum_t *sha1sum);
long_size_t _sha1_feed(sha1sum_t *sha1sum, uchar *text,
                                           long_size_t remained_text_size);
long_size_t _sha1_hatch(sha1sum_t *sha1sum, uchar *text,
                                            long_size_t remained_text_size);
long_size_t _sha1_append(sha1sum_t *sha1sum,
                         uchar *append, long_size_t append_size);

#ifdef __cplusplus
}
#endif
#endif /* SHA_H */
