#ifndef __ASSERT_XXX_H__
#define __ASSERT_XXX_H__

#include "libfugou_base.h"

#define _f (stderr)

int failed(int success, char *test_name);
void assert_by_00(uchar *result,
                  size_t zero_size,
                  char *test_name);
void assert_by_mem(uchar *expected,
                   uchar *result,
                   size_t mem_size,
                   char *test_name);
void assert_by_str(char *expected,
                   char *result,
                   char *test_name);
void assert_by_null(void *result,
                        char *test_name);
void assert_by_not_null(void *result,
                        char *test_name);
void assert_by_address(void *expected,
                       void *result,
                       char *test_name);
void assert_by_size(size_t expected,
                    size_t result,
                    char *test_name);
void assert_by_uint(uint expected,
                    uint result,
                    char *test_name);
void assert_by_ushort(ushort expected,
                      ushort result,
                      char *test_name);
void assert_success(int ret, char *test_name);
void assert_true(int success, char *test_name);
#endif
