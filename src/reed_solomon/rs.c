/* Copyright (C) 平成20-25(2008-2013)年 梅濁酒(umedoblock)
 * rs library source
 */

#include "rs.h"

/*****************************************************************************/
/* API ***********************************************************************/
/*****************************************************************************/

/* rs.c **********************************************************************/
int rs_big_bang(void)
{
    big_bang_t *universe = _rs_bright();
    int ret;

    if (universe->mem_status == BB_MEM_ALLOCATED) {
        LOGGER(ERROR, "try to call rs_big_bang() after run rs_big_bang(). must call rs_big_bang() just one time.\n");
        return RS_BIG_BANG_ERROR;
    }

    ret = _rs_init_the_universe(universe);
    if (ret < 0) {
        LOGGER(ERROR, "error occured with ret=%d in rs_big_bang().\n", ret);
        return ret;
    }
    universe->mem_status = BB_MEM_ALLOCATED;
    _rs_init_gf_gfi(universe);

    return RS_SUCCESS;
}

int rs_ultimate_fate_of_the_universe(void)
{
    big_bang_t *universe = _rs_bright();
    int ret;

    if (universe->mem_status == BB_MEM_ALLOCATED) {
        ret = RS_SUCCESS;
        free(universe->mem);
        universe->mem_status = BB_MEM_FREED;
    }
    else {
        ret = RS_FATE_ERROR;
        LOGGER(ERROR, "try to free(universe->mem) when universe->mem_status(=%d) is not appropriate value(=BB_MEM_ALLOCATED, 2).\nmust call rs_big_bang() before call rs_ultimate_fate_of_the_universe().\n", universe->mem_status);
    }
    return ret;
}

int rs_take_rs(reed_solomon_t **rs, uint bits, uint division)
{
    reed_solomon_t *rs_;

    rs_ = _rs_get_rs(bits);
    if (rs_ == NULL) {
        LOGGER(ERROR, "bits(=%u) must chose 4, 8, 16 or 24 for bits.\n",
                            bits);
        return RS_INVALID_BITS_ERROR;
    }
    if (division < 2 || division > rs_->gf_max) {
        LOGGER(ERROR, "division(=%u) must be 2 <= division <= %u.\n",
                            division, rs_->gf_max);
        return RS_INVALID_DIVISION_ERROR;
    }
    *rs = rs_;

    return RS_SUCCESS;
}

void _rs_view_rse(rs_encode_t *rse)
{
    LOGGER(INFO, "rse = %p\n", rse);
    LOGGER(INFO, "                rs = %p\n", rse->rs);
    LOGGER(INFO, "          division = %u\n", rse->division);
    LOGGER(INFO, "       vandermonde = %p\n", rse->vandermonde.ptr);
    LOGGER(INFO, "              _row = %p\n", rse->_row.ptr);
    LOGGER(INFO, "             _row2 = %p\n", rse->_row2.ptr);
    LOGGER(INFO, "     allocate_size = %zu\n", rse->allocate_size);
    LOGGER(INFO, "         _row_size = %zu\n", rse->_row_size);
    LOGGER(INFO, "       matrix_size = %zu\n", rse->matrix_size);
    LOGGER(INFO, "\n");
}

void _rs_view_rsd(rs_decode_t *rsd)
{
    LOGGER(INFO, "rsd = %p\n", rsd);
    LOGGER(INFO, "                rs = %p\n", rsd->rs);
    LOGGER(INFO, "          division = %u\n", rsd->division);
    LOGGER(INFO, "     allocate_size = %zu\n", rsd->allocate_size);
    LOGGER(INFO, "       matrix_size = %zu\n", rsd->matrix_size);
    LOGGER(INFO, "         _row_size = %zu\n", rsd->_row_size);
    LOGGER(INFO, "      _column_size = %zu\n", rsd->_column_size);
    LOGGER(INFO, "            merged = %p\n", rsd->merged.ptr);
    LOGGER(INFO, "           inverse = %p\n", rsd->inverse.ptr);
    LOGGER(INFO, "              _row = %p\n", rsd->_row.ptr);
    LOGGER(INFO, "             _row2 = %p\n", rsd->_row2.ptr);
    LOGGER(INFO, "           _column = %p\n", rsd->_column.ptr);
    LOGGER(INFO, "\n");
}

/* for slots *****************************************************************/

