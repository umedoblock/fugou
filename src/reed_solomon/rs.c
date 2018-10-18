/* Copyright (C) 平成20-25(2008-2013)年 梅濁酒(umedoblock)
 * rs library source
 */

#include "rs.h"

/*****************************************************************************/
/* static functions **********************************************************/
/*****************************************************************************/
static big_bang_t *_rs_bright(void);
static int _rs_init_gf_gfi(big_bang_t *universe);
static int _rs_init_the_universe(big_bang_t *universe);
static reed_solomon_t *_rs_get_rs(uint bits);
static uint _rs_mul32(reed_solomon_t *rs, uint a, uint b);
static ushort _rs_mul16(reed_solomon_t *rs, ushort a, ushort b);

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

/* TODO:
 * poly=>bits=8で，
   for (j=0;j<256;j++) {
       for (i=0;i<256;i++) {
         gf65536 = (gf[j] << 8) + gf[i];
       }
   }

   としてみて，gf65536, gfi65536 の
   処理時間と実効速度を調べる。

   単純には，２倍に実行速度になるはず。
   他の処理もあったりするから，現実には1.5倍で満足しないといけないかな。
   memory 参照に多くの時間を費やしているようだった。

   単なる table なら，endian 変換も必要ないような気がする。
 */

#define rs_mul_matrix_vector(XX)                                         \
void                                                                     \
rs_mul_matrix_vector##XX(reed_solomon_t *rs,                             \
                          vector_t *answer,                              \
                          matrix_t *mat,                                 \
                          vector_t *vec)                                 \
{                                                                        \
    register uint e, j;                                                  \
    register uint ans, tmp;                                              \
                                                                         \
    for (j=0;j<MATRIX_columns(mat);j++){                                 \
        ans = 0;                                                         \
        for (e=0;e<VECTOR_elements(vec);e++){                            \
            tmp = _rs_mul##XX(rs,                                        \
                            MATRIX_u(XX, mat)[j * MATRIX_rows(mat) + e], \
                            VECTOR_u(XX, vec)[e]);                       \
            ans = _rs_ADD(ans, tmp);                                     \
        }                                                                \
        VECTOR_u(XX, answer)[j] = ans;                                   \
    }                                                                    \
}

rs_mul_matrix_vector(16);
rs_mul_matrix_vector(32);

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

void set_rse(reed_solomon_encode_t *rse, uint bits, uint division, uchar *mem)
{
    size_t matrix_mem_size, vector_mem_size;

    rse->division = division;
    rs_take_rs(&rse->rs, bits, division);

    matrix_mem_size = \
        matrix_calc_mem_size(division, division, rse->rs->register_size);
    rse->matrix_mem_size = matrix_mem_size;
    vector_mem_size = vector_calc_mem_size(division, rse->rs->register_size);
    rse->vector_mem_size = vector_mem_size;

    rse->vandermonde = (matrix_t *)mem; mem += matrix_mem_size;
    rse->parity_vector = (vector_t *)mem; mem += vector_mem_size;
    rse->data_vector = (vector_t *)mem; mem += vector_mem_size;

    matrix_init(rse->vandermonde, division, division, rse->rs->register_size);
    vector_init(rse->parity_vector, division, rse->rs->register_size);
    vector_init(rse->data_vector, division, rse->rs->register_size);

    matrix_make_vandermonde_matrix(rse->vandermonde, rse->rs, division);
}

/*****************************************************************************/
/* private functions *********************************************************/
/*****************************************************************************/

#define _rs_mul(XX, TYPE)                                            \
inline TYPE _rs_mul ## XX(reed_solomon_t *rs, TYPE a, TYPE b) \
{                                                                    \
    uint ta, tb, tc, tgf_max;                                        \
                                                                     \
    if (a == 0 || b == 0)                                            \
        return 0;                                                    \
                                                                     \
    ta = VECTOR_u(XX, rs->gf)[a];                                    \
    tb = VECTOR_u(XX, rs->gf)[b];                                    \
    tc = ta + tb;                                                    \
    tgf_max = rs->gf_max;                                            \
    if (tc < tgf_max)                                                \
        return VECTOR_u(XX, rs->gfi)[tc];                            \
    return VECTOR_u(XX, rs->gfi)[tc - tgf_max];                      \
}

