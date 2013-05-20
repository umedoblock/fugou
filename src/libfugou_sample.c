#include "config.h"

#include "libfugou.h"

int main(int argc, char *argv[])
{
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    uchar digest[SHA1SUM_HASH_SIZE], hex[256];
    long_size_t bit_length;
    FILE *fp = NULL, *f = stdout;
    int i;

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
    fprintf(f, "bit 長 = %lld bits\n", bit_length);

    return 0;
}