void rs_encode_slots(slot_t *parity,
                     slot_t *norm,
                     rs_encode_t *rse,
                     uint symbol_num)
{
    if (rse->rs->register_size == 2)
        _rs_encode16_slots(parity, norm, rse, symbol_num);
    else
        _rs_encode32_slots(parity, norm, rse, symbol_num);
}

void rs_decode_slots(slot_t *recover,
                     slot_t *merged,
                     rs_decode_t *rsd,
                     uint symbol_num)
{
    if (rsd->rs->register_size == 2)
        _rs_decode16_slots(recover, merged, rsd, symbol_num);
    else
        _rs_decode32_slots(recover, merged, rsd, symbol_num);
}

/*****************************************************************************/
/* private functions *********************************************************/
/*****************************************************************************/

static inline ushort _rs_mul16(reed_solomon_t *rs, ushort a, ushort b)
{
    register ushort c;

    if (a == 0 || b == 0)
        return 0;

    c = rs->gf.u16[a] + rs->gf.u16[b];
    if (c < rs->gf_max)
        return rs->gfi.u16[c];
    return rs->gfi.u16[c - rs->gf_max];
}

static inline uint _rs_mul32(reed_solomon_t *rs, uint a, uint b)
{
    /* no need to think about 32bit */
    register uint c;

    if (a == 0U || b == 0U)
        return 0U;

    c = rs->gf.u32[a] + rs->gf.u32[b];
    if (c < rs->gf_max)
        return rs->gfi.u32[c];
    return rs->gfi.u32[c - rs->gf_max];
}

static ushort _rs_div16(reed_solomon_t *rs, ushort a, ushort b)
{
    int c;

    if (a == 0)
        return 0;

    /* u16 同士の差を求めて、その差が0以上かを調べている以下の箇所は
     * bug臭いが cast がうまい具合に働いているんだろうか？
     * c = rs->gf.u16[a] - rs->gf.u16[b];
     * 不安でしょうがないので cast するように修正。
     * c = (int )rs->gf.u16[a] - (int )rs->gf.u16[b];
     */
    c = (int )rs->gf.u16[a] - (int )rs->gf.u16[b];
    if (c >= 0)
        return rs->gfi.u16[c];
    return rs->gfi.u16[c + rs->gf_max];
}

static uint _rs_div32(reed_solomon_t *rs, uint a, uint b)
{
    /* no need to think about 32bit */
    uint a32, b32;

    if (a == 0U)
        return 0U;

    a32 = rs->gf.u32[a];
    b32 = rs->gf.u32[b];
    if (a32 >= b32)
        return rs->gfi.u32[a32 - b32];
    /* a32 < b32 */
    return rs->gfi.u32[rs->gf_max + a32 - b32];

    #if 0
    /* 高速化する機会があれば、今(=平成26年1月9日23時11分7秒)思いついた
     * 手法を適用してみたい。
     * 高速化手法の結果は、reed_solomon/guard.c を実行すると明らかに！
     * あと、RS_gf32, RS_gfi32 使うようにしないと。
     */
    guard = 0U;
    if (a32 < b32)
        guard = rs->gf_max;
    return rs->gfi.u32[guard + a32 - b32];
    #endif
}

#if 0
static ushort _rs_pow16(reed_solomon_t *rs, ushort a, ushort x)
{
    int i;
    ushort c;

    if (a == 0)
        return 0;

    if (x == 0 || a == 1)
        return 1;

    c = a;
    for (i=0;i<x-1;i++)
        c = _rs_mul16(rs, c, a);
    return c;
}

static uint _rs_pow32(reed_solomon_t *rs, uint a, uint x)
{
    /* no need to think about 32bit */
    uint i;
    uint c;

    if (a == 0)
        return 0;

    if (x == 0 || a == 1)
        return 1;

    c = a;
    for (i=0;i<x-1;i++)
        c = _rs_mul32(rs, c, a);
    return c;
}
#endif

