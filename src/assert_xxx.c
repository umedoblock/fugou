#include "libfugou.h"
#include "assert_xxx.h"

int failed(int success, char *test_name)
{
    int fail;
    if (success) {
        fprintf(stdout, ".");
        fail = 0;
    }
    else {
        fprintf(_f, "[FAILED] %s\n", test_name);
        fail = 1;
    }
    return fail;
}

void assert_by_00(uchar *result,
                  size_t zero_size,
                  char *test_name)
{
    int success = 1;
    size_t i;

    for (i=0;i<zero_size;i++) {
        if (result[i] != 0x00) {
            success = 0;
            break;
        }
    }

    if (failed(success, test_name)) {
        dump(result, zero_size, 16);
        fprintf(_f, "expected 0x00 * %zu, but result[%zu] is 0x%02x.\n",
                     zero_size, i, result[i]);
        fprintf(_f, "\n");
    }
}

void assert_by_mem(uchar *expected,
                   uchar *result,
                   size_t mem_size,
                   char *test_name)
{
    int cmp = memcmp(expected, result, mem_size);
    int success = (cmp == 0);
    if (failed(success, test_name)) {
        fprintf(_f, "expected=\n");
        dump(expected, mem_size, 16);
        fprintf(_f, "\n");
        fprintf(_f, "result=\n");
        dump(result, mem_size, 16);
        fprintf(_f, "\n");
    }
}

void assert_by_str(char *expected,
                   char *result,
                   char *test_name)
{
    int cmp = strcmp(expected, result);
    int success = (cmp == 0);
    if (failed(success, test_name)) {
        fprintf(_f, "expected=\"%s\"\n  result=\"%s\"\n", expected, result);
    }
}

void assert_by_null(void *result,
                        char *test_name)
{
    int success = result == NULL;
    if (failed(success, test_name)) {
        fprintf(_f, "result=%p is not NULL\n", result);
    }
}

void assert_by_not_null(void *result,
                        char *test_name)
{
    int success = result != NULL;
    if (failed(success, test_name)) {
        fprintf(_f, "result=%p\n", result);
    }
}

void assert_by_address(void *expected,
                       void *result,
                       char *test_name)
{
    int success = expected == result;
    if (failed(success, test_name)) {
        fprintf(_f, "expected=%p, result=%p\n", expected, result);
    }
}

void assert_by_size(size_t expected,
                    size_t result,
                    char *test_name)
{
    int success = expected == result;
    if (failed(success, test_name)) {
        fprintf(_f, "expected=%zu, result=%zu\n", expected, result);
    }
}

void assert_by_uint(uint expected,
                    uint result,
                    char *test_name)
{
    assert_by_size((size_t )expected, (size_t )result, test_name);
}

void assert_by_ushort(ushort expected,
                      ushort result,
                      char *test_name)
{
    assert_by_size((size_t )expected, (size_t )result, test_name);
}

void assert_success(int ret, char *test_name)
{
    failed(ret == SUCCESS, test_name);
}

void assert_true(int success, char *test_name)
{
    failed(success, test_name);
}

