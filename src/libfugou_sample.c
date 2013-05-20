#include "config.h"

#include "libfugou.h"

int main(int argc, char *argv[])
{
    CAMELLIA_KEY cm_, *cm = &cm_;
    uchar key[CAMELLIA_BLOCK_SIZE * 2];
    uchar m[CAMELLIA_BLOCK_SIZE];
    uchar c[CAMELLIA_BLOCK_SIZE];
    uchar d[CAMELLIA_BLOCK_SIZE];

    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    uchar digest[SHA1SUM_HASH_SIZE], hex[256];
    long_size_t bit_length;
    FILE *fp = NULL, *f = stdout;
    int i, cmp;

    for(i=0;argv[i]!=NULL;i++){
    }
    fprintf(f, "これは、%sの sample program です。\n", PACKAGE_NAME);
    fprintf(f, "%s から compile しました。\n", __FILE__);
    fprintf(f, "%s の使い方を調べる際は、 %s を参考にして下さい。\n\n",
                PACKAGE_NAME, __FILE__);

    bit_length = sha1(sha1sum, (uchar *)"abc", 3);
    sha1_get_hex(hex, sha1sum);
    fprintf(f, "sha1(\"abc\") =\n");
    fprintf(f, "%s\n", hex);
    fprintf(f, "bit 長 = %lld bits\n", bit_length);
    fprintf(f, "\n");

    fp = fopen(__FILE__, "rb");
    if (fp == NULL) {
        fprintf(f, "%s が見つかりませんでした。\n", __FILE__);
        return -1;
    }
    bit_length = sha1_as_file(sha1sum, fp);
    fclose(fp);
    sha1_get_hex(hex, sha1sum);
    fprintf(f, "sha1(file=\"%s\") =\n", __FILE__);
    fprintf(f, "%s\n", hex);
    fprintf(f, "bit 長 = %lld bits\n\n", bit_length);

    memset(key, 0x88, sizeof(key));
    memset(m, 0xc9, sizeof(m));
    camellia_keyset(cm, key, 256 /* 128 or 192 */);
    camellia_keygen(cm);
    camellia_encrypt(c, m, cm);
    camellia_decrypt(d, c, cm);
    fprintf(f, "平文 0x88 を 256 bit 長の対象鍵 0xc9 で暗号化して\n");
    fprintf(f, "復号分を得てみると、、、\n");
    cmp = memcmp(m, d, CAMELLIA_BLOCK_SIZE);
    if (m != d && cmp == 0) {
        fprintf(f, "平文と復号分が一致しました。\n");
    }
    else {
        fprintf(f, "平文と復号分が一致しませんでした。\n");
    }
    /* CBC mode についてはその内書く。*/

    return 0;
}