static void _rs_make_gf_and_gfi(reed_solomon_t *rs)
{
    /* no need to think about 32bit */
    uint i, bit_pattern = 1;
    _ptr_t gf = rs->gf, gfi = rs->gfi;

    _DEBUG("in _rs_make_gf_and_gfi()\n");

    for (i=0;i<rs->gf_max;i++) {
        if (bit_pattern & rs->w)
            bit_pattern ^= rs->poly;
    if (rs->register_size == 2) {
        gf.u16[bit_pattern] = i;
        gfi.u16[i] = bit_pattern;
    }
    else {
        gf.u32[bit_pattern] = i;
        gfi.u32[i] = bit_pattern;
    }
        bit_pattern <<= 1;
    }
/*
for debug.
        fprintf(stdout, "gf\n");
    for (i=0;i<rs->gf_max;i++) {
        fprintf(stdout, "0x%02x, ", gf.u16[i]);
    }
        fprintf(stdout, "\n");
        fprintf(stdout, "gfi\n");
    for (i=0;i<rs->gf_max;i++) {
        fprintf(stdout, "0x%02x, ", gfi.u16[i]);
    }
        fprintf(stdout, "\n");
*/

    if (rs->bits <= 4) {
        _DEBUG("gf =\n");
        for (i=0;i<rs->w;i++)
            _DEBUG("i = %u, 0x%04x\n", i, gf.u16[i]);
        _DEBUG("\n");
        _DEBUG("gfi =\n");
        for (i=0;i<rs->w;i++)
            _DEBUG("i = %u, 0x%04x\n", i, gfi.u16[i]);
        _DEBUG("\n\n");
    }
}

static inline void _rs_mul_matrix_vector16(
                         reed_solomon_t *rs,
                        _ptr_t answer, _ptr_t matrix, _ptr_t vec,
                         uint division)
{
    register uint i, j;
    register uint ans, tmp;

    for (j=0;j<division;j++){
        ans = 0;
        for (i=0;i<division;i++){
            tmp = _rs_mul16(rs,
                             matrix.u16[j * division + i],
                             vec.u16[i]);
            ans = _rs_ADD(ans, tmp);
            answer.u16[j] = ans;
        }
    }
}

static inline void
_rs_mul_matrix_vector32(
                reed_solomon_t *rs,
                _ptr_t answer, _ptr_t matrix, _ptr_t vec,
                uint division)
{
    register uint i, j;
    register uint ans, tmp;

    for (j=0;j<division;j++){
        ans = 0;
        for (i=0;i<division;i++){
                          /* matrix.u32[index] */
            tmp = _rs_mul32(rs,
                             matrix.u32[j * division + i],
                             vec.u32[i]);
            ans = _rs_ADD(ans, tmp);
            answer.u32[j] = ans;
        }
    }
}

static void _rs_make_e_matrix(_ptr_t e_matrix,
                               size_t register_size,
                               uint division)
{
    uint i;
    size_t matrix_size; /* division の値によって変わりうる値。
                         * 書き込み用構造体の導入によって、
                         * 書き込み用構造体に新たにmember入りさせる。
                         */

    memset(e_matrix.ptr, '\0',
           division * division * register_size);

    if (register_size == 2) {
        for (i=0;i<division;i++) {
            e_matrix.u16[i * division + i] = 1;
        }
    }
    else {
        for (i=0;i<division;i++) {
            e_matrix.u32[i * division + i] = 1;
        }
    }
}

