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

void test_rs_mul(void)
{
    reed_solomon_t *rs;
    /*
    assert_by_ushort(65581, rs[1].poly, "test_rs_mul() with _rs_mul16_for_test16()");
    assert_by_ushort(0, _rs_mul16_for_test(rs + 1, 0, 0), "test_rs_mul() with _rs_mul16_for_test16()");
    */
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

void test_rs_take_rs(void)
{
    int ret;
    reed_solomon_t *rs = NULL;
    uint bits, division;

    /* 正常系、good case */
    bits = 4, division = 15;
    ret = rs_take_rs(&rs, bits, division);
    assert_success(ret, "rs_take_rs() with test_rs_take_rs()");
    assert_by_not_null(rs, "rs_take_rs() with test_rs_take_rs()");
    assert_by_uint(RS_bits(rs), bits, "rs_take_rs() with bits");
    assert_by_uint(RS_poly(rs), 19, "rs_take_rs() with poly");
    assert_by_size(RS_symbol_size(rs), 1, "rs_take_rs() with symbol_size");
    assert_by_size(RS_register_size(rs), 2, "rs_take_rs() with register_size");
    assert_by_uint(RS_w(rs), (1 << bits), "rs_take_rs() with w");
    assert_by_uint(RS_gf_max(rs), (1 << bits) - 1, "rs_take_rs() gf_maxith gf_max");
    /*
    assert_by_uint(RS_gf(rs), gf, "rs_take_rs() gfith gf");
    assert_by_uint(RS_gfi(rs), gfi, "rs_take_rs() gfiith gfi");
    */
    assert_by_size(RS_gf_size(rs), RS_w(rs) * RS_register_size(rs), "rs_take_rs() gf_size");
    assert_by_size(RS_allocate_size(rs), RS_gf_size(rs) * 2, "rs_take_rs() allocate_size");
    /*
    assert_by_uint(RS_division(rs), division, "rs_take_rs() with division");
    */
}

void test_rs(void)
{
    test_rs_take_rs();
    /*
    test_take_rs();
    test_rs_add();
    test_rs_mul();
    */
}

int main(void)
{
    set_logger(stderr);
    set_logger_level(DEBUG_);
    /* -DDEBUG を付けて compile する事。
    _DEBUG("%s start!\n", __FILE__);
    */
    fprintf(stdout, "%s start!\n", __FILE__);

    rs_big_bang();
    test_rs();
    fprintf(stdout, "\n");
    rs_ultimate_fate_of_the_universe();

    fprintf(stdout, "%s done!\n", __FILE__);
    return 0;
}
