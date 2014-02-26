/* Copyright (C) 平成20-25(2008-2013)年 梅濁酒(umedoblock)
 #258  libpar の仕様をはっきりさせる。
 rs library header
 Reed Solomon 符号により、任意のdataに対して冗長性を持たせる。

 -----------------------------------------------------------------------------
 specific in English.

 REED SOLOMON THEORY.

 simple over view:
 text = split() => norm slots
 norm slots = encode() => parity slots
 collect norm and parity slots = recover() => recovery
 RECOVERY MUST BE EQUAL TO TEXT.

 define:
 octet means 8 bits.
 xxx_size means octet length of xxx.
 kilo is defined by power(2, 10).
 kilo is equal to 1024.
 kilo octet equal to 1024 * 8 bits.
 kilo octet stands for KO.

 init:
 text is text data.
 text_size is text size.
 division describes that how many split to text.
 division of one means that copy.
 therefore division must be satisfied below condition.
    2 <= division <= gf_max.
    NODE: what is gf_max ?
          please study reed solomon theory.
          please see in rs_init_rs().
          数学のお話。説明大変だよー。
 redundancy must be satisfied below condition.
    1 <= redundancy <= division.

 padding and normalize:
 remainder_size = text_size mod division
 padding_size = division - remainder_size
 padding = "\0" * padding_size
 if remainder_size is equal to zero, padding is empty string,
 padding_size is zero.
 norm = text + padding
    norm means normalized text.
 norm_size = text_size + padding_size
 therefore, norm_size is multiple of division.

 calc slot_size;
 slot_size = norm_size / division
 norm_slot_size = slot_size
 parity_slot_size = slot_size

 split and encode:
 norm_slots = split(norm, --bytes=norm_slot_size)
    split() like the split command.
    therefore norm_slots of max length is equal to division.
 parity_slots = reed_solomon_encode(norm_slots, redundancy)

 ... deliverly diffuse norm_slots and parity_slots to the world ...

 collect, merge:
 available_slots = collect(norm_slots, parity_slots)
 number of available_slots must be greater than division or equal.
 merged_slots = merge(available_slots)
    merge() like the merge command.

 recover:
 recovery_slots = reed_solomon_recover(merged_slots)
 recovery_slots are equal to norm_slots.
 recovery = cat(recovery_slots)
    cat() like the cat command.
 come_back = recovery - padding

 validate:
 text == come_back

 -----------------------------------------------------------------------------
 example:

 text is below 21 charactors.
    "abcdef0123456789vwxyz"
 text_size = 21
 division = 5
 redundancy = 3

 remainder_size = 1 = text_size(=21) mod division(=5)
 padding_size = 4 = division(=5) - remainder_size(=1)
 norm_size = 25 = text_size(=21) + padding_size(=4)
 slot_size = 5 = norm_size(=25) / (division=5)
 padding:
    "\0\0\0\0"
 norm:
    "abcdef0123456789vwxyz\0\0\0\0"

 norm_slots:
    slot_no, norm_slot
    0, "abcde"
    1, "f0123"
    2, "45678"
    3, "9vwxy"
    4, "z\0\0\0\0"

 2 <= redundancy <= division(=5)
 parity_slots:
    slot_no, parity_slot
    5, "PPPP5"
    6, "PPPP6"
    7, "PPPP7"

 collect available_slots are no 6, 0, 7, 5, 4.
 available_slots:
    slot_no, available_slot
    6, "PPPP6"
    0, "abcde"
    7, "PPPP7"
    5, "PPPP5"
    4, "z\0\0\0\0"

 merged_slots:
    slot_no, merged_slot
    0, "abcde"
    5, "PPPP5"
    6, "PPPP6"
    7, "PPPP7"
    4, "z\0\0\0\0"

 recovery_slots:
    slot_no, recovery_slot
    0, "abcde"
    1, "f0123"
    2, "45678"
    3, "9vwxy"
    4, "z\0\0\0\0"

 recovery:
    "abcdef0123456789vwxyz\0\0\0\0"

 come_back: recovery - padding
    "abcdef0123456789vwxyz"

 come_back is equal to text.
 also norm_slots and recovery_slots.

 -----------------------------------------------------------------------------

 構造体について
 rs_encode_t => 符号化用, norm slot から parity slot を生成する。
 rs_decode_t => 復号用, norm or parity slot から、text を復元する。

 */