static int _rs_solve_inverse(_ptr_t inverse,
                               _ptr_t matrix,
                                rs_decode_t *rsd)
{
    uint i, j, k, swap;
    uint division = rsd->division;
    int index, index_j, index_k, index_x, index_y, index_z;
    _ptr_t im = inverse, mt = matrix;
    uint tmp, tmp1, tmp2, tmp3;
    uint work;
    reed_solomon_t *rs = rsd->rs;

    _rs_make_e_matrix(inverse, rs->register_size, division);

    for (k=0;k<division;k++) {
        work = 0;
        if (rs->register_size == 2)
            work = mt.u16[k * division + k];
        else
            work = mt.u32[k * division + k];

        if (work == 0) {
            swap = 0;
            for (j=k+1;j<division;j++) {
                if (rs->register_size == 2)
                    work = mt.u16[j * division + k];
                else
                    work = mt.u32[j * division + k];

                if (work) {
                    swap = 1;
                    _DEBUG(
                           "matrix[%d], matrix[%d] = matrix[%d], matrix[%d]\n",
                            k, j, j, k);
                    /*
                    matrix[k], matrix[j] = matrix[j], matrix[k]
                    im[k], im[j] = im[j], im[k]
                    */
                    index_j = j * division * rs->register_size;
                    index_k = k * division * rs->register_size;
                    memcpy(rsd->_row.ptr, mt.ptr + index_j,
                           rsd->_row_size);
                    memcpy(mt.ptr + index_j, mt.ptr + index_k,
                           rsd->_row_size);
                    memcpy(mt.ptr + index_k, rsd->_row.ptr,
                           rsd->_row_size);
                    memcpy(rsd->_row.ptr, im.ptr + index_j, rsd->_row_size);
                    memcpy(im.ptr+ index_j, im.ptr + index_k,
                           rsd->_row_size);
                    memcpy(im.ptr+ index_k, rsd->_row.ptr,
                           rsd->_row_size);
                    break;
                }
            }

            if (swap == 0) {
                /*
                message =
                    ('cannot make inverse. division = {}, ',
                     'gf_max = {}').format(self.division, self.gf_max)
                raise Par2RankError(message)
                */
                return RS_RANK_ERROR;
            }
        }

        tmp = 0;
        if (rs->register_size == 2)
            tmp = mt.u16[k * division + k];
        else
            tmp = mt.u32[k * division + k];

        if (tmp != 1) {
            for (i=0;i<division;i++){
                index = k * division + i;
                if (rs->register_size == 2) {
                    mt.u16[index] = _rs_div16(rs, mt.u16[index], tmp);
                    im.u16[index] = _rs_div16(rs, im.u16[index], tmp);
                }
                else {
                    mt.u32[index] = _rs_div32(rs, mt.u32[index], tmp);
                    im.u32[index] = _rs_div32(rs, im.u32[index], tmp);
                }
            }
        }

        for (j=k+1;j<division;j++){
            if (rs->register_size == 2)
                tmp = mt.u16[j * division + k];
            else
                tmp = mt.u32[j * division + k];
            if (tmp == 0)
                continue;

            for (i=0;i<division;i++) {
                index_k = k * division + i;
                index_j = j * division + i;
/*
*/
                if (rs->register_size == 2) {
                    tmp1 = mt.u16[index_k];
                    tmp2 = _rs_mul16(rs, tmp, tmp1);
                    tmp3 = mt.u16[index_j];
                    mt.u16[index_j] = _rs_ADD(tmp3, tmp2);

                    tmp1 = im.u16[index_k];
                    tmp2 = _rs_mul16(rs, tmp, tmp1);
                    tmp3 = im.u16[index_j];
                    im.u16[index_j] = _rs_ADD(tmp3, tmp2);
                }
                else {
                    tmp1 = mt.u32[index_k];
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    tmp3 = mt.u32[index_j];
                    mt.u32[index_j] = _rs_ADD(tmp3, tmp2);

                    tmp1 = im.u32[index_k];
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    tmp3 = im.u32[index_j];
                    im.u32[index_j] = _rs_ADD(tmp3, tmp2);
                }
            }
        }
    }

    _DEBUG("in _rs_solve_inverse() 前進完了 moving front done.\n\n");

    for (k=0;k<division-1;k++) {
        for (j=0;j<division-1-k;j++) {
            index_z = division - 1 - k;
            index_x = division - 1 - k;
            index_y = division - 1 - k - j - 1;

            if (rs->register_size == 2)
                tmp = mt.u16[index_y * division + index_x];
            else
                tmp = mt.u32[index_y * division + index_x];

            for (i=0;i<division;i++) {
/*
                tmp1 = mt[index_z * division + i];
                tmp2 = _mul(rse, tmp, tmp1);
                tmp3 = mt[index_y * division + i];
                mt[index_y * division + i] = _add(tmp3, tmp2);
*/
                if (rs->register_size == 2) {
                    tmp1 = im.u16[index_z * division + i];
                    tmp2 = _rs_mul16(rs, tmp, tmp1);
                    tmp3 = im.u16[index_y * division + i];
                    im.u16[index_y * division + i] = _rs_ADD(tmp3, tmp2);
                }
                else {
                    tmp1 = im.u32[index_z * division + i];
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    tmp3 = im.u32[index_y * division + i];
                    im.u32[index_y * division + i] = _rs_ADD(tmp3, tmp2);
                }
            }
        }
    }

    #ifdef DEBUG
    _rs_view_matrix16(mt.u16, division);
    _rs_view_matrix16(im.u16, division);
    #endif

    return RS_SUCCESS;
}

size_t aligned_size(size_t size)
{
    size_t padding_size, mod;
    mod = size % ALIGHNMENT_SIZE;
    padding_size = ALIGHNMENT_SIZE - mod;
    if (padding_size == ALIGHNMENT_SIZE)
        padding_size = 0;
    return size + padding_size;
}

