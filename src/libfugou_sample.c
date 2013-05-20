#include "config.h"

#include "libfugou.h"

int main(int argc, char *argv[])
{
    CAMELLIA_KEY cm_, *cm = &cm_;
    uchar key[CAMELLIA_BLOCK_SIZE * 2];
    uchar m[CAMELLIA_BLOCK_SIZE];
    uchar c[CAMELLIA_BLOCK_SIZE];
    uchar d[CAMELLIA_BLOCK_SIZE];
    uchar *message, *cipher, *decipher, *iv;

    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    uchar digest[SHA1SUM_HASH_SIZE], hex[256];
    long_size_t bit_length;
    FILE *fp = NULL, *f = stdout;
    int i, cmp;
    size_t text_size = BUFFER_SIZE + 11, mem_size, encode_size, cipher_size;
    char *mem, *mem_;

    encode_size = CAMELLIA_NORMED_SIZE(text_size);
    cipher_size = CAMELLIA_BLOCK_SIZE + encode_size;
    mem_size = encode_size * 2 + cipher_size;
    mem = (char *)malloc(mem_size);

    if (mem == NULL) {
        fprintf(stderr, "cannot allocate mem_size %u\n", mem_size);
        return -1;
    }
    mem_ = mem;
    memset(mem, '\0', mem_size);

     message = mem; mem += encode_size;
          iv = mem; mem += cipher_size;
    decipher = mem; mem += encode_size;
      cipher = iv + CAMELLIA_BLOCK_SIZE;

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
    fprintf(f, "復号文を得てみると、、、\n");
    fprintf(f, "対象鍵が、\n");
    dump(key, sizeof(key), 16);
    fprintf(f, "その対象鍵の副鍵が、\n");
    dump(cm, sizeof(cm_), 16);
    fprintf(f, "平文が、\n");
    dump(m, sizeof(m), 16);
    fprintf(f, "暗号文が、\n");
    dump(c, sizeof(c), 16);
    fprintf(f, "復号文が、\n");
    dump(d, sizeof(d), 16);
    fprintf(f, "でしたので、\n平文と復号文が");
    cmp = memcmp(m, d, CAMELLIA_BLOCK_SIZE);
    if (m != d && cmp == 0) {
        fprintf(f, "一致しました。\n");
    }
    else {
        fprintf(f, "一致しませんでした。\n");
    }
    fprintf(f, "\n");

    memset(message, 0x33, text_size);
    memset(iv, 0x88, CAMELLIA_BLOCK_SIZE);
    memset(cipher, 0xbb, cipher_size - CAMELLIA_BLOCK_SIZE);
    memset(decipher, 0xdd, text_size);

    camellia_encrypt_cbc_DataData(cipher, message, iv, text_size, cm);
    camellia_decrypt_cbc_DataData(decipher, cipher, iv, cipher_size, cm);
    fprintf(f, "平文 0x33 を 256 bit 長の対象鍵 0xc9 で "
               "CBC mode にて暗号化して\n");
    fprintf(f, "復号文を得てみると、、、\n");
    fprintf(f, "対象鍵が、\n");
    dump(key, sizeof(key), 16);
    fprintf(f, "その対象鍵の副鍵が、\n");
    dump(cm, sizeof(cm_), 16);
    fprintf(f, "平文が、\n");
    dump(message, encode_size, 16);
    fprintf(f, "初期化 vector が、\n");
    dump(iv, CAMELLIA_BLOCK_SIZE, 16);
    fprintf(f, "暗号文が、\n");
    dump(cipher, cipher_size - CAMELLIA_BLOCK_SIZE, 16);
    fprintf(f, "復号文が、\n");
    dump(decipher, encode_size, 16);
    fprintf(f, "でしたので、\n平文と復号文が");
    cmp = memcmp(message, decipher, text_size);
    if (message != decipher && cmp == 0) {
        fprintf(f, "一致しました。\n");
    }
    else {
        fprintf(f, "一致しませんでした。\n");
    }

    free(mem_);

    return 0;
}
