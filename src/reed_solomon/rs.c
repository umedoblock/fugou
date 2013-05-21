/* Copyright (C) 平成20-25(2008-2013)年 梅どぶろく(umedoblock)
 * rs library source
 */

#include "rs.h"
#define rs_ADD(a, b) (a ^ b)

static void _rs_debug(const char *fmt, ...)
{
    va_list ap;

    if (_log != NULL && DEBUG_ >= _log_level) {
        fprintf(_log, "[rs] [DEBUG] ");
        va_start(ap, fmt);
        vfprintf(_log, fmt, ap);
        va_end(ap);
    }
}

void rs_set_logger(FILE *fp)
{
    _log = fp;
}

static enum _log_levels {
    DEBUG_, INFO, WARN, ERROR, FATAL, BUG
};

static const char *_log_level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "BUG"
};

static int _log_level = DEBUG_;
static FILE *_log = NULL;

static void rs_logger(const int level, const char *fmt, ...)
{
    va_list ap;

    /*
    fprintf(stderr, "[rs] [%s] _log=%p\n", _log_level_names[level], _log);
    fprintf(stderr, "(_log=%p != NULL && level=%d >= _log_level=%d)\n",
                      _log, level, _log_level);
                      */
    if (_log != NULL && level >= _log_level) {
        fprintf(_log, "[rs] [%s] ", _log_level_names[level]);
        /*
        n = vsnprintf(p, size, fmt, ap);
        */
        va_start(ap, fmt);
        vfprintf(_log, fmt, ap);
        va_end(ap);
    }
}

static inline ushort _rs_mul16(reed_solomon_t *rs, ushort a, ushort b)
{
    register uint c;

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

    if (a == 0 || b == 0)
        return 0;

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

    c = rs->gf.u16[a] - rs->gf.u16[b];
    if (c >= 0)
        return rs->gfi.u16[c];
    return rs->gfi.u16[c + rs->gf_max];
}

static uint _rs_div32(reed_solomon_t *rs, uint a, uint b)
{
    /* no need to think about 32bit */
    int c;

    if (a == 0)
        return 0;

    c = rs->gf.u32[a] - rs->gf.u32[b];
    if (rs->gf.u32[a] >= rs->gf.u32[b])
        return rs->gfi.u32[c];
    return rs->gfi.u32[c + rs->gf_max];
}

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

static void _rs_make_gf_and_gfi(reed_solomon_t *rs)
{
    /* no need to think about 32bit */
    uint i, bit_pattern = 1;
    _ptr_t gf = rs->gf, gfi = rs->gfi;

    _rs_debug("in _rs_make_gf_and_gfi()\n");

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

    if (rs->bits <= 4) {
        _rs_debug("gf =\n");
        for (i=0;i<rs->w;i++)
            _rs_debug("i = %u, 0x%04x\n", i, gf.u16[i]);
        _rs_debug("\n");
        _rs_debug("gfi =\n");
        for (i=0;i<rs->w;i++)
            _rs_debug("i = %u, 0x%04x\n", i, gfi.u16[i]);
        _rs_debug("\n\n");
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
            tmp = _rs_mul16(rs, \
                             matrix.u16[j * division + i], \
                             vec.u16[i]);
            ans = _rs_ADD(ans, tmp);
            answer.u16[j] = ans;
        }
    }
}

static inline void
_rs_mul_matrix_vector32(
                reed_solomon_t *rs, \
                _ptr_t answer, _ptr_t matrix, _ptr_t vec, \
                uint division)
{
    register uint i, j;
    register uint ans, tmp;

    for (j=0;j<division;j++){
        ans = 0;
        for (i=0;i<division;i++){
                          /* matrix.u32[index] */
            tmp = _rs_mul32(rs, \
                             matrix.u32[j * division + i], \
                             vec.u32[i]);
            ans = _rs_ADD(ans, tmp);
            answer.u32[j] = ans;
        }
    }
}

