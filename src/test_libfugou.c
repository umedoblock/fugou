#include "libfugou.h"

void _validate(int success, char *test_name)
{
    if (success) {
        fprintf(stdout, ".");
    }
    else {
        fprintf(stderr, "failed %s\n", test_name);
    }
}

static void test_camellia_normed_size(void)
{
    int success;
    size_t i, text_size, encode_size;
    char msg[BUFFER_SIZE];

    for (i=0;i<=CAMELLIA_BLOCK_SIZE * 2;i++) {
        success = 0;
        text_size = i;
        encode_size = CAMELLIA_NORMED_SIZE(text_size);
        if (text_size < CAMELLIA_BLOCK_SIZE) {
            success = (encode_size == CAMELLIA_BLOCK_SIZE);
        }
        else if (text_size >= CAMELLIA_BLOCK_SIZE &&
                 text_size < CAMELLIA_BLOCK_SIZE * 2) {
            success = (encode_size == 2 * CAMELLIA_BLOCK_SIZE);
        }
        else if (text_size >= CAMELLIA_BLOCK_SIZE * 2) {
            success = (encode_size == 3 * CAMELLIA_BLOCK_SIZE);
        }
        snprintf(msg, BUFFER_SIZE, "test_camellia_normed_size(), "
                                   "text_size = %u, encode_size = %u",
                                    text_size, encode_size);
        _validate(success, msg);
    }
}

static void test(void)
{
    test_camellia_normed_size();
}

int main(void)
{
    test();
    return 0;
}
