/* Copyright (C) 平成20-25(2008-2013)年 梅どぶろく(umedoblock)
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

#ifndef RS_H
#define RS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "../libfugou_base.h"
#include "../sha/sha.h"

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

typedef struct {
    size_t remainder_size;
    size_t padding_size;
    size_t norm_size;
    size_t slot_size;
} _slot_size_brother_t;

typedef struct {
    /*
    size_t slot_size;
    */
    uchar *slot;
    FILE *fp;
    sha1sum_t sha1sum[1];
    _slot_size_brother_t ssb_;
} rs_slot_t;

typedef struct {
    reed_solomon_t *rs;
    uint division;

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
    reed_solomon_t rs[RS_GF_NUM];
} big_bang_t;

#define RS_SCUCCESS (0)
#define RS_MALLOC_ERROR (-1)
#define RS_INVALID_DIVISION_ERROR (-2)
#define RS_INVALID_BITS_ERROR (-3)
#define RS_RANK_ERROR (-6)
#define RS_RSD_AND_RSE_SET_ERROR (-11)

int rs_big_bang(void);
int rs_ultimate_fate_of_the_universe(void);

void rs_encode_slots(rs_slot_t *parity,
                     rs_slot_t *norm,
                     rs_encode_t *rse,
                     uint symbol_num);
void rs_decode_slots(rs_slot_t *recover,
                     rs_slot_t *merged,
                     rs_decode_t *rsd,
                     uint symbol_num);

#ifdef __cplusplus
}
#endif
#endif /* RS_H */