static void _rs_make_e_matrix(_ptr_t e_matrix,
                                 reed_solomon_t *rs,
                                 uint division)
{
    uint i;

    memset(e_matrix.ptr, '\0', \
           division * division * rs->register_size);

    if (rs->register_size == 2) {
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

    _rs_make_e_matrix(inverse, rs, division);

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
                    _rs_debug(
                           "matrix[%d], matrix[%d] = matrix[%d], matrix[%d]\n",
                            k, j, j, k);
                    /*
                    matrix[k], matrix[j] = matrix[j], matrix[k]
                    im[k], im[j] = im[j], im[k]
                    */
                    index_j = j * division * rs->register_size;
                    index_k = k * division * rs->register_size;
                    memcpy(rsd->_row.ptr, mt.ptr + index_j, \
                           rsd->_row_size);
                    memcpy(mt.ptr + index_j, mt.ptr + index_k, \
                           rsd->_row_size);
                    memcpy(mt.ptr + index_k, rsd->_row.ptr, \
                           rsd->_row_size);
                    memcpy(rsd->_row.ptr, im.ptr + index_j, rsd->_row_size);
                    memcpy(im.ptr+ index_j, im.ptr + index_k, \
                           rsd->_row_size);
                    memcpy(im.ptr+ index_k, rsd->_row.ptr, \
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

    _rs_debug("in _rs_solve_inverse() 前進完了 moving front done.\n\n");

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

    return RS_SCUCCESS;
}

/* public functions */

int rs_big_bang(void)
{
    big_bang_t *universe = _rs_bright();

    if (_rs_init_the_universe(universe) < 0)
        return RS_MALLOC_ERROR;
    _rs_init_gf_gfi(universe);

    return RS_SCUCCESS;
}

int rs_ultimate_fate_of_the_universe(void)
{
    big_bang_t *universe = _rs_bright();
    free(universe->mem);
    return RS_SCUCCESS;
}

/* private functions */

static big_bang_t *_rs_bright(void)
{
    return &_dokkaan;
}

/* essence functions */

static size_t _aligned_size(size_t size)
{
    size_t padding_size, mod;
    mod = size % ALIGHNMENT_SIZE;
    padding_size = ALIGHNMENT_SIZE - mod;
    if (padding_size == ALIGHNMENT_SIZE)
        padding_size = 0;
    return size + padding_size;
}

big_bang_t *_rs_bright(void);

static big_bang_t _dokkaan = {
/* allocate_size       mem */
               0,     NULL, {
/* bits      poly  symbol_size   register_size */
    { 4,       19,      1,          2, 0, 0, {NULL}, {NULL}, 0, 0},
    { 8,      285,      1,          2, 0, 0, {NULL}, {NULL}, 0, 0},
    {16,    65581,      2,          2, 0, 0, {NULL}, {NULL}, 0, 0},
    {24, 16777243,      3,          4, 0, 0, {NULL}, {NULL}, 0, 0},
    }
};

static void _rs_view_reed_solomon(reed_solomon_t *rs)
{
    rs_logger(INFO, "rs = %p\n", rs);
    rs_logger(INFO, "             bits = %u\n", rs->bits);
    rs_logger(INFO, "             poly = %u\n", rs->poly);
    rs_logger(INFO, "           symbol_size = %u\n", rs->symbol_size);
    rs_logger(INFO, "        register_size = %zu\n", rs->register_size);
    rs_logger(INFO, "                w = %u\n", rs->w);
    rs_logger(INFO, "           gf_max = %u\n", rs->gf_max);
    rs_logger(INFO, "          gf_size = %zu\n", rs->gf_size);
    rs_logger(INFO, "               gf = %p\n", rs->gf.ptr);
    rs_logger(INFO, "              gfi = %p\n", rs->gfi.ptr);
    rs_logger(INFO, "\n");
}

static void _rs_view_encode(rs_encode_t *rse)
{
    rs_logger(INFO, "rse = %p\n", rse);
    rs_logger(INFO, "               rs = %p\n", rse->rs);
    rs_logger(INFO, "        division = %u\n", rse->division);
    rs_logger(INFO, "       vandermonde = %p\n", rse->vandermonde.ptr);
    rs_logger(INFO, "              _row = %p\n", rse->_row.ptr);
    rs_logger(INFO, "             _row2 = %p\n", rse->_row2.ptr);
    rs_logger(INFO, "     allocate_size = %zu\n", rse->allocate_size);
    rs_logger(INFO, "         _row_size = %zu\n", rse->_row_size);
    rs_logger(INFO, "       matrix_size = %zu\n", rse->matrix_size);
    rs_logger(INFO, "\n");
}

static void _rs_view_decode(rs_decode_t *rsd)
{
    rs_logger(INFO, "rsd = %p\n", rsd);
    rs_logger(INFO, "               rs = %p\n", rsd->rs);
    rs_logger(INFO, "        division = %u\n", rsd->division);
    rs_logger(INFO, "     allocate_size = %zu\n", rsd->allocate_size);
    rs_logger(INFO, "       matrix_size = %zu\n", rsd->matrix_size);
    rs_logger(INFO, "         _row_size = %zu\n", rsd->_row_size);
    rs_logger(INFO, "      _column_size = %zu\n", rsd->_column_size);
    rs_logger(INFO, "            merged = %p\n", rsd->merged.ptr);
    rs_logger(INFO, "           inverse = %p\n", rsd->inverse.ptr);
    rs_logger(INFO, "              _row = %p\n", rsd->_row.ptr);
    rs_logger(INFO, "             _row2 = %p\n", rsd->_row2.ptr);
    rs_logger(INFO, "           _column = %p\n", rsd->_column.ptr);
    rs_logger(INFO, "\n");
}

static void _rs_view_big_bang(void)
{
    big_bang_t *big_bang = _rs_bright();
    int i;
    rs_logger(INFO, "     allocate_size = %zu\n", big_bang->allocate_size);
    rs_logger(INFO, "               mem = %p\n", big_bang->mem);
    rs_logger(INFO, "\n");
    for (i=0;i<RS_GF_NUM;i++) {
        _rs_view_reed_solomon(big_bang->rs + i);
    }
}

static reed_solomon_t *_rs_get_reed_solomon(uint bits)
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
    _rs_view_reed_solomon(rs);
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
    _rs_set_logger(stderr);
    #endif

    universe = _rs_bright();

    allocate_size = 0;
    for (i=0;i<RS_GF_NUM;i++) {
        rs = universe->rs + i;
        _rs_init_rs(rs);

        allocate_size += rs->allocate_size;
    }
    universe->allocate_size = allocate_size;

    mem = (char *)malloc(allocate_size);
    _rs_debug("in _rs_init_the_universe()\n");
    _rs_debug("malloc(allocate_size=%u) = %p\n\n", allocate_size, mem);
    if (mem == NULL) {
        rs_logger(ERROR, "failed _rs_init_the_universe()\n");
        rs_logger(ERROR, "cause of that malloc(allocate_size=%u).\n\n",
                    allocate_size);
        return RS_MALLOC_ERROR;
    }

    universe->mem = mem;
    for (i=0;i<RS_GF_NUM;i++) {
        rs = universe->rs + i;
        rs->gf.ptr = mem;
        mem += rs->gf_size;

        universe->rs[i].gfi.ptr = mem;
        mem += rs->gf_size;
    }

    return RS_SCUCCESS;
}

static int _rs_init_gf_gfi(big_bang_t *universe)
{
    int i;

    for (i=0;i<RS_GF_NUM;i++) {
        _rs_make_gf_and_gfi(universe->rs + i);
    }

    return RS_SCUCCESS;
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
    rse->matrix_size = _aligned_size(rse->matrix_size);
    /* 8KO * division * 2*/
    rse->_row_size = division * rs->register_size;
    rse->_row_size = _aligned_size(rse->_row_size);

    rse->allocate_size = rse->matrix_size + \
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
    rsd->matrix_size = _aligned_size(rsd->matrix_size);
    /* 8 KO * division * 2 */
    rsd->_row_size = rsd->division * rs->register_size;
    rsd->_row_size = _aligned_size(rsd->_row_size);
    rsd->_column_size = rsd->division * rs->symbol_size;
    rsd->_column_size = _aligned_size(rsd->_column_size);

    rsd->allocate_size = rsd->matrix_size * 2 + \
                         rsd->_row_size * 2 + \
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
                vm.u16[j * division + i] = \
                    _rs_mul16(rs, vm.u16[(j-1) * division + i], i + 1);
    }
    else {
        for(j=1;j<division;j++)
            for(i=0;i<division;i++)
                vm.u32[j * division + i] = \
                    _rs_mul32(rs, vm.u32[(j-1) * division + i], i + 1);
    }
}