#ifndef __RS_H__
#define __RS_H__

#include "../slot/slot.h"
#include "../libfugou_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _rs_ADD(a, b) (a ^ b)
#define RS_BUFFER_SIZE 80
#define ALIGHNMENT_SIZE (sizeof(void *))

typedef union {
    /* assign available memory address to ptr */
    char *ptr;
    /* u16 and u32 need for rs_encode_t */
    ushort *u16;
    uint *u32;
} _ptr_t;

typedef struct {
    uint bits;
    uint poly;
    size_t symbol_size; /* for reed solomon error correction */

    size_t register_size; /* for cpu */

    uint w;
    uint gf_max;
    _ptr_t gf;
    _ptr_t gfi;
    size_t gf_size;

    size_t allocate_size;
} reed_solomon_t;

/* row, 行
 * column, 列
 *              column
 *                |
 *                |
 *                |
 *                |
 * row -----------+-----------
 *                |
 *                |
 *                |
 *                |
 */

typedef struct {
    uint elements;       /* = division */
    size_t element_size; /* see name */
    size_t vector_size;  /* = elements * element_size */
    size_t mem_size;     /* = sizeof(vector_t) + vector_size */
    _ptr_t mem;          /* memory area */
} vector_t;

typedef struct {
    uint columns;        /* = division */
    uint rows;           /* = division */
    size_t element_size; /* see name */
    size_t matrix_size;  /* = columns * rows * element_size */
    size_t mem_size;     /* = sizeof(matrix_t) + matrix_size */
    _ptr_t mem;          /* memory area */
} matrix_t;

typedef struct {
    uint poly;
    uint division;

    size_t mem_size; /* = 3 * matrix_size + 2 * vector_size */
    _ptr_t mem;
    matrix_t vandermonde; /* rse */
    matrix_t inverse;     /* rsd */
    matrix_t elementary;  /* rsd */
    vector_t vec1;        /* rse and rsd, for tempolary area */
    vector_t vec2;        /* rse and rsd, for tempolary area */
} rs_readonly_t;

typedef struct {
    uint poly;
    uint division;

    size_t mem_size; /* = 3 * matrix_size + 2 * vector_size */
    _ptr_t mem;
    matrix_t vandermonde; /* rse */
    matrix_t inverse;     /* rsd */
    matrix_t elementary;  /* rsd */
    vector_t vec1;        /* rse and rsd, for tempolary area */
    vector_t vec2;        /* rse and rsd, for tempolary area */
} rs_writable_t;

#define MATRIX(mtrx) ((matrix_t *)mtrx)
#define MATRIX_matrix_size(mtrx) (MATRIX(mtrx)->matrix_size)
#define MATRIX_element_size(mtrx) (MATRIX(mtrx)->element_size)
#define MATRIX_mem(mtrx) (MATRIX(mtrx)->mem)
#define MATRIX_mem_size(mtrx) (MATRIX(mtrx)->mem_size)
#define MATRIX_ptr(mtrx) (MATRIX_mem(mtrx).ptr)
#define MATRIX_uXX(mtrx, XX) (MATRIX_mem(mtrx).u ## XX)
#define MATRIX_get(mtrx, XX, INDEX, VALUE) \
    for(;;) { \
    if (MATRIX_element_size(mtrx) == 2) { \
        VALUE = MATRIX_uXX(mtrx, 16)[INDEX]; \
    } \
    else { \
        VALUE = MATRIX_uXX(mtrx, 32)[INDEX]; \
    } \
    break; \
    }
#define MATRIX_set(mtrx, INDEX, VALUE) \
    for(;;) { \
    if (MATRIX_element_size(mtrx) == 2) { \
        MATRIX_uXX(mtrx, 16)[INDEX] = VALUE; \
    } \
    else { \
        MATRIX_uXX(mtrx, 32)[INDEX] = VALUE; \
    } \
    break; \
    }

