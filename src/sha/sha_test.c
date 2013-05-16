#include "libsha.h"
#include "libsha_test.h"

void _sha1_dump_block_of_W(sha1sum_t *sha1sum)
{
    uint i;

    for (i=0;i<sizeof(sha1sum->W)/sizeof(uint);i++) {
        fprintf(stdout, "W[%2d] = 0x%08x\n", i, sha1sum->W[i]);
    }
}

void _sha1_dump_ABCDE_header(void)
{
    fprintf(stdout, "                 ");
    fprintf(stdout, "A          B          C          D          E\n");
}

void _sha1_dump_ABCDE(int t, uint a, uint b, uint c, uint d, uint e)
{
    fprintf(stdout, "t = %2d: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                     t, a, b, c, d, e);
}

void _sha1_load_appendix_A(sha1sum_t *sha1sum)
{
    uint *W = sha1sum->W;

    memset(sha1sum->W, '\0', sizeof(sha1sum->W));
    W[0] = 0x61626380;
    W[1] = 0x00000000;
    W[2] = 0x00000000;
    W[3] = 0x00000000;
    W[4] = 0x00000000;
    W[5] = 0x00000000;
    W[6] = 0x00000000;
    W[7] = 0x00000000;
    W[8] = 0x00000000;
    W[9] = 0x00000000;
    W[10] = 0x00000000;
    W[11] = 0x00000000;
    W[12] = 0x00000000;
    W[13] = 0x00000000;
    W[14] = 0x00000000;
    W[15] = 0x00000018;

    /* for canceling in _sha1_compute() */
    _sha1_normalize_endian(sha1sum);
}

void sha1_test_result(char *test_name, char *expected_hex, char *hex)
{
    int cmp;

    cmp = strcmp(expected_hex, hex);
    if (cmp == 0) {
        fprintf(stdout, ".");
    }
    else {
        fprintf(stdout, "%s =\n", test_name);
        fprintf(stdout, "expected_hex =\n");
        fprintf(stdout, "%s\n", expected_hex);
        fprintf(stdout, "hex =\n");
        fprintf(stdout, "%s\n", hex);
    }
}

void sha1_test_null(void)
{
    /*
    8e8832c642a6a38c74c17fc92ccedc266c108e6c  null_55.txt
    9438e360f578e12c0e0e8ed28e2c125c1cefee16  null_56.txt
    0f2bf6d5e1a0209d19f8f6e7d08b3e2d9cf4c5ab  null_57.txt

    0b8bf9fc37ad802cefa6733ec62b09d5f43a1b75  null_63.txt
    c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7  null_64.txt
    f0fa45906bd0f4c3668fcd0d8f68d4b298b30e5b  null_65.txt
    */
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    char hex[80];
    uchar null55[55] = {0};
    char *null55_digest = "8e8832c642a6a38c74c17fc92ccedc266c108e6c";
    uchar null56[56] = {0};
    char *null56_digest = "9438e360f578e12c0e0e8ed28e2c125c1cefee16";
    uchar null57[57] = {0};
    char *null57_digest = "0f2bf6d5e1a0209d19f8f6e7d08b3e2d9cf4c5ab";
    uchar null63[63] = {0};
    char *null63_digest = "0b8bf9fc37ad802cefa6733ec62b09d5f43a1b75";
    uchar null64[64] = {0};
    char *null64_digest = "c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7";
    uchar null65[65] = {0};
    char *null65_digest = "f0fa45906bd0f4c3668fcd0d8f68d4b298b30e5b";

    /*
    dump(null56, 56, 16);
    */
    sha1(sha1sum, null55, 55);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_null null 55()", null55_digest, hex);
    sha1(sha1sum, null56, 56);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_null null 56()", null56_digest, hex);
    sha1(sha1sum, null57, 57);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_null null 57()", null57_digest, hex);
    sha1(sha1sum, null63, 63);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_null null 63()", null63_digest, hex);
    sha1(sha1sum, null64, 64);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_null null 64()", null64_digest, hex);
    sha1(sha1sum, null65, 65);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_null null 65()", null65_digest, hex);
}

void sha1_test_appendix_A(void)
{
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    char hex[80];
    char *message_digest = "a9993e364706816aba3e25717850c26c9cd0d89d";

    sha1(sha1sum, (uchar *)"abc", 3);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_appendix_A()", message_digest, hex);
}

void sha1_test_appendix_B(void)
{
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    char hex[80];
    char *message_digest = "84983e441c3bd26ebaae4aa1f95129e5e54670f1";

    sha1(sha1sum, (uchar *)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_appendix_B()", message_digest, hex);
}

void sha1_test_appendix_C(void)
{
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    char hex[80];
    char *message_digest = "34aa973cd4c4daa4f61eeb2bdbad27316534016f";
    uchar *a1000000 = NULL;

    a1000000 = (uchar *)malloc(sizeof(uchar) * 1000 * 1000);
    memset(a1000000, 'a', 1000 * 1000);
    sha1(sha1sum, a1000000, 1000 * 1000);
    free(a1000000);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_appendix_C()", message_digest, hex);
}

void sha1_test_appendix_C_with__sha1_append(void)
{
    /* 432e7e01de7086c5246b6ac57f5f435b58f13752  a1000001.txt */
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    char hex[80];
    char *message_digest = "432e7e01de7086c5246b6ac57f5f435b58f13752";
    uchar a64[64];
    long_size_t n_blocks, n_remained, i;

    /*
    >>> 1000001 // 64
    15625
    >>> 1000001 % 64
    1
    */
    n_blocks = 1000001 / SHA1SUM_BLOCK_SIZE;
    n_remained = 1000001 % SHA1SUM_BLOCK_SIZE;

    memset(a64, 'a', sizeof(a64));

    _sha1_init(sha1sum);
    for (i=0ULL;i<n_blocks;i++) {
        _sha1_append(sha1sum,
                     a64,
                     SHA1SUM_BLOCK_SIZE);
    }
    _sha1_hatch(sha1sum, a64, n_remained);

    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_appendix_C_with__sha1_append()",
                     message_digest, hex);
}

void sha1_test_appendix_A_with_load(void)
{
    sha1sum_t sha1sum_, *sha1sum = &sha1sum_;
    char hex[80];
    char *init_message_digest = "67452301efcdab8998badcfe10325476c3d2e1f0";
    char *message_digest = "a9993e364706816aba3e25717850c26c9cd0d89d";

    _sha1_load_appendix_A(sha1sum);

    _sha1_init(sha1sum);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_appendix_A_with_load() init_message_digest",
                     init_message_digest, hex);

    _sha1_compute(sha1sum);
    sha1_get_hex(hex, sha1sum);
    sha1_test_result("sha1_test_appendix_A_with_load() message_digest",
                     message_digest, hex);
}

int main(void)
{
    sha1_test_appendix_A_with_load();
    sha1_test_appendix_A();
    sha1_test_appendix_B();
    sha1_test_appendix_C();
    sha1_test_appendix_C_with__sha1_append();

    sha1_test_null();

    return 0;
}
