#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
openssl dgst -sha1 abc.txt
SHA1(abc.txt)=   a9993e364706816aba3e25717850c26c9cd0d89d
SHA1(abc56.txt)= 84983e441c3bd26ebaae4aa1f95129e5e54670f1

../../omoide/tests/sha/checksum.txt
sha-1
abc.txt    a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d
abc

abc56.txt  84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1
abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq

a100.txt   34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f
'a' * 1000000

楽しおってけしからん！
import hashlib
m = hashlib.sha1()
m.update(b'a' * 1000000)
h = m.hexdigest()
print(h)
34aa973cd4c4daa4f61eeb2bdbad27316534016f
*/

typedef unsigned char uchar;
void view_hash(uchar *hash, int len_hash);
void sha1(uchar *hash, uchar *data, FILE *fp, int len_data);

int main(void)
{
    uchar hash[160/8];
    int i;
    char *msg[] = {
        "abc", "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        NULL, NULL
    };
    char *p;
    int len_data, len_a;

    uchar expected_hash[][20] = {
        {0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e,
         0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d},
        {0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e, 0xba, 0xae,
         0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5, 0xe5, 0x46, 0x70, 0xf1},
        {0x34, 0xaa, 0x97, 0x3c, 0xd4, 0xc4, 0xda, 0xa4, 0xf6, 0x1e,
         0xeb, 0x2b, 0xdb, 0xad, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6f},
    };

    len_a = 100 * 10000;
    p = malloc(len_a + 1);
    if (p == NULL)
        return -1;
    memset(p, 'a', len_a);
    p[len_a] = '\0';
    msg[2] = p;

    for (i=0;msg[i] != NULL;i++) {
        len_data = strlen(msg[i]);
        if (msg[i] != p)
            fprintf(stdout, "sha1(\"%s\")", msg[i]);
        else
            fprintf(stdout, "sha1(\"a\" * %d)", len_a);
        sha1(hash, (uchar *)msg[i], NULL, len_data);

        if (memcmp(expected_hash[i], hash, 160 / 8) == 0) {
            fprintf(stdout, " ok");
        }
        else {
            fprintf(stdout, " bad\n");
            fprintf(stdout, "expected ");
            view_hash(expected_hash[i], 160);
            fprintf(stdout, "\n");

            fprintf(stdout, "     but ");
            view_hash(hash, 160);
        }
        fprintf(stdout, "\n");
    }

    free(p);

    return 0;
}
