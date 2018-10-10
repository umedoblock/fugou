#include "slot.h"
#include "../reed_solomon/rs.h"

#define SS_SIZE (1024)
#define TEST_MAX_SLOTS (257)

char *tmp_dir = ".";
const char *random_1048576_bin = "random_1048576.bin";
char msg[SS_SIZE];
char ss[SS_SIZE];

uchar *dump_1048576;
uchar *tmp;
char random_path[SS_SIZE]; /* tmp_dir/random_1048576.bin */

typedef struct {
    reed_solomon_t *rs;
    uint division;
    matrix_t *vandermonde;
    vector_t *parity_vector;
    vector_t *data_vector;
    size_t matrix_mem_size;
    size_t vector_mem_size;
    uint symbol_num;
} reed_solomon_encode_t;

void set_rse(reed_solomon_encode_t *rse, uint bits, uint division, uchar *mem)
{
    size_t matrix_mem_size, vector_mem_size;

    rse->division = division;
    rs_take_rs(&rse->rs, bits, division);
    rse->symbol_num = 0;

    matrix_mem_size = \
        matrix_calc_mem_size(division, division, rse->rs->register_size);
    rse->matrix_mem_size = matrix_mem_size;
    vector_mem_size = vector_calc_mem_size(division, rse->rs->register_size);
    rse->vector_mem_size = vector_mem_size;

    rse->vandermonde = (matrix_t *)mem; mem += matrix_mem_size;
    rse->parity_vector = (vector_t *)mem; mem += vector_mem_size;
    rse->data_vector = (vector_t *)mem; mem += vector_mem_size;
}

static size_t _rs_encode16_slots(slot_t *parity,
                                 slot_t *data,
                                 reed_solomon_t *rs,
                                 matrix_t *vandermonde,
                                 vector_t *parity_vector,
                                 vector_t *data_vector,
                                 uint division,
                                 uint symbol_num)
{
    uint i, j, k;
    size_t symbol_size = rs->symbol_size, encoded_size=0;
    uchar unum;
    uint num;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += SLOT_buf(data + j)[i * symbol_size + k];
            }
            VECTOR16(data_vector)[j] = num;
        }

        /*
        rs_mul_matrix_vector16(rs, parity_vector, vandermonde, data_vector);
        */

        for (j=0;j<division;j++) {
            num = VECTOR16(parity_vector)[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                SLOT_buf(parity + j)[i * symbol_size + k] = unum;
            }
        }
        encoded_size += VECTOR_row_size(data_vector);
    }

    return encoded_size;
}

static size_t slot_reed_solomon_encode(slot_t *parity,
                                     slot_t *data,
                                     uint division,
                                     void *args)
{
    reed_solomon_encode_t *rse = args;
    return _rs_encode16_slots(parity, data,
                              rse->rs,
                              rse->vandermonde,
                              rse->parity_vector, rse->data_vector,
                              rse->division, rse->symbol_num);
}