static big_bang_t _dokkaan = {
/* allocate_size       mem          mem_status */
               0,     NULL, BB_MEM_NO_ALLOCATE, {
/* bits      poly  symbol_size   register_size */
    { 4,       19,      1,          2, 0, 0, {NULL}, {NULL}, 0, 0},
    { 8,      285,      1,          2, 0, 0, {NULL}, {NULL}, 0, 0},
    {16,    65581,      2,          2, 0, 0, {NULL}, {NULL}, 0, 0},
    /*
    {24, 16777243,      3,          4, 0, 0, {NULL}, {NULL}, 0, 0},
    */
    }
};

static big_bang_t *_rs_bright(void)
{
    return &_dokkaan;
}

static void _rs_view_rs(reed_solomon_t *rs)
{
    LOGGER(INFO, "rs = %p\n", rs);
    LOGGER(INFO, "              bits = %u\n", rs->bits);
    LOGGER(INFO, "              poly = %u\n", rs->poly);
    LOGGER(INFO, "       symbol_size = %u\n", rs->symbol_size);
    LOGGER(INFO, "     register_size = %zu\n", rs->register_size);
    LOGGER(INFO, "                 w = %u\n", rs->w);
    LOGGER(INFO, "            gf_max = %u\n", rs->gf_max);
    LOGGER(INFO, "           gf_size = %zu\n", rs->gf_size);
    LOGGER(INFO, "                gf = %p\n", rs->gf.ptr);
    LOGGER(INFO, "               gfi = %p\n", rs->gfi.ptr);
    LOGGER(INFO, "\n");
}

static void _rs_view_big_bang(void)
{
    big_bang_t *bb = _rs_bright();
    int i;
    LOGGER(INFO, "big_bang = %p\n", bb);
    LOGGER(INFO, "     allocate_size = %zu\n", bb->allocate_size);
    LOGGER(INFO, "               mem = %p\n", bb->mem);
    LOGGER(INFO, "\n");
    for (i=0;i<RS_GF_NUM;i++) {
        _rs_view_rs(bb->rs + i);
    }
}

static reed_solomon_t *_rs_get_rs(uint bits)
{
    int i;
    big_bang_t *universe = _rs_bright();
    reed_solomon_t *rs = NULL;

    for (i=0;i<RS_GF_NUM;i++) {
        if (bits == universe->rs[i].bits) {
            rs = universe->rs + i;
            break;
        }
    }
    if (i == RS_GF_NUM) {
        rs = NULL;
    }

    return rs;
}

static int _rs_init_rs(reed_solomon_t *rs)
{

    rs->w = 1 << rs->bits;
    rs->gf_max = rs->w - 1;
    rs->gf_size = rs->w * rs->register_size;
    rs->allocate_size = rs->gf_size * 2;

    #ifdef DEBUG
    _rs_view_rs(rs);
    #endif

    return 0;
}

static int _rs_init_the_universe(big_bang_t *universe)
{
    reed_solomon_t *rs;
    uint allocate_size = 0;
    int i;
    char *mem;

    #ifdef DEBUG
    set_logger(stderr);
    #endif

    universe = _rs_bright();
    if (universe->mem_status == BB_MEM_ALLOCATED) {
        LOGGER(WARN, "already allocated memory for universe->mem(=%p).\n", universe->mem);
        return RS_SUCCESS;
    }

    allocate_size = 0;
    for (i=0;i<RS_GF_NUM;i++) {
        rs = universe->rs + i;
        _rs_init_rs(rs);

        allocate_size += rs->allocate_size;
    }
    universe->allocate_size = allocate_size;

    mem = (char *)malloc(allocate_size);
    _DEBUG("in _rs_init_the_universe()\n");
    _DEBUG("malloc(allocate_size=%u) = %p\n\n", allocate_size, mem);
    if (mem == NULL) {
        LOGGER(ERROR, "failed _rs_init_the_universe()\n");
        LOGGER(ERROR, "cause of that malloc(allocate_size=%u).\n\n",
                    allocate_size);
        return RS_MALLOC_ERROR;
    }

    universe->mem = mem;
    for (i=0;i<RS_GF_NUM;i++) {
        rs = universe->rs + i;
        rs->gf.ptr = mem;
        mem += rs->gf_size;

        /* 後で上と同じように直す。*/
        universe->rs[i].gfi.ptr = mem;
        mem += rs->gf_size;
    }

    return RS_SUCCESS;
}