_rs_mul(16, ushort)
_rs_mul(32, uint)

static ushort _rs_div16(reed_solomon_t *rs, ushort a, ushort b)
{
    if (a == 0)
        return 0;

    uint ta, tb, tgf_max;
    ta = VECTOR_u(16, rs->gf)[a];
    tb = VECTOR_u(16, rs->gf)[b];
    tgf_max = rs->gf_max;                                            \
    if (ta >= tb)
        return VECTOR_u(16, rs->gfi)[ta - tb];
    return VECTOR_u(16, rs->gfi)[(tgf_max + ta) - tb];
}

static uint _rs_div32(reed_solomon_t *rs, uint a, uint b)
{
    /* no need to think about 32bit */
    uint a32, b32;

    if (a == 0U)
        return 0U;

    a32 = VECTOR_u(32, rs->gf)[a];
    b32 = VECTOR_u(32, rs->gf)[b];
    if (a32 >= b32)
        return VECTOR_u(32, rs->gfi)[a32 - b32];
    /* a32 < b32 */
    return VECTOR_u(32, rs->gfi)[RS_gf_max(rs) + a32 - b32];
}

static void _rs_make_gf_and_gfi(reed_solomon_t *rs)
{
    /* no need to think about 32bit */
    uint i, bit_pattern = 1;

    _DEBUG("in _rs_make_gf_and_gfi()\n");

    for (i=0;i<RS_gf_max(rs);i++) {
        if (bit_pattern & RS_w(rs))
            bit_pattern ^= RS_poly(rs);
    if (RS_register_size(rs) == 2) {
        VECTOR_u(16, RS_gf(rs))[bit_pattern] = i;
        VECTOR_u(16, RS_gfi(rs))[i] = bit_pattern;
    }
    else {
        VECTOR_u(32, RS_gf(rs))[bit_pattern] = i;
        VECTOR_u(32, RS_gfi(rs))[i] = bit_pattern;
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
        for (i=0;i<RS_w(rs);i++)
            _DEBUG("i = %u, 0x%04x\n", i, MATRIX_u(16, RS_gf(rs))[i]);
        _DEBUG("\n");
        _DEBUG("gfi =\n");
        for (i=0;i<RS_w(rs);i++)
            _DEBUG("i = %u, 0x%04x\n", i, MATRIX_u(16, RS_gfi(rs))[i]);
        _DEBUG("\n\n");
    }
}

#define _rs_mul_matrixes(XX)                                                \
static inline void _rs_mul_matrixes##XX(reed_solomon_t *rs,                 \
                                        matrix_t *answer,                   \
                                        matrix_t *mat1,                     \
                                        matrix_t *mat2)                     \
{                                                                           \
    register uint i, j, k;                                                  \
    register uint ans, tmp;                                                 \
                                                                            \
/*                                                                          \
    see reed_solomon/matrix.py                                              \
                                                                            \
    for k in range(4):                                                      \
        for j in range(4):                                                  \
            sum = 0                                                         \
            for i in range(4):                                              \
                sum += mat1[k][i] * mat2[i][j]                              \
            mat3[k][j] = sum                                                \
*/                                                                          \
    for (k=0;k<MATRIX_rows(mat1);k++){                                      \
        for (j=0;j<MATRIX_columns(mat2);j++){                               \
            ans = 0;                                                        \
            for (i=0;i<MATRIX_rows(mat2);i++){                              \
                tmp = _rs_mul##XX(                                          \
                            rs,                                             \
                            MATRIX_u(XX, mat1)[k * MATRIX_rows(mat1) + i],  \
                            MATRIX_u(XX, mat2)[i * MATRIX_rows(mat2) + j]); \
                ans = _rs_ADD(ans, tmp);                                    \
            }                                                               \
            MATRIX_u(XX, answer)[k * MATRIX_columns(mat2) + j] = ans;       \
        }                                                                   \
    }                                                                       \
}

_rs_mul_matrixes(32)
_rs_mul_matrixes(16)

