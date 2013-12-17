#include "rs.h"
#include "../assert_xxx.h"

/*
#define _rs_ADD(a, b) (a ^ b)
*/

ushort _rs_mul16_for_test(reed_solomon_t *rs, ushort a, ushort b);

void test_rs_add(void)
{
    assert_by_uint(0xa, _rs_ADD(0xc, 0x6), "test_rs_add()");
}

void test_rs_mul(reed_solomon_t *rs)
{
    assert_by_ushort(65581, rs[1].poly, "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(0, _rs_mul16_for_test(rs + 1, 0, 0), "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(0, _rs_mul16_for_test(rs + 1, 0, 0x1), "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(0, _rs_mul16_for_test(rs + 1, 0x1, 0), "test_rs_mul() with _rs_mul16_for_test16()");
    /*
    assert_by_ushort(1, _rs_mul16_for_test(rs + 1, 0x01, 0x01), "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(1, _rs_mul16_for_test(rs + 1, 0x01, 0xff), "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(1, _rs_mul16_for_test(rs + 1, 0xff, 0xff), "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(1, _rs_mul16_for_test(rs + 1, 0x8d, 0x4c), "test_rs_mul() with _rs_mul16_for_test16()");
    */
    assert_by_ushort(0xa, _rs_mul16_for_test(rs + 1, 0xc, 0x6), "test_rs_mul() with _rs_mul16_for_test16()");
    /*
    assert_by_uint(0, _rs_mul32(0, 0x2), "test_rs_mul() with _rs_mul32()");
    assert_by_uint(0, _rs_mul32(0x2, 0), "test_rs_mul() with _rs_mul32()");
    assert_by_uint(0xa, _rs_mul32(0xc, 0x6), "test_rs_mul() with _rs_mul32()");
    */
}

void test_rs(void)
{
    reed_solomon_t *rs;
    /*
    test_take_rs();
    */
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

    rs_big_bang();
    test_rs(rs);
    rs_ultimate_fate_of_the_universe();

    fprintf(_f, "%s done!\n", __FILE__);
    return 0;
}