static int _rs_init_gf_gfi(big_bang_t *universe)
{
    int i;

    for (i=0;i<RS_GF_NUM;i++) {
        _rs_make_gf_and_gfi(universe->rs + i);
    }

    return RS_SUCCESS;
}

static size_t _rs_calc_rse_memory_size(rs_encode_t *rse,
                                         reed_solomon_t *rs, uint division)
{
    /*  need three matrixes.
        vandermonde, merged, inverse
        need two vectors.
        vector, parity
     */
    rse->division = division;
    rse->matrix_size = rse->division * rse->division * rs->register_size;
    rse->matrix_size = aligned_size(rse->matrix_size);
    /* 8KO * division * 2*/
    rse->_row_size = division * rs->register_size;
    rse->_row_size = aligned_size(rse->_row_size);

    rse->allocate_size = sizeof(rs_encode_t) +
                         rse->matrix_size +
                         rse->_row_size * 2;
    return rse->allocate_size;
}

static size_t _rs_init_rse(rs_encode_t *rse,
                             char *mem, reed_solomon_t *rs)
{
    char *mem_ = mem;
    rse->rs = rs;
    rse->vandermonde.ptr = (void *)mem; mem += rse->matrix_size;
    /* _row and _row2 need in rs_encodeXX() */
    rse->_row.ptr = (void *)mem;        mem += rse->_row_size;
    rse->_row2.ptr = (void *)mem;       mem += rse->_row_size;
    return (size_t )(mem - mem_);
}

static size_t _rs_calc_rsd_memory_size(rs_decode_t *rsd,
                                         reed_solomon_t *rs,
                                         uint division)
{
    /*  need three matrixes.
        vandermonde, merged, inverse
        need two vectors.
        vector, parity
     */
    rsd->division = division;
    rsd->matrix_size = rsd->division * rsd->division * rs->register_size;
    /* 8 KO * division * 2 */
    rsd->_row_size = rsd->division * rs->register_size;
    rsd->_column_size = rsd->division * rs->symbol_size;

    rsd->matrix_size = aligned_size(rsd->matrix_size);
    rsd->_row_size = aligned_size(rsd->_row_size);
    rsd->_column_size = aligned_size(rsd->_column_size);

    rsd->allocate_size = sizeof(rs_decode_t) +
                         rsd->matrix_size * 2 +
                         rsd->_row_size * 2 +
                         rsd->_column_size;
    return rsd->allocate_size;
}

static size_t _rs_init_rsd(rs_decode_t *rsd,
                             char *mem,
                             reed_solomon_t *rs)
{
    char *mem_ = mem;
    rsd->rs = rs;
    rsd->merged.ptr = (void *)mem;      mem += rsd->matrix_size;
    rsd->inverse.ptr = (void *)mem;     mem += rsd->matrix_size;
    /* _row and _row2 need in rs_decodeXX() */
    rsd->_row.ptr = (void *)mem;        mem += rsd->_row_size;
    rsd->_row2.ptr = (void *)mem;       mem += rsd->_row_size;
    rsd->_column.ptr = (void *)mem;     mem += rsd->_column_size;
    return (size_t )(mem - mem_);
}

static void _rs_make_vandermonde(rs_encode_t *rse)
{
    _ptr_t vm = rse->vandermonde;
    uint i, j, division = rse->division;
    reed_solomon_t *rs = rse->rs;

    for(i=0;i<division;i++)
    if (rs->register_size == 2)
        vm.u16[i] = 1;
    else
        vm.u32[i] = 1;

    if (rs->register_size == 2) {
        for(j=1;j<division;j++)
            for(i=0;i<division;i++)
                vm.u16[j * division + i] =
                    _rs_mul16(rs, vm.u16[(j-1) * division + i], i + 1);
    }
    else {
        for(j=1;j<division;j++)
            for(i=0;i<division;i++)
                vm.u32[j * division + i] =
                    _rs_mul32(rs, vm.u32[(j-1) * division + i], i + 1);
    }
}

static void _rs_view_matrix16(ushort *matrix, uint division)
{
    uint i, j;

    LOGGER(INFO, "_rs_view_matrix16(matrix=%p, division=%u)\n",
                                     matrix, division);
    LOGGER(INFO, "     ");
    for (i=0;i<division;i++)
        LOGGER(INFO, "%4x ", i);
    LOGGER(INFO, "\n");
    for (j=0;j<division;j++) {
        LOGGER(INFO, "%4x ", j);
        for (i=0;i<division;i++) {
            LOGGER(INFO, "%4x ", matrix[j * division + i]);
        }
        LOGGER(INFO, "\n");
    }
}

