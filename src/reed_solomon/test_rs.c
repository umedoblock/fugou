#include "rs.h"
#include "../assert_xxx.h"

/*
#define _rs_ADD(a, b) (a ^ b)
*/

#define TEMPORARY_SIZE (1024 * 1024)
static char *temporary = NULL;

ushort _rs_mul16_wrap(reed_solomon_t *rs, ushort a, ushort b);
big_bang_t *_rs_get_universe_wrap(void);
void _rs_make_e_matrix_wrap(_ptr_t e_matrix,
                                 size_t register_size,
                                 uint division);

void test_rs_add(void)
{
    assert_by_uint(0xa, _rs_ADD(0xc, 0x6), "test_rs_add()");
}

void test_rs_mul(void)
{
    reed_solomon_t *rs16 = NULL;
    uint bits, division;
    ushort expected, exponent_in_gf16;

    bits = 16, division = 1000;
    assert_by_null(rs16, "test_rs_mul() with _rs_mul16_wrap16()");
    rs_take_rs(&rs16, bits, division);
    assert_by_not_null(rs16, "test_rs_mul() with _rs_mul16_wrap16()");
    assert_by_uint(65581, RS_poly(rs16), "test_rs_mul() with _rs_mul16_wrap16()");
    assert_by_ushort(0, _rs_mul16_wrap(rs16, 0, 0), "test_rs_mul() with _rs_mul16_wrap16()");
    assert_by_ushort(0, _rs_mul16_wrap(rs16, 0, 0x1), "test_rs_mul() with _rs_mul16_wrap16()");
    assert_by_ushort(0, _rs_mul16_wrap(rs16, 0x1, 0), "test_rs_mul() with _rs_mul16_wrap16()");
    exponent_in_gf16 = (RS_gf16(rs16)[0x28a1] + RS_gf16(rs16)[0x7cce]) % RS_gf_max(rs16);
    expected = RS_gfi16(rs16)[exponent_in_gf16];
    assert_by_ushort(expected, _rs_mul16_wrap(rs16, 0x28a1, 0x7cce), "test_rs_mul() with _rs_mul16_wrap16()");

    /*
    assert_by_uint(0, _rs_mul32(0, 0x2), "test_rs_mul() with _rs_mul32()");
    assert_by_uint(0, _rs_mul32(0x2, 0), "test_rs_mul() with _rs_mul32()");
    assert_by_uint(0xa, _rs_mul32(0xc, 0x6), "test_rs_mul() with _rs_mul32()");
    */
}

void test_rs_div(void)
{
    reed_solomon_t *rs16 = NULL;
    uint bits, division;
    ushort expected, exponent_in_gf16;

    bits = 16, division = 1000;
    assert_by_null(rs16, "test_rs_div() with _rs_div16_wrap16()");
    rs_take_rs(&rs16, bits, division);
    assert_by_not_null(rs16, "test_rs_div() with _rs_div16_wrap16()");
    assert_by_uint(65581, RS_poly(rs16), "test_rs_div() with _rs_div16_wrap16()");
    assert_by_ushort(0, _rs_div16_wrap(rs16, 0, 0x1), "test_rs_div() with _rs_div16_wrap16()");
    /* whe b div a then _rs_div16(a, b) NEVER accept b of value of zero. */
    /* below sentence occur FATAL error.
    _rs_div16_wrap(rs16, 0x1, 0)
    */
    exponent_in_gf16 = RS_gf16(rs16)[0x28a1] - RS_gf16(rs16)[0x7cce];
    if (RS_gf16(rs16)[0x28a1] < RS_gf16(rs16)[0x7cce]) {
        exponent_in_gf16 += RS_gf_max(rs16);
    }
    expected = RS_gfi16(rs16)[exponent_in_gf16];
    assert_by_ushort(expected, _rs_div16_wrap(rs16, 0x28a1, 0x7cce), "test_rs_div() with _rs_div16_wrap16()");
}

