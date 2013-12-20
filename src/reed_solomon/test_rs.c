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
    char ss[SS_SIZE];
    int ret, i;
    reed_solomon_t *rs = NULL;
    ushort bits4_gfi16[] = {0x01, 0x02, 0x04, 0x08, 0x03, 0x06, 0x0c, 0x0b, 0x05, 0x0a, 0x07, 0x0e, 0x0f, 0x0d, 0x09};
    uint bits, division;

    /* 正常系、good case */
    bits = 4, division = 15;
    ret = rs_take_rs(&rs, bits, division);
    assert_success(ret, "rs_take_rs() with test_rs_take_rs()");
    assert_by_not_null(rs, "rs_take_rs() with test_rs_take_rs()");
    assert_by_uint(bits, RS_bits(rs), "rs_take_rs() with bits");
    assert_by_uint(19, RS_poly(rs), "rs_take_rs() with poly");
    assert_by_size(1, RS_symbol_size(rs), "rs_take_rs() with symbol_size");
    assert_by_size(2, RS_register_size(rs), "rs_take_rs() with register_size");
    assert_by_uint((1 << bits), RS_w(rs), "rs_take_rs() with w");
    assert_by_uint(RS_w(rs) - 1, RS_gf_max(rs), "rs_take_rs() gf_maxith gf_max");
    for (i=0;i<RS_gf_max(rs);i++) {
        sprintf(ss, "rs_take_rs() gfi16[%d]\n", i);
        assert_by_ushort(RS_gfi16(rs)[i], bits4_gfi16[i], ss);
        sprintf(ss, "rs_take_rs() RS_gf16(rs)[RS_gfi16(rs)[%d]]\n", i);
        assert_by_ushort(RS_gf16(rs)[RS_gfi16(rs)[i]], i, ss);
/*
        gf.u32[bit_pattern] = i;
        gfi.u32[i] = bit_pattern;
************************************
         = i;
        gfi.u32[gf.u32[bit_pattern]] = bit_pattern;
************************************
        gf.u32[gfi.u32[i]] = i; oooooooooooooooooooooooooooo
         = bit_pattern;
*/
    }

    assert_by_size(RS_w(rs) * RS_register_size(rs), RS_gf_size(rs), "rs_take_rs() gf_size");
    assert_by_size(RS_gf_size(rs) * 2, RS_allocate_size(rs), "rs_take_rs() allocate_size");
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