static void _rs_encode16_slots(slot_t *parity,
                               slot_t *norm,
                               rs_encode_t *rse,
                               uint symbol_num)
{
    reed_solomon_t *rs = rse->rs;
    uint i, j, k;
    size_t symbol_size = rs->symbol_size;
    register uint division = rse->division;
    uchar unum;
    uint num;
    _ptr_t vandermonde = rse->vandermonde;
    _ptr_t _vector = rse->_row;
    _ptr_t _parity = rse->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += SLOT_buf(norm + j)[i * symbol_size + k];
            }
            _vector.u16[j] = num;
        }

        _rs_mul_matrix_vector16(rs, _parity, vandermonde, _vector,
                                division);

        for (j=0;j<division;j++) {
            num = _parity.u16[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                SLOT_buf(parity + j)[i * symbol_size + k] = unum;
            }
        }
    }
}

static void _rs_encode32_slots(slot_t *parity,
                               slot_t *norm,
                               rs_encode_t *rse,
                               uint symbol_num)
{
    reed_solomon_t *rs = rse->rs;
    uint i, j, k;
    size_t symbol_size = rs->symbol_size;
    register uint division = rse->division;
    uchar unum;
    uint num;
    _ptr_t vandermonde = rse->vandermonde;
    _ptr_t _vector = rse->_row;
    _ptr_t _parity = rse->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += SLOT_buf(norm + j)[i * symbol_size + k];
            }
            _vector.u32[j] = num;
        }

        _rs_mul_matrix_vector32(rs, _parity, vandermonde, _vector,
                                division);

        for (j=0;j<division;j++) {
            num = _parity.u32[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                SLOT_buf(parity + j)[i * symbol_size + k] = unum;
            }
        }
    }
}

static void _rs_decode16_slots(slot_t *recover,
                               slot_t *merged,
                               rs_decode_t *rsd,
                               uint symbol_num)
{
    reed_solomon_t *rs = rsd->rs;
    uint i, j, k;
    size_t symbol_size = rs->symbol_size;
    register uint division = rsd->division;
    uchar unum;
    uint num;
    _ptr_t _vector = rsd->_row;
    _ptr_t _recover = rsd->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += SLOT_buf(merged + j)[i * symbol_size + k];
            }
            _vector.u16[j] = num;
        }

        _rs_mul_matrix_vector16(rs, _recover, rsd->inverse, _vector,
                                division);

        for (j=0;j<division;j++) {
            num = _recover.u16[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                SLOT_buf(recover + j)[i * symbol_size + k] = unum;
            }
        }
    }
}

static void _rs_decode32_slots(slot_t *recover,
                               slot_t *merged,
                               rs_decode_t *rsd,
                               uint symbol_num)
{
    reed_solomon_t *rs = rsd->rs;
    uint i, j, k;
    size_t symbol_size = rs->symbol_size;
    register uint division = rsd->division;
    uchar unum;
    uint num;
    _ptr_t _vector = rsd->_row;
    _ptr_t _recover = rsd->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += SLOT_buf(merged + j)[i * symbol_size + k];
            }
            _vector.u32[j] = num;
        }

        _rs_mul_matrix_vector32(rs, _recover, rsd->inverse, _vector,
                                division);

        for (j=0;j<division;j++) {
            num = _recover.u32[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                SLOT_buf(recover + j)[i * symbol_size + k] = unum;
            }
        }
    }
}

#ifdef __TEST__

/*****************************************************************************/
/* for test functions ********************************************************/
/*****************************************************************************/

ushort _rs_mul16_wrap(reed_solomon_t *rs, ushort a, ushort b)
{
    return _rs_mul16(rs, a, b);
}

void _rs_make_e_matrix_wrap(_ptr_t e_matrix,
                               size_t register_size,
                               uint division)
{
    _rs_make_e_matrix(e_matrix, register_size, division);
}

ushort _rs_div16_wrap(reed_solomon_t *rs, ushort a, ushort b)
{
    return _rs_div16(rs, a, b);
}

big_bang_t *_rs_get_universe_wrap(void)
{
    big_bang_t *universe = _rs_bright();
    return universe;
}
#endif
