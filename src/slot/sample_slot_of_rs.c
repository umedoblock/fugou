#include "slot.h"
#include "../reed_solomon/rs.h"

#define SS_SIZE (1024)
#define TEST_MAX_SLOTS (257)

char *tmp_dir = ".";
const char *random_1048576_bin = "random_1048576.bin";
char msg[SS_SIZE];
char ss[SS_SIZE];

uchar *dump_1048576;
char random_path[SS_SIZE]; /* tmp_dir/random_1048576.bin */

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
                num += SLOT_buf(parity + j)[i * symbol_size + k];
            }
            VECTOR16(data_vector)[j] = num;
        }

        rs_mul_matrix_vector16(rs, parity_vector, vandermonde, data_vector);

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
                                     uint symbol_num,
                                     void *args)
{
    reed_solomon_encode_t *rse = args;
    return _rs_encode16_slots(parity, data,
                              rse->rs,
                              rse->vandermonde,
                              rse->parity_vector, rse->data_vector,
                              rse->division, symbol_num);
}

void sample_slot_divide(slot_t *parent, slot_t *children,
                        uint bits, uint division,
                        uchar *tmp)
{
    /*
    1048576 % 41 =  1
   (1048576 + 122) / 41 = 25578.0, column_size = 123, division = 41
    */
    uint symbol_num = 0;
    size_t integrate_target_size;
    size_t symbol_size;
    int ret;
    reed_solomon_encode_t _rse, *rse = &_rse;

    /* reed solomon の設定 */
    rs_set_rse(rse, bits, division, tmp);
    symbol_size = rse->rs->symbol_size;

    /* file to file で試してみる。
     * parent file を children file に分割。
     * children file を integrate file に統合。
    SLOT_writing(parent) = slot_fwrite;
     */
    slot_set(parent, slot_fread, slot_reed_solomon_encode, NULL);
    slot_set_ntimes(children, NULL, NULL, slot_fwrite, division);
        /*
        SLOT_computing(children_i) = slot_reed_solomon_recover;
        SLOT_reading(children_i) = slot_fread;
        */

    slot_birth(parent, children, division,
               symbol_size, SLOT_FILE,
               tmp_dir, random_1048576_bin);

    symbol_num = SLOT_slot_size(children) / symbol_size;

    /* parent => children の分割を行う。*/
    /* sample の目玉 */
    ret = _slot_divide(children, parent, division, symbol_num, (void *)rse);

    slot_children_fclose(SLF(children), division);

    slot_file_fclose(SLF(parent));
}

void sample_slot_integrate(slot_t *parent, slot_t *children,
                           uint bits, uint division,
                           uchar *tmp)
{
    size_t symbol_size;
    reed_solomon_encode_t _rse, *rse = &_rse;

    /* reed solomon の設定 */
    rs_set_rse(rse, bits, division, tmp);
    symbol_size = rse->rs->symbol_size;

    /* file to file で試してみる。
     * parent file を children file に分割。
     * children file を integrate file に統合。
    SLOT_writing(parent) = slot_fwrite;
     */
    slot_set(parent, NULL, slot_reed_solomon_encode, slot_fwrite);
    slot_set_ntimes(children, slot_fread, NULL, NULL, division);

#if 0
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
    /*
    slot_children_fclose(SLF(children), division);
    slot_children_remove(SLF(children), division);

    _slot_divide_or_integrate(verb=SLOT_INTEGRATE)
     * の実行結果として integrate が作成されるから、integrate は毎回削除。
    slot_file_fclose(SLF(parent));
     */
    /*
    slot_file_remove(SLF(parent));
     */

}

int main(int argc, char *argv[])
{
    size_t mem_size = 0;
    uint bits=16, division = 41;
    uchar *mem, *_mem, *tmp;
    slot_t *parent, *children;

    mem_size += 1048576 * 1;
    mem_size += (1 + TEST_MAX_SLOTS) * slot_get_memory_size();

    mem = (uchar *)malloc(mem_size);
    if (mem == NULL) {
        fprintf(stderr, "faild malloc(mem_size=%lu)\n", mem_size);
        return -1;
    }
    _mem = mem;
    memset(mem, '\0', mem_size);

    tmp = mem; mem += 1048576;

    parent = slot_set_memory(mem, 1);
    mem += slot_get_memory_size();
    children = slot_set_memory(mem, TEST_MAX_SLOTS);
    mem += slot_get_memory_size() * TEST_MAX_SLOTS;

    /* initialize reed solomon memory */
    rs_big_bang();

    sample_slot_divide(parent, children, bits, division, tmp);

    /* clean up memory */
    mem = _mem;
    memset(mem, '\0', mem_size);

    /* retry memory mapping */
    tmp = mem; mem += 1048576;
    parent = slot_set_memory(mem, 1);
    mem += slot_get_memory_size();
    children = slot_set_memory(mem, TEST_MAX_SLOTS);
    mem += slot_get_memory_size() * TEST_MAX_SLOTS;

    sample_slot_integrate(parent, children, bits, division, tmp);

    /* finalize reed solomon memory */
    rs_ultimate_fate_of_the_universe();

    /* free working area */
    free(_mem);

    return 0;
}
