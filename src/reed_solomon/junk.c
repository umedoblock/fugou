/* from rs.c *****************************************************************/
static void _rs_encode16_slots(slot_t *parity,
                               slot_t *norm,
                               rs_encode_t *rse,
                               uint symbol_num);
static void _rs_encode32_slots(slot_t *parity,
                               slot_t *norm,
                               rs_encode_t *rse,
                               uint symbol_num);
static void _rs_decode32_slots(slot_t *recover,
                               slot_t *merged,
                               rs_decode_t *rsd,
                               uint symbol_num);
static void _rs_decode16_slots(slot_t *recover,
                               slot_t *merged,
                               rs_decode_t *rsd,
                               uint symbol_num);

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

/* _rs_solve_inverse(matrix_t *inverse, */
                /*
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
                */

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
#endif


/* rs.c static ushort _rs_div16(reed_solomon_t *rs, ushort a, ushort b) */
    /* u16 同士の差を求めて、その差が0以上かを調べている以下の箇所は
     * bug臭いが cast がうまい具合に働いているんだろうか？
     * c = rs->gf.u16[a] - rs->gf.u16[b];
     * 不安でしょうがないので cast するように修正。
     * c = (int )rs->gf.u16[a] - (int )rs->gf.u16[b];
     */
    #if 0
    c = (int )VECTOR_u(16, rs->gf)[a] - (int )VECTOR_u(16, rs->gf)[b];
    if (c >= 0)
        return VECTOR_u(16, rs->gfi)[c];
    return VECTOR_u(16, rs->gfi)[c + RS_gf_max(rs)];

    う〜ん。どうしようかな？
    _rs_mulXX() のようなことがあると怖いけど、
    bug っていないようだ。
    #endif

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

#if 0
def _mul_matrixes(self, a, b):
    answer = self._make_square_matrix()
    for j in range(self.division):
        for i in range(self.division):
            tmp = 0
            for k in range(self.division):
                muled = self.rds._mul(a[j][k], b[k][i])
                tmp = self.rds._add(tmp, muled)
            answer[j][i] = tmp
    return answer
#endif

typedef struct {                  | typedef struct {
    size_t allocate_size;         |     uint bits;
    char *mem;                    |     uint poly;
    int mem_status;               |     size_t symbol_size;
    reed_solomon_t rs[RS_GF_NUM]; |
} big_bang_t;                     |     size_t register_size;
                                  |
                                  |     uint w;
                                  |     uint gf_max;
                                  |     vector_t *gf;
                                  |     vector_t *gfi;
                                  |
                                  |     size_t allocate_size;
                                  | } reed_solomon_t;

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
/***************************************************************** from rs.c */
