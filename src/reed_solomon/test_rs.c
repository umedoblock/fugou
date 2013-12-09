#include "rs.h"
#include "../assert_xxx.h"

/*
#define _rs_ADD(a, b) (a ^ b)
*/

void test_rs_add(void)
{
    assert_by_uint(0xa, _rs_ADD(0xc, 0x6), "test_rs_add()");
}

ushort _rs_mul16(reed_solomon_t *rs, ushort a, ushort b);
void test_rs_mul(void)
{
    assert_by_ushort(0, _rs_mul16(NULL, 0, 0x1), "test_rs_mul() with _rs_mul16()");
    assert_by_ushort(0, _rs_mul16(NULL, 0x1, 0), "test_rs_mul() with _rs_mul16()");
    assert_by_ushort(0xa, _rs_mul16(NULL, 0xc, 0x6), "test_rs_mul() with _rs_mul16()");
    /*
    assert_by_uint(0, _rs_mul32(0, 0x2), "test_rs_mul() with _rs_mul32()");
    assert_by_uint(0, _rs_mul32(0x2, 0), "test_rs_mul() with _rs_mul32()");
    assert_by_uint(0xa, _rs_mul32(0xc, 0x6), "test_rs_mul() with _rs_mul32()");
    */
}

void test_rs(void)
{
    test_rs_add();
    test_rs_mul();
}

int main(void)
{
    set_logger(stderr);
    set_logger_level(DEBUG_);
    /* -DDEBUG を付けて compile する事。
    _DEBUG("%s start!\n", __FILE__);
    */
    fprintf(_f, "%s start!\n", __FILE__);

    test_rs();

    return 0;
}