static int _rs_set_rs(reed_solomon_t **rs, uint bits, uint division)
{
    reed_solomon_t *rs_;

    rs_ = _rs_get_reed_solomon(bits);
    if (rs_ == NULL) {
        rs_logger(ERROR, "bits(=%u) must chose 4, 8, 16 or 24 for bits.\n",
                            bits);
        return RS_INVALID_BITS_ERROR;
    }
    if (division < 2 || division > rs_->gf_max) {
        rs_logger(ERROR, "division(=%u) must be 2 <= division <= %u.\n",
                            division, rs_->gf_max);
        return RS_INVALID_DIVISION_ERROR;
    }
    *rs = rs_;

    return RS_SCUCCESS;
}

static void _rs_view_matrix16(ushort *matrix, uint division)
{
    uint i, j;

    rs_logger(INFO, "_rs_view_matrix16(matrix=%p, division=%u)\n",
                                     matrix, division);
    rs_logger(INFO, "     ");
    for (i=0;i<division;i++)
        rs_logger(INFO, "%4x ", i);
    rs_logger(INFO, "\n");
    for (j=0;j<division;j++) {
        rs_logger(INFO, "%4x ", j);
        for (i=0;i<division;i++) {
            rs_logger(INFO, "%4x ", matrix[j * division + i]);
        }
        rs_logger(INFO, "\n");
    }
}
