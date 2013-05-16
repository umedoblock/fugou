#include "sha.h"

typedef unsigned char uchar;

void validation_hex(sha1sum_t *sha1sum, char *expected_hex, char *message)
{
    char hex[80];

    sha1_get_hex(hex, sha1sum);
    fprintf(stdout, "%s\n", message);
    if (strcmp(expected_hex, hex) == 0) {
        fprintf(stdout, "correct\n");
    }
    else {
        fprintf(stdout, "failed\n");
        fprintf(stdout, "%s\n", hex);
    }
}

int main(void)
{
    sha1sum_t sha1sum[1];
    char hex[80];
    uchar digest[80];
    char *random_1123_digest = "133354c387a9bb311add4f455f15bf7b41c753ca";
    uchar *random_1123 = NULL;
    size_t random_1123_size = 1123, read_size;
    long_size_t n_blocks, n_remained;
    FILE *fp = NULL;
    int ret;

    sha1(sha1sum, (uchar *)"abc", 3);
    sha1_get_hex(hex, sha1sum);
    fprintf(stdout, "sha1sum(abc) =\n");
    fprintf(stdout, "%s\n", hex);

    /* 133354c387a9bb311add4f455f15bf7b41c753ca  random_1123.bin
     */
    random_1123_size = 1123;
    random_1123 = (uchar *)malloc(random_1123_size);
    if (random_1123 == NULL) {
        fprintf(stderr, "cannot allocate memory of 1123 octets.");
        ret = -1;
        goto err_no_resource;
    }

    fp = fopen("random_1123.bin", "rb");
    if (fp == NULL) {
        fprintf(stderr, "cannot find random_1123.bin.");
        ret = -2;
        goto err_after_have_memory;
    }

    /* how many octets does read ?
     * 最小単位が sizeof(uchar) である OCTET 列を
     * random_1123_size 個、読み込める？という考えで、fread()を実行。
     */
    read_size = fread(random_1123, sizeof(uchar), random_1123_size, fp);
    if (read_size != random_1123_size) {
        fprintf(stderr, "read_size is %u.\n", read_size);
        fprintf(stderr, "cannot read random_1123 enough size.");
        ret = -3;
        goto err_after_have_memory_and_file;
    }

    /*************************************************************************/
    /*************************** main part ***********************************/
    /*************************************************************************/

    /* how to use sha1() */
    sha1(sha1sum, random_1123, random_1123_size);
    /* validation stage. */
    validation_hex(sha1sum, random_1123_digest,
                           "random_1123 sha1sum sha1() is");

    /* no use */
    sha1_get_digest(digest, sha1sum);

    /* here, no need to sha1sum and hex variant, so memset() break it. */
    memset(sha1sum, 0xff, sizeof(sha1sum_t));
    memset(hex, 0xff, sizeof(hex));

    /* how to use _sha1_append() */
    /* please see sha1_test_appendix_C_with__sha1_append() in libsha_test.c */
    /* but, not recommended to use it if not need to be special .*/
    n_blocks = random_1123_size / SHA1SUM_BLOCK_SIZE;
    n_remained = random_1123_size % SHA1SUM_BLOCK_SIZE;
    _sha1_init(sha1sum);
    _sha1_append(sha1sum, random_1123, SHA1SUM_BLOCK_SIZE * n_blocks);
    _sha1_hatch(sha1sum, random_1123 + SHA1SUM_BLOCK_SIZE * n_blocks,
                         n_remained);
    /* validation stage. */
    validation_hex(sha1sum, random_1123_digest,
                           "random_1123 sha1sum by _sha1_append() is");

    /* here, no need to sha1sum and hex variant, so memset() break it. */
    memset(sha1sum, 0xff, sizeof(sha1sum_t));
    memset(hex, 0xff, sizeof(hex));

    fseek(fp, 0L, SEEK_SET);
    sha1_as_file(sha1sum, fp);
    /* validation stage. */
    validation_hex(sha1sum, random_1123_digest,
                           "random_1123 sha1sum by sha1_as_file() is");

    /* success */
    ret = 0;

err_after_have_memory_and_file:
    fclose(fp);
err_after_have_memory:
    free(random_1123);
err_no_resource:

    return ret;
}