size_t vector_calc_vector_size(uint elements, size_t element_size)
{
    size_t vector_size;

    if (element_size != 2 && element_size != 4) {
        return 0;
    }

    vector_size = elements * element_size;

    return aligned_size(vector_size);
}

size_t vector_calc_mem_size(uint elements, size_t element_size)
{
    size_t vector_size, mem_size;

    if (element_size != 2 && element_size != 4) {
        return 0;
    }

    vector_size = vector_calc_vector_size(elements, element_size);
    mem_size = sizeof(vector_t) + vector_size;

    return mem_size;
}

int vector_init(vector_t *vector, uint elements, size_t element_size)
{
    if (element_size != 2 && element_size != 4) {
        return -1;
    }

    vector->elements = elements;
    vector->element_size = element_size;
    vector->vector_size = vector_calc_vector_size(elements, element_size);
    vector->mem_size = vector_calc_mem_size(elements, element_size);
    VECTOR_ptr(vector) = ((char *)vector) + sizeof(vector_t);
    memset((void *)VECTOR_ptr(vector), '\0', vector->vector_size);

    return 0;
}

size_t matrix_calc_matrix_size(uint rows, uint columns,
                               size_t element_size)
{
    size_t matrix_size;

    if (element_size != 2 && element_size != 4) {
        return 0;
    }

    matrix_size = rows * columns * element_size;

    return aligned_size(matrix_size);
}

size_t matrix_calc_mem_size(uint rows, uint columns,
                            size_t element_size)
{
    size_t matrix_size, mem_size;

    if (element_size != 2 && element_size != 4) {
        return 0;
    }

    matrix_size = matrix_calc_matrix_size(rows, columns, element_size);
    mem_size = sizeof(matrix_t) + matrix_size;

    return mem_size;
}

int matrix_init(matrix_t *matrix, uint rows, uint columns, size_t element_size)
{
    if (element_size != 2 && element_size != 4) {
        return -1;
    }

    matrix->rows = rows;
    matrix->columns = columns;
    matrix->element_size = element_size;
    matrix->matrix_size = matrix_calc_matrix_size(rows, columns, element_size);
    matrix->mem_size = matrix_calc_mem_size(rows, columns, element_size);
    MATRIX_ptr(matrix) = ((char *)matrix) + sizeof(matrix_t);
    memset((void *)MATRIX_ptr(matrix), '\0', matrix->matrix_size);

    return 0;
}

void matrix_make_elementary(matrix_t *elementary, uint n)
{
    uint i;

    memset(MATRIX_ptr(elementary), '\0', \
           MATRIX_matrix_size(elementary));

    for (i=0;i<n;i++) {
        MATRIX_set(elementary, i * n + i, 1);
    }
}

void matrix_make_vandermonde_matrix(matrix_t *vandermonde,
    reed_solomon_t *rs,
    uint division)
{
    uint i, j;
    matrix_t *vm = vandermonde;

    for(i=0;i<division;i++)
    if (rs->register_size == 2)
        MATRIX_u(16, vm)[i] = 1;
    else
        MATRIX_u(32, vm)[i] = 1;

    if (rs->register_size == 2) {
        for(j=1;j<division;j++)
            for(i=0;i<division;i++)
                MATRIX_u(16, vm)[j * division + i] = \
                  _rs_mul16(rs, MATRIX_u(16, vm)[(j-1) * division + i], i + 1);
    }
    else {
        for(j=1;j<division;j++)
            for(i=0;i<division;i++)
                MATRIX_u(32, vm)[j * division + i] = \
                  _rs_mul32(rs, MATRIX_u(32, vm)[(j-1) * division + i], i + 1);
    }
}

/* 逆行列を計算するのは，復元時の最初の一回だけ。
 * そのため，以下の _rs_solve_inverse() の高速化をしても，
 * 高速化の恩恵はあまり得られない。
 * と思ってる。profile もせずに。
 *
 * reed solomon 復元時に，逆行列計算時間の占める割合は，
 * 一体如何ほどか？
 * という話。
 * 私も知らんけど。
 *
 * よって，以下では多少非効率な計算であっても，許容している。
 * 何よりも優先すべきは，記述量を減らすことであった。
 */