void test_rs_take_rs(void)
{
    char ss[SS_SIZE];
    int ret, i;
    reed_solomon_t *rs4 = NULL;
    ushort bits4_gfi16[] = {0x01, 0x02, 0x04, 0x08, 0x03, 0x06, 0x0c, 0x0b,
    0x05, 0x0a, 0x07, 0x0e, 0x0f, 0x0d, 0x09};
    uint bits, division;

    /* 正常系、good case */
    bits = 4, division = 15;
    assert_by_null(rs4, "rs_take_rs(&rs4) with value of rs4");
    ret = rs_take_rs(&rs4, bits, division);
    assert_success(ret, "rs_take_rs(&rs4) with ret");
    assert_by_not_null(rs4, "rs_take_rs(&rs4) with value of rs4");
    assert_by_not_null(RS_gf16(rs4), "rs_take_rs(&rs4) with gf16");
    assert_by_not_null(RS_gfi16(rs4), "rs_take_rs(&rs4) with gfi16");
    assert_by_uint(bits, RS_bits(rs4), "rs_take_rs(&rs4) with bits");
    assert_by_uint(19, RS_poly(rs4), "rs_take_rs(&rs4) with poly");
    assert_by_size(1, RS_symbol_size(rs4), "rs_take_rs(&rs4) with symbol_size");
    assert_by_size(2, RS_register_size(rs4), "rs_take_rs(&rs4) with register_size");
    assert_by_uint((1 << bits), RS_w(rs4), "rs_take_rs(&rs4) with w");
    assert_by_uint(RS_w(rs4) - 1, RS_gf_max(rs4), "rs_take_rs(&rs4) with gf_max");
    for (i=0;i<RS_gf_max(rs4);i++) {
        sprintf(ss, "rs_take_rs(&rs4) gfi16[%d]\n", i);
        assert_by_ushort(bits4_gfi16[i], RS_gfi16(rs4)[i], ss);
        sprintf(ss, "rs_take_rs(&rs4) RS_gf16(rs4)[RS_gfi16(rs4)[%d]]\n", i);
        assert_by_ushort(i, RS_gf16(rs4)[RS_gfi16(rs4)[i]], ss);
    }

    assert_by_size(RS_w(rs4) * RS_register_size(rs4), RS_gf_size(rs4), "rs_take_rs(&rs4) with gf_size");
    assert_by_size(RS_gf_size(rs4) * 2, RS_allocate_size(rs4), "rs_take_rs(&rs4) with allocate_size");
}

void test_rs_take_rs_failed(void)
{
    char ss[SS_SIZE];
    int ret, i;
    reed_solomon_t *rs = NULL;
    uint bits, division;

    /* 失敗系、bad case */
    bits = 100, division = 15;
    ret = rs_take_rs(&rs, bits, division);
    assert_true(ret == RS_INVALID_BITS_ERROR, "rs_take_rs(&rs) with bits=100");

    bits = 4, division = 1;
    ret = rs_take_rs(&rs, bits, division);
    assert_true(ret == RS_INVALID_DIVISION_ERROR, "rs_take_rs(&rs) with division=1");
    bits = 4, division = 16;
    ret = rs_take_rs(&rs, bits, division);
    assert_true(ret == RS_INVALID_DIVISION_ERROR, "rs_take_rs(&rs) with division=16");
}

/*
 * rs_big_bang(), rs_ultimate_fate_of_the_universe()
 * の組み合わせによってreed solomon code library
 * 使用に必要な資源確保・解放を行う。
 */