#define RS_ptr(r) ((reed_solomon_t *)r)
#define RS_bits(r) (RS_ptr(r)->bits)
#define RS_poly(r) (RS_ptr(r)->poly)
#define RS_symbol_size(r) (RS_ptr(r)->symbol_size)
#define RS_register_size(r) (RS_ptr(r)->register_size)
#define RS_w(r) (RS_ptr(r)->w)
#define RS_gf_max(r) (RS_ptr(r)->gf_max)
#define RS_gf(r) (RS_ptr(r)->gf)
#define RS_gfi(r) (RS_ptr(r)->gfi)
#define RS_gf16(r) (RS_gf(r).u16)
#define RS_gfi16(r) (RS_gfi(r).u16)
#define RS_gf_size(r) (RS_ptr(r)->gf_size)
#define RS_allocate_size(r) (RS_ptr(r)->allocate_size)

#define RS_division(r) (RS_ptr(r)->division)

#define UINT_BITS (sizeof(uint)*8)
#define RS_SET_REDUNDANCY_BIT(map, nbit) \
       (map[nbit/UINT_BITS]|=(1<<(nbit%UINT_BITS)))
#define RS_GET_REDUNDANCY_BIT(map, nbit) \
       (map[nbit/UINT_BITS]&(1<<(nbit%UINT_BITS)))
#define RS_CLEAR_REDUNDANCY_BIT(map, nbit) \
       (map[nbit/UINT_BITS]&=(~(1<<(nbit%UINT_BITS))))

typedef struct {
    reed_solomon_t *rs;
    /* ssb->target_size -= ssb->padding_size when last slot, slots[division-1].
     * division must be 2 or more.
     */
    uint division;
    uint redundancy;

    uint *redundancy_map;

    _ptr_t vandermonde;
    _ptr_t _row; /* horizontal vector */
    _ptr_t _row2; /* horizontal vector */

    size_t allocate_size; /* no need ? */
    size_t matrix_size;
    size_t _row_size;
} rs_encode_t;

typedef struct {
    reed_solomon_t *rs;
    uint division;

    _ptr_t merged;
    _ptr_t inverse;
    _ptr_t _row; /* horizontal vector */
    _ptr_t _row2; /* horizontal vector */
    _ptr_t _column; /* vertical vector */

    size_t allocate_size; /* no need ? */
    size_t matrix_size;
    size_t _row_size;
    size_t _column_size;
} rs_decode_t;

#define RS_GF_NUM (4)
#define RS_GF_4   (0)
#define RS_GF_8   (1)
#define RS_GF_16  (2)
#define RS_GF_24  (3)

typedef struct {
    size_t allocate_size;
    char *mem;
    int mem_status;
    reed_solomon_t rs[RS_GF_NUM];
} big_bang_t;

#define BB_MEM_NO_ALLOCATE (1)
#define BB_MEM_ALLOCATED (2)
#define BB_MEM_FREED (3)

#define RS_SUCCESS (0)
#define RS_MALLOC_ERROR (-1)
#define RS_INVALID_DIVISION_ERROR (-2)
#define RS_INVALID_BITS_ERROR (-3)
#define RS_RANK_ERROR (-4)
#define RS_RSD_AND_RSE_SET_ERROR (-5)
#define RS_NORM_SIZE_ERROR (-6)
#define RS_FREE_ERROR (-7)
#define RS_BIG_BANG_ERROR (-8)
#define RS_FATE_ERROR (-9)

int rs_big_bang(void);
int rs_ultimate_fate_of_the_universe(void);

int rs_take_rs(reed_solomon_t **rs, uint bits, uint division);

void rs_encode_slots(slot_t *parity,
                     slot_t *norm,
                     rs_encode_t *rse,
                     uint symbol_num);
void rs_decode_slots(slot_t *recover,
                     slot_t *merged,
                     rs_decode_t *rsd,
                     uint symbol_num);
size_t aligned_size(size_t size);
int _rs_take_rs(reed_solomon_t **rs, uint bits, uint division);

/*****************************************************************************/
/* prototype *****************************************************************/
/*****************************************************************************/
size_t matrix_calc_matrix_size(uint rows, uint columns, size_t element_size);
size_t matrix_calc_mem_size(uint rows, uint columns, size_t element_size);
int matrix_init(matrix_t *matrix, uint rows, uint columns, size_t element_size);
void matrix_make_elementary(matrix_t *elementary, uint n);

#ifdef __cplusplus
}
#endif
#endif /* __RS_H__ */
