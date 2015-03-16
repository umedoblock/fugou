#include "slot.h"
#include "../reed_solomon/rs.h"

#define SS_SIZE (1024)

char *tmp_dir = ".";
const char *random_1048576_bin = "random_1048576.bin";
char msg[SS_SIZE];
char ss[SS_SIZE];
uchar *tmp;

typedef struct {
    reed_solomon_t *rs;
    matrix_t *vandermonde;
    vector_t *parity_vector;
    vector_t *data_vector;
    uint division;
    uint symbol_num;
} reed_solomon_encode_t;

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

        _rs_mul_matrix_vector16(rs, parity_vector, vandermonde, data_vector,
                                division);

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
                                     void *args)
{
    reed_solomon_encode_t *rse = args;
    return _rs_encode16_slots(parity, data,
                              rse->rs,
                              rse->vandermonde,
                              rse->parity_vector, rse->data_vector,
                              rse->division, rse->symbol_num);
}

void sample__slot_divide_and_computing_and_integrate(slot_t *parent, slot_t *children)
{
    /*
    1048576 % 41 =  1
   (1048576 + 122) / 41 = 25578.0, column_size = 123, division = 41
    */
    uint i, division = 41, symbol_size = 3;
    size_t parent_target_size = 0, child_slot_size, child_target_size;
    size_t zero_size;
    size_t integrate_target_size;
    int ret;
    uchar *expected_mem, *result_mem;
    FILE *fp;
    uchar *mem;
    uchar *dump_1048576;

    mem = (uchar *)malloc(1048576 * 2);

    tmp = mem; mem += 1048576;
    dump_1048576 = mem; mem += 1048576;

    _test_clean_parent_children(parent, children);
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, division);

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

    for (i=0;i<division;i++) {
        SLOT_name(children_i)[0] = '\0';
        /* slot_children_named() を実行する前に、
         * children->name が空文字列になるようにする。
         */
    }

    /* さあ親の名を取って、子に名付けてみよう。*/
    slot_children_named(SLF(children), parent, division);

    for (i=0;i<division;i++) {
        sprintf(ss, "%s%s%s.0x%08x", tmp_dir, SLOT_SEP, random_1048576_bin, i);
        sprintf(msg, "slot_children_named(children + %u)", i);
        /* 名前を空文字列にしておいたにも関わらず、
         * ssに表示した値と名前が一致する。
         * 名前付けすると、SLOT_type() も自動的に設定される。
         */
       assert_by_str(ss, SLOT_name(children_i), msg);
        sprintf(msg, "slot_children_named(), SLOT_type(children + %u)", i);
       assert_by_uint(SLOT_FILE, SLOT_type(children_i), msg);
    }

    slot_children_fopen(SLF(children), "wb+", division);
    for (i=0;i<division;i++) {
        sprintf(msg, "test__slot_divide_and_integrate() with "
                     "SLOT_target(children+%u)", i);
       assert_by_not_null(SLOT_target(children_i), msg);
    }

    slot_children_set_first_pos(parent, children, division);
    for (i=0;i<division;i++) {
        sprintf(msg, "slot_children_set_first_pos(parent=%p, children=%p, "
                     "division=%u) i=%u",
                      parent, children, division, i);
        assert_by_size(0, SLOT_index(children_i), msg);
    }

    /* parent => children の分割を行う。*/
    ret = _slot_divide(children, parent, division);
    sprintf(msg, "_slot_divide(children=%p, parent=%p, "
                 "division=%u, slot_fread, slot_fwrite)",
                  children, parent, division);
   assert_success(ret, msg);

    child_slot_size = SLOT_slot_size(children);
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
    ret = _slot_integrate(parent, children, division);
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
    slot_children_fclose(SLF(children), division);
    slot_children_remove(SLF(children), division);

    /* test 実行の度に _slot_divide_or_integrate(verb=SLOT_INTEGRATE)
     * の実行結果として integrate が作成されるから、integrate は毎回削除。
     */
    slot_file_fclose(SLF(parent));
    /*
    slot_file_remove(SLF(parent));
     */

}

int main(int argc, char *argv[])
{
    sample__slot_divide_and_computing_and_integrate(parent, children);

    return 0;
}