void test_rs_big_bang_and_rs_ultimate_fate_of_the_universe(void)
{
    int ret;
    big_bang_t *_universe;
    char *mem;

 /*
  * rs_big_bang() を呼び出す前の初期状態の確認。
  * mem が NULL であり、
  * mem_status が BB_MEM_NO_ALLOCATE である事
  * を確認する。
  */
    _universe = _rs_get_universe_wrap();

    assert_by_null(_universe->mem, "rs_big_bang()");
    assert_true(_universe->mem_status == BB_MEM_NO_ALLOCATE, "rs_big_bang()");

 /*
  * rs_big_bang() の呼び出しによって、
  * mem が有効な address(!=NULL) に変わり、
  * mem_status が BB_MEM_ALLOCATED に変わっている事
  * を確認する。
  */
    ret = rs_big_bang();
    assert_true(ret == RS_SUCCESS, "rs_big_bang()");
    assert_by_not_null(_universe->mem, "rs_big_bang()");
    assert_true(_universe->mem_status == BB_MEM_ALLOCATED, "rs_big_bang()");
    mem = _universe->mem;

    ret = rs_ultimate_fate_of_the_universe();
    assert_true(ret == RS_SUCCESS, "rs_ultimate_fate_of_the_universe()");
    /* no change and not NULL */
    assert_by_address(mem, _universe->mem, "rs_ultimate_fate_of_the_universe()");
    assert_true(_universe->mem_status == BB_MEM_FREED, "rs_ultimate_fate_of_the_universe()");
    /* ここまでで、正常系の一連の処理は実行した。
     * rs_big_bang() => rs_ultimate_fate_of_the_universe()
     */

    ret = rs_big_bang();
    mem = _universe->mem;
    assert_true(ret == RS_SUCCESS, "rs_big_bang()");
    assert_true(_universe->mem_status == BB_MEM_ALLOCATED, "rs_big_bang()");
    ret = rs_big_bang();
    assert_true(ret == RS_BIG_BANG_ERROR, "rs_big_bang()");
    assert_by_address(mem, _universe->mem, "rs_ultimate_fate_of_the_universe()");
    assert_true(_universe->mem_status == BB_MEM_ALLOCATED, "rs_big_bang()");

    ret = rs_ultimate_fate_of_the_universe();
    assert_true(ret == RS_SUCCESS, "rs_ultimate_fate_of_the_universe()");
    assert_by_not_null(_universe->mem, "rs_ultimate_fate_of_the_universe()");
    assert_true(_universe->mem_status == BB_MEM_FREED, "rs_big_bang()");

    ret = rs_ultimate_fate_of_the_universe();
    assert_true(ret == RS_FATE_ERROR, "rs_ultimate_fate_of_the_universe()");
    assert_by_not_null(_universe->mem, "rs_ultimate_fate_of_the_universe()");
    assert_true(_universe->mem_status == BB_MEM_FREED, "rs_big_bang()");

    _universe->mem_status = BB_MEM_NO_ALLOCATE;
    _universe->mem = NULL;
}

/*
void test_rs_solve_inverse()
_rank_matrix(void)
*/

void test_invalid_rank_matrix(void)
{
    reed_solomon_t *rs4 = NULL;

    rs_take_rs(&rs4, 4, 4);

    /*
    _rs_mul_matrix_vector16();
    */
}

void test_rs_make_e_matrix(void)
{
    uint division = 4;
    _ptr_t e_matrix;
    int i, j;
    char msg[SS_SIZE];

    e_matrix.ptr = temporary;

    memset(temporary, 0xff, TEMPORARY_SIZE);
    _rs_make_e_matrix_wrap(e_matrix, 2, division);
    for (j=0;j<division;j++) {
        for (i=0;i<division;i++) {
            sprintf(msg, "test_rs_make_e_matrix(u16)(j, i)=(%d, %d)", j, i);
            if (j == i) {
            assert_by_ushort(1, e_matrix.u16[j * division + i], msg);
            }
            else {
            assert_by_ushort(0, e_matrix.u16[j * division + i], msg);
            }
        }
    }

    memset(temporary, 0xff, TEMPORARY_SIZE);
    _rs_make_e_matrix_wrap(e_matrix, 4, division);
    for (j=0;j<division;j++) {
        for (i=0;i<division;i++) {
            sprintf(msg, "test_rs_make_e_matrix(u32)(j, i)=(%d, %d)", j, i);
            if (j == i) {
            assert_by_uint(1, e_matrix.u32[j * division + i], msg);
            }
            else {
            assert_by_uint(0, e_matrix.u32[j * division + i], msg);
            }
        }
    }
}

void test_rs(void)
{
    test_rs_big_bang_and_rs_ultimate_fate_of_the_universe();

    rs_big_bang();

    test_rs_take_rs();
    test_rs_take_rs_failed();

    test_rs_add();
    test_rs_mul();

    test_rs_make_e_matrix();
    test_rs_invalid_rank_matrix();

    rs_ultimate_fate_of_the_universe();
}

/*
void test_rs_solve_inverse()        正常系
void test_rs_mul_matrixes()        正常系
void test_rs_invalid_rank_matrix(void) 異常系
*/

int main(void)
{
    set_logger(stderr);
    set_logger_level(DEBUG_);
    /* -DDEBUG を付けて compile する事。
    _DEBUG("%s start!\n", __FILE__);
    */
    temporary = malloc(TEMPORARY_SIZE);
    if (temporary == NULL) {
        fprintf(stderr, "cannot malloc() enough memory space.\n");
        return -1;
    }

    fprintf(stdout, "%s start!\n", __FILE__);

    test_rs();
    fprintf(stdout, "\n");

    fprintf(stdout, "%s done!\n", __FILE__);

    free(temporary);

    return 0;
}