void sample_slot_divide_and_computing_and_integrate(
    slot_t *parent, slot_t *children, uchar *mem)
{
    /*
    1048576 % 41 =  1
   (1048576 + 122) / 41 = 25578.0, column_size = 123, division = 41
    */
    uint i, division = 41, symbol_size = 3, bits = 8;
    size_t parent_target_size = 0, child_slot_size, child_target_size;
    size_t integrate_target_size, mem_size;
    int ret;
    reed_solomon_encode_t _rse, *rse = &_rse;

    /* file to file で試してみる。
     * parent file を children file に分割。
     * children file を integrate file に統合。
     */
    SLOT_writing(parent) = slot_fwrite;
    SLOT_computing(parent) = slot_reed_solomon_encode;
    SLOT_reading(parent) = slot_fread;
    for (i=0;i<division;i++) {
        SLOT_writing(children_i) = slot_fwrite;
        /*
        SLOT_computing(children_i) = slot_reed_solomon_recover;
        */
        SLOT_reading(children_i) = slot_fread;
    }

    slot_file_named(SLF(parent), tmp_dir, random_1048576_bin);
    slot_file_fopen(SLF(parent), "rb");
    parent_target_size = slot_ask_target_size(parent, FROM_HEAD);

    slot_calc_sb_by_division(parent, children,
                             parent_target_size,
                             division,
                             symbol_size, 0);

    slot_children_named(SLF(children), parent, division);
    slot_children_fopen(SLF(children), "wb+", division);
    slot_children_set_first_pos(parent, children, division);

/*
typedef struct {
    reed_solomon_t *rs;
    matrix_t *vandermonde;
    vector_t *parity_vector;
    vector_t *data_vector;
    uint division;
    uint symbol_num;
} reed_solomon_encode_t;
*/

    /* reed solomon の設定 */
    set_rse(rse, bits, division, mem);
    child_slot_size = SLOT_slot_size(children);
    rse->symbol_num = child_slot_size / rse->rs->symbol_size;

    /* parent => children の分割を行う。*/
    /* sample の目玉 */
    /*
    ret = _slot_divide(children, parent, division, (void *)rse);
    */

    #if 0
    for (i=0;i<division;i++) {
        rewind(SLOT_target_f(children_i));
        fread(tmp, 1, child_slot_size,
                      SLOT_target_f(children_i));
        expected_mem = dump_1048576 + i * child_slot_size;
        result_mem = tmp;
        sprintf(msg, "test__slot_divide_and_integrate() with "
                     "SLOT_target(children+%u) == "
                     "dump_1048576 + i(=%u) * child_slot_size(=%zu)",
                      i, i, child_slot_size);
        if (i != division - 1) {
           assert_by_mem(expected_mem, result_mem,
                          child_slot_size, msg);
        }
        else {
            /* 最終 slot, last slot */
            child_target_size = SLOT_target_size(children_i);
            zero_size = child_slot_size - child_target_size;
            sprintf(msg, "test__slot_divide_and_integrate() with "
                         "SLOT_padding_size(children_i), zero_size");
           assert_by_size(SLOT_padding_size(children_i), zero_size, msg);
            sprintf(msg, "test__slot_divide_and_integrate() with "
                         "assert_by_mem(expected_mem, result_mem)");
           assert_by_mem(expected_mem, result_mem,
                          child_target_size, msg);
           /* 簡易版 debug 関数
           sprintf(msg, "child_target_size=%u, zero_size=%u, "
                        "child_slot_size=%u, padding_size=%u\n",
                         child_target_size, zero_size,
                         child_slot_size, SLOT_padding_size(children_i));
           assert_true(0, msg);
           */
            sprintf(msg, "test__slot_divide_and_integrate() with "
                         "assert_by_00(result_mem=%p + child_target_size=%zu, "
                         "zero_size=%zu)",
                          result_mem, child_target_size, zero_size);
           assert_by_00(result_mem + child_target_size, zero_size, msg);
        }
    }
    #endif

#if 0
    /* children は使い回すので、file pointer を先頭に戻しておく。*/
    for (i=0;i<division;i++) {
        rewind(SLOT_target_f(children_i));
    }

    /* parent の target を integrate で上書きする前に、
     * parent の target を閉じておく。しかし、削除はしない。
     */
    slot_file_fclose(SLF(parent));

    strcat(SLOT_name(parent), ".integrate");
    fp = fopen(SLOT_name(parent), "wb+");
    SLOT_target(parent) = fp;

    slot_children_set_first_pos(parent, children, division);
    for (i=0;i<division;i++) {
        sprintf(msg, "slot_children_set_first_pos(parent=%p, children=%p, "
                     "division=%u) i=%u",
                      parent, children, division, i);
        assert_by_size(0, SLOT_index(children_i), msg);
    }

    /* children => parent の統合 */
    ret = _slot_integrate(parent, children, args);
    sprintf(msg, "_slot_divide_or_integrate(parent=%p, children=%p, "
                 "division=%u, slot_fread, slot_fwrite)",
                  parent, children, division);
   assert_success(ret, msg);

    integrate_target_size = slot_ask_target_size(parent, FROM_HEAD);
    sprintf(msg, "_slot_integrate(parent=%p, children=%p, "
                 "division=%u, slot_fread, slot_fwrite) "
                 "integrate_target_size=%zu",
                  parent, children, division, integrate_target_size);
   assert_by_size(1048576, integrate_target_size, msg);

    rewind(SLOT_target_f(parent));
    fread(tmp, 1, 1048576, SLOT_target_f(parent));
    sprintf(msg, "_slot_divide_or_integrate(parent=%p, children=%p, "
                 "SLOT_INTEGRATE, ...", parent, children);
   assert_by_mem(dump_1048576, tmp, 1048576, msg);
   /*
   */
#endif

    /* 後始末 */
    #if 0
    slot_children_fclose(SLF(children), division);
    slot_children_remove(SLF(children), division);

    /* test 実行の度に _slot_divide_or_integrate(verb=SLOT_INTEGRATE)
     * の実行結果として integrate が作成されるから、integrate は毎回削除。
     */
    slot_file_fclose(SLF(parent));
    #endif
    /*
    slot_file_remove(SLF(parent));
     */

}

int main(int argc, char *argv[])
{
    size_t mem_size = 0;
    uint division = 41;
    FILE *fp;
    uchar *dump_1048576;
    uchar *mem, *_mem;
    slot_t *parent, *children;
    size_t matrix_mem_size, vector_mem_size;

    mem_size += 1048576 * 2;
    mem_size += (1 + TEST_MAX_SLOTS) * slot_get_memory_size();

    mem = (uchar *)malloc(mem_size);
    if (mem == NULL) {
        fprintf(stderr, "faild malloc(mem_size=%lu)\n", mem_size);
        return -1;
    }
    memset(mem, '\0', mem_size);
    _mem = mem;

    dump_1048576 = mem; mem += 1048576;
    tmp = mem; mem += 1048576;

    parent = SLT(mem);
    parent = slot_set_memory((uchar *)parent, 1);
    mem += slot_get_memory_size();
    children = SLT(mem);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);
    mem += slot_get_memory_size() * TEST_MAX_SLOTS;

    rs_big_bang();
    sample_slot_divide_and_computing_and_integrate(parent, children, tmp);
    rs_ultimate_fate_of_the_universe();

    free(_mem);

    return 0;
}