static int _rs_solve_inverse(matrix_t *inverse,
                             matrix_t *matrix,
                             reed_solomon_t *rs,
                             uint division,
                             vector_t *buffer)
{
    uint i, j, k, need_swap = 0;
    int index, index_j, index_k, index_x, index_y, index_z;
    matrix_t *im = inverse, *mt = matrix;
    uint tmp, tmp1, tmp2, tmp3;
    uint work;

    /* おい、気をつけろ。
     * matrix は破壊されてしまうぞ。
     * 気をつけろ。
     * こんなんで、何日もかかったとか。。。
     * 詳しくは、#260:  gaussian elimination の見直し。
     * を見るんだな。私の苦闘が記されている。
     */

    matrix_make_elementary(inverse, division);

    for (k=0;k<division;k++) {
        MATRIX_get(work, mt, k * division + k);

        if (work == 0) {
            need_swap = 0;
            for (j=k+1;j<division;j++) {
                MATRIX_get(work, mt, j * division + k);

                if (work) {
                    need_swap = 1;
                    _DEBUG(
                           "matrix[%d], matrix[%d] = matrix[%d], matrix[%d]\n",
                            k, j, j, k);

                    index_j = j * division * rs->register_size;
                    index_k = k * division * rs->register_size;
                    /*
                    do swap, via buffer
                    matrix[k], matrix[j] = matrix[j], matrix[k]
                    */
                    /* 高速化する必要はないといえど，memory の copy は気になる。
                     * まあ，手を入れたりはしません。
                     * このまま使います。
                     */
                    memcpy(VECTOR_ptr(buffer),
                           MATRIX_ptr(mt) + index_j,
                           VECTOR_row_size(buffer));
                    memcpy(MATRIX_ptr(mt) + index_j,
                           MATRIX_ptr(mt) + index_k,
                           MATRIX_row_size(mt));
                    memcpy(MATRIX_ptr(mt) + index_k,
                           VECTOR_ptr(buffer),
                           MATRIX_row_size(mt));

                    /*
                    do swap, via buffer
                    im[k], im[j] = im[j], im[k]
                    */
                    memcpy(VECTOR_ptr(buffer),
                           MATRIX_ptr(im) + index_j,
                           VECTOR_row_size(buffer));
                    memcpy(MATRIX_ptr(im) + index_j,
                           MATRIX_ptr(im) + index_k,
                           MATRIX_row_size(mt));
                    memcpy(MATRIX_ptr(im) + index_k,
                           VECTOR_ptr(buffer),
                           MATRIX_row_size(mt));
                    break;
                }
            }

            if (need_swap == 0) {
                /*
                message =
                    ('cannot make inverse. division = {}, ',
                     'gf_max = {}').format(self.division, self.gf_max)
                raise Par2RankError(message)
                */
                return RS_RANK_ERROR;
            }
        }

        MATRIX_get(tmp, mt, k * division + k);
        if (tmp != 1) {
            for (i=0;i<division;i++){
                index = k * division + i;
                if (rs->register_size == 2) {
                    MATRIX_get(tmp1, mt, index);
                    MATRIX_set(mt, index, _rs_div16(rs, tmp1, tmp));
                    MATRIX_get(tmp2, im, index);
                    MATRIX_set(im, index, _rs_div16(rs, tmp2, tmp));
                    /*
                    mt.u16[index] = _rs_div16(rs, mt.u16[index], tmp);
                    im.u16[index] = _rs_div16(rs, im.u16[index], tmp);
                    */
                }
                else {
                    MATRIX_get(tmp1, mt, index);
                    MATRIX_set(mt, index, _rs_div32(rs, tmp1, tmp));
                    MATRIX_get(tmp2, im, index);
                    MATRIX_set(im, index, _rs_div32(rs, tmp2, tmp));
                    /*
                    mt.u32[index] = _rs_div32(rs, mt.u32[index], tmp);
                    im.u32[index] = _rs_div32(rs, im.u32[index], tmp);
                    */
                }
            }
        }

        for (j=k+1;j<division;j++){
            MATRIX_get(tmp, mt, j * division + k);
            if (tmp == 0)
                continue;

            for (i=0;i<division;i++) {
                index_k = k * division + i;
                index_j = j * division + i;
/*
*/
                if (rs->register_size == 2) {
                    /*
                    tmp1 = mt.u16[index_k];
                    tmp3 = mt.u16[index_j];
                    */
                    MATRIX_get(tmp1, mt, index_k);
                    tmp2 = _rs_mul16(rs, tmp, tmp1);
                    MATRIX_get(tmp3, mt, index_j);
                    MATRIX_u(16, mt)[index_j] = _rs_ADD(tmp3, tmp2);

                    MATRIX_get(tmp1, im, index_k);
                    tmp2 = _rs_mul16(rs, tmp, tmp1);
                    MATRIX_get(tmp3, im, index_j);
                    MATRIX_u(16, im)[index_j] = _rs_ADD(tmp3, tmp2);
                }
                else {
                    MATRIX_get(tmp1, mt, index_k);
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    MATRIX_get(tmp3, mt, index_j);
                    MATRIX_u(32, mt)[index_j] = _rs_ADD(tmp3, tmp2);

                    MATRIX_get(tmp1, im, index_k);
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    MATRIX_get(tmp3, im, index_j);
                    MATRIX_u(32, im)[index_j] = _rs_ADD(tmp3, tmp2);
                /*
                    tmp1 = mt.u32[index_k];
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    tmp3 = mt.u32[index_j];
                    mt.u32[index_j] = _rs_ADD(tmp3, tmp2);

                    tmp1 = im.u32[index_k];
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    tmp3 = im.u32[index_j];
                    im.u32[index_j] = _rs_ADD(tmp3, tmp2);
                */
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

            MATRIX_get(tmp, mt, index_y * division + index_x);

            for (i=0;i<division;i++) {
/*
                tmp1 = mt[index_z * division + i];
                tmp2 = _mul(rse, tmp, tmp1);
                tmp3 = mt[index_y * division + i];
                mt[index_y * division + i] = _add(tmp3, tmp2);
*/
                if (rs->register_size == 2) {
                    tmp1 = MATRIX_u(16, im)[index_z * division + i];
                    tmp2 = _rs_mul16(rs, tmp, tmp1);
                    tmp3 = MATRIX_u(16, im)[index_y * division + i];
                    MATRIX_u(16, im)[index_y * division + i] = _rs_ADD(tmp3, tmp2);
                }
                else {
                    tmp1 = MATRIX_u(32, im)[index_z * division + i];
                    tmp2 = _rs_mul32(rs, tmp, tmp1);
                    tmp3 = MATRIX_u(32, im)[index_y * division + i];
                    MATRIX_u(32, im)[index_y * division + i] = _rs_ADD(tmp3, tmp2);
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
    if (mod != 0)
        padding_size = ALIGHNMENT_SIZE - mod;
    else
        padding_size = 0;
    return size + padding_size;
}

static big_bang_t _dokkaan = {
/* allocate_size       mem          mem_status */
               0,     NULL, BB_MEM_NO_ALLOCATE, {
/* reed_solomon_t:
   bits      poly  symbol_size   register_size
                                       w, gf_max,
                                             *gf,  *gfi, allocate_size */
    { 4,       19,      1,          2, 0, 0, NULL, NULL, 0},
    { 8,      285,      1,          2, 0, 0, NULL, NULL, 0},
    {16,    65581,      2,          2, 0, 0, NULL, NULL, 0},
    {24, 16777243,      3,          4, 0, 0, NULL, NULL, 0},
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
    LOGGER(INFO, "                gf = %p\n", rs->gf);
    LOGGER(INFO, "               gfi = %p\n", rs->gfi);
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
    size_t vector_mem_size;

    rs->w = 1 << rs->bits;
    rs->gf_max = rs->w - 1;

    vector_mem_size = \
        vector_calc_mem_size(RS_w(rs), rs->register_size);

    rs->allocate_size = vector_mem_size * 2; /* for gf and gfi */

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

        RS_gf(rs) = (vector_t *)mem;
        vector_init(RS_gf(rs), RS_w(rs), RS_register_size(rs));
        mem += VECTOR_mem_size(RS_gf(rs));

        RS_gfi(rs) = (vector_t *)mem;
        vector_init(RS_gfi(rs), RS_w(rs), RS_register_size(rs));
        mem += VECTOR_mem_size(RS_gfi(rs));
    }
    _DEBUG("mem=%p, universe->mem=%p\n", mem, universe->mem);
    _DEBUG("mem - universe->mem = 0x%08x, allocate_size=0x%08x\n", mem - universe->mem, allocate_size);

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

static void _rs_view_matrix16(matrix_t *mat)
{
    uint i, j;
    extern FILE *_log;
    extern int _log_level;

    if (_log != NULL && DEBUG_ >= _log_level) {
        fprintf(_log, "_rs_view_matrix16(matrix=%p, columns=%u, rows=%u))\n",
                                   mat, MATRIX_columns(mat), MATRIX_rows(mat));
        fprintf(_log, "     ");
        for (i=0;i<MATRIX_columns(mat);i++)
            fprintf(_log, "%4x ", i);
        fprintf(_log, "\n");
        for (j=0;j<MATRIX_rows(mat);j++) {
            fprintf(_log, "%4x ", j);
            for (i=0;i<MATRIX_columns(mat);i++) {
                fprintf(_log, "%4x ", MATRIX_u(16, mat)[j * MATRIX_rows(mat) + i]);
            }
            fprintf(_log, "\n");
        }
    }
}

static void _rs_view_matrix32(uint *matrix, uint division)
{
    uint i, j;
    extern FILE *_log;
    extern int _log_level;

    if (_log != NULL && DEBUG_ >= _log_level) {
        fprintf(_log, "_rs_view_matrix32(matrix=%p, division=%u)\n",
                                     matrix, division);
        fprintf(_log, "         ");
        for (i=0;i<division;i++)
            fprintf(_log, "%8x ", i);
        fprintf(_log, "\n");
        for (j=0;j<division;j++) {
            fprintf(_log, "%8x ", j);
            for (i=0;i<division;i++) {
                fprintf(_log, "%8x ", matrix[j * division + i]);
            }
            fprintf(_log, "\n");
        }
    }
}

static void _rs_view_vector16(vector_t *vector)
{
    uint i;
    extern FILE *_log;
    extern int _log_level;

    if (_log != NULL && DEBUG_ >= _log_level) {
        fprintf(_log, "_rs_view_vector16(vector=%p)\n", vector);
        fprintf(_log, "sizeof(vector_t)=%lu\n", sizeof(vector_t));
        fprintf(_log, "elements=%u\n", VECTOR_elements(vector));
        fprintf(_log, "element_size=%lu\n", VECTOR_element_size(vector));
        fprintf(_log, "vector_size=%lu\n", VECTOR_vector_size(vector));
        fprintf(_log, "mem_size=%lu\n", VECTOR_mem_size(vector));
        fprintf(_log, "mem=%p\n", VECTOR(vector));

        for (i=0;i<VECTOR_elements(vector);i++)
            fprintf(_log, "%4x ", i);
        fprintf(_log, "\n");
        for (i=0;i<VECTOR_elements(vector);i++)
            fprintf(_log, "%4x ", VECTOR_u(16, vector)[i]);
        fprintf(_log, "\n");
    }
}

static void _rs_view_vector32(vector_t *vector)
{
    uint i;
    extern FILE *_log;
    extern int _log_level;

    if (_log != NULL && DEBUG_ >= _log_level) {
        fprintf(_log, "_rs_view_vector32(vector=%p)\n", vector);
        fprintf(_log, "sizeof(vector_t)=%lu\n", sizeof(vector_t));
        fprintf(_log, "elements=%u\n", VECTOR_elements(vector));
        fprintf(_log, "element_size=%lu\n", VECTOR_element_size(vector));
        fprintf(_log, "vector_size=%lu\n", VECTOR_vector_size(vector));
        fprintf(_log, "mem_size=%lu\n", VECTOR_mem_size(vector));
        fprintf(_log, "mem=%p\n", VECTOR(vector));

        for (i=0;i<VECTOR_elements(vector);i++)
            fprintf(_log, "%4x ", i);
        fprintf(_log, "\n");
        for (i=0;i<VECTOR_elements(vector);i++)
            fprintf(_log, "%4x ", VECTOR_u(32, vector)[i]);
        fprintf(_log, "\n");
    }
}


#ifdef __TEST__

/*****************************************************************************/
/* for test functions ********************************************************/
/*****************************************************************************/

void _rs_make_gfgfi_wrap(reed_solomon_t *rs)
{
    return _rs_make_gf_and_gfi(rs);
}

ushort _rs_mul16_wrap(reed_solomon_t *rs, ushort a, ushort b)
{
    return _rs_mul16(rs, a, b);
}

void matrix_make_elementary_wrap(matrix_t *elementary, uint n)
{
    matrix_make_elementary(elementary, n);
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

void _rs_mul_matrix_vector16_wrap(reed_solomon_t *rs,
                                  vector_t *answer,
                                  matrix_t *mat,
                                  vector_t *vec)
{
    return rs_mul_matrix_vector16(rs, answer, mat, vec);
}

void _rs_mul_matrix_vector32_wrap(reed_solomon_t *rs,
                                  vector_t *answer,
                                  matrix_t *mat,
                                  vector_t *vec)
{
    return rs_mul_matrix_vector32(rs, answer, mat, vec);
}

void _rs_mul_matrixes32_wrap(reed_solomon_t *rs,
                           matrix_t *answer,
                           matrix_t *mat1,
                           matrix_t *mat2)
{
    return _rs_mul_matrixes32(rs, answer, mat1, mat2);
}

void _rs_mul_matrixes_wrap(reed_solomon_t *rs,
                           matrix_t *answer,
                           matrix_t *mat1,
                           matrix_t *mat2)
{
    #define _DEBUG_ 0
    #if _DEBUG_
    fprintf(stderr, "in _rs_mul_matrixes_wrap()\n");
    fprintf(stderr, "answer:\n");
    _rs_view_matrix16(answer);
    fprintf(stderr, "mat1:\n");
    _rs_view_matrix16(mat1);
    fprintf(stderr, "mat2:\n");
    _rs_view_matrix16(mat2);
    fprintf(stderr, "\n");
    fprintf(stderr, "do _rs_mul_matrixes_wrap()\n");
    fprintf(stderr, "\n");
    #endif
    _rs_mul_matrixes16(rs, answer, mat1, mat2);
    #if _DEBUG_
    fprintf(stderr, "answer:\n");
    _rs_view_matrix16(answer);
    fprintf(stderr, "mat1:\n");
    _rs_view_matrix16(mat1);
    fprintf(stderr, "mat2:\n");
    _rs_view_matrix16(mat2);
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    #endif
}

void _rs_view_matrix32_wrap(uint *matrix, uint division)
{
    _rs_view_matrix32(matrix, division);
}

void _rs_view_matrix16_wrap(matrix_t *matrix)
{
    _rs_view_matrix16(matrix);
}

void _rs_view_vector16_wrap(vector_t *vector)
{
    _rs_view_vector16(vector);
}

void _rs_view_vector32_wrap(vector_t *vector)
{
    _rs_view_vector32(vector);
}

void _matrix_make_vandermonde_wrap(
    matrix_t *vandermonde,
    reed_solomon_t *rs,
    uint division)
{
    matrix_make_vandermonde_matrix(vandermonde, rs, division);
    #if 0
    fprintf(stderr, "_matrix_make_vandermonde_wrap()\n");
    _rs_view_matrix16(MATRIX_u(16, vandermonde), division);
    #endif
}

int _rs_solve_inverse_wrap(matrix_t *inverse,
                           matrix_t *matrix,
                           reed_solomon_t *rs,
                           uint division,
                           vector_t *buffer)
{
    int ret;
    ret = _rs_solve_inverse(inverse, matrix, rs, division, buffer);
    #if 0
    fprintf(stderr, "_rs_solve_inverse_wrap()=%d\n", ret);
    _rs_view_matrix16(MATRIX_u(16, inverse), division);
    #endif
    return ret;
}

#endif
