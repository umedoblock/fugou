#include "libfugou.h"
#include "assert.h"
#include "slot.h"

#define TEST_MAX_SLOTS (257)

char msg[1024];
char ss[SS_SIZE];
uchar *dump_1048576;
uchar *tmp;
const char *tmp_dir = "../reed_solomon";
const char *random_1048576_bin = "random_1048576.bin";
char random_path[SS_SIZE]; /* tmp_dir/random_1048576.bin */

void assert_slot_size_brother(size_t target_size, size_t symbol_size,
                               uint division, size_t slot_size,
                               size_t column_size, size_t padding_size,
                               size_t buf_size, size_t index,
                               slot_t *slt, uint i,
                               char *name)
{
    sprintf(msg, "%s (slf + %u)->%s", name, i, "target_size");
    assert_by_size(target_size, SLOT_target_size(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "symbol_size");
    assert_by_size(symbol_size, SLOT_symbol_size(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "division");
    assert_by_size(division, SLOT_division(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "slot_size");
    assert_by_size(slot_size, SLOT_slot_size(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "column_size");
    assert_by_size(column_size, SLOT_column_size(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "padding_size");
    assert_by_size(padding_size, SLOT_padding_size(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "buf_size");
    assert_by_size(buf_size, SLOT_buf_size(slt + i), msg);
    sprintf(msg, "%s (slf + %u)->%s", name, i, "index");
    assert_by_size(index, SLOT_index(slt + i), msg);
}

void _test_clean_slot(slot_t *slt)
{
    memset(slt, '\0', slot_get_memory_size() * 1);
}

void _test_clean_parent_children(slot_t *parent, slot_t *children)
{
    memset(parent, '\0', slot_get_memory_size() * 1);
    memset(children, '\0', slot_get_memory_size() * TEST_MAX_SLOTS);
}

void test_slot_ask_target_size(slot_t *slt)
{
    _test_clean_slot(slt);

    slot_file_named(SLF(slt), tmp_dir, random_1048576_bin);
    slot_file_fopen(SLF(slt), "rb");
    assert_by_size(0, SLOT_target_size(slt),
                   "before slot_ask_target_size()");
    /* get a target_size */
    slot_ask_target_size(slt, FROM_HEAD);
    assert_by_size(1048576, SLOT_target_size(slt),
                   " after slot_ask_target_size()");

    /* 複数回連続で slot_ask_target_size() を実行しても大丈夫な事を確認。*/
    slot_ask_target_size(slt, FROM_HEAD);
    assert_by_size(1048576, SLOT_target_size(slt),
                   " after twice slot_ask_target_size()");

    /* 以下では必要なくなるので閉じておく。*/
    slot_file_fclose(SLF(slt));

    SLOT_type(slt) = SLOT_SOCKET;
    assert_by_size(0, slot_ask_target_size(slt, FROM_CURRENT),
                   "test_slot_ask_target_size() with SLOT_SOCKET");

    SLOT_type(slt) = SLOT_MEMORY;
    assert_by_size(0, slot_ask_target_size(slt, FROM_CURRENT),
                   "test_slot_ask_target_size() with SLOT_MEMORY");

}

void test_slot_set_memory(uchar *mem)
{
    slot_t *slt;

    _test_clean_slot(SLT(mem));

    slt = slot_set_memory(mem, 1);
    sprintf(msg, "test_slot_set_memory() SLOT_buf_size(slt=%p) = %zu\n",
                  slt, SLOT_buf_size(slt));
    assert_by_size(SLOT_BUF_SIZE, SLOT_buf_size(slt), msg);
}

/* 21d5139883ce722acefe05cb5ab3b0a870bf9b6b  reed_solomon/random_1048576.bin */
void test_slot_calc_sb_by_division(slot_t *parent, slot_t *children)
{
    /* symbol_size 3 の時のみ test する。他は面倒くさいし、
     * 一番 bug が出やすいのは、symbol_size=3の時でしょ？
     * それで十分。
     * 若干手抜き気分だけど、ここが一番重要な test !
     */
    uint i, j;
    uint divisions[] = {17, 32, 41, 0}, division;
    size_t symbol_size, parent_target_size;
    size_t expected_child_slot_size, expected_column_size;
    size_t expected_padding_size_[] = {35, 32, 122};
    size_t expected_parent_slot_size, expected_child_target_size;
    size_t expected_last_child_target_size, expected_padding_size;

    for (i=0;divisions[i] >= 2;i++) {
        division = divisions[i];

        /* clean *************************************************************/
        _test_clean_parent_children(parent, children);

        /* initialize start. *************************************************/
        /* reed_solomon/calc_slot.py
        1048576 % 17 = 16
        1048576 % 32 =  0
        1048576 % 41 =  1

       (1048576 +  35) / 17 = 61683.0, column_size =  51, division = 17
       (1048576 +  32) / 32 = 32769.0, column_size =  96, division = 32
       (1048576 + 122) / 41 = 25578.0, column_size = 123, division = 41
        */

        /* for mem */
        parent = slot_set_memory((uchar *)parent, 1);
        children = slot_set_memory((uchar *)children, division);

        /* for file */
        slot_file_named(SLF(parent), tmp_dir, random_1048576_bin);
        slot_file_fopen(SLF(parent), "rb");
        slot_ask_target_size(parent, FROM_HEAD);

        symbol_size = 3;
        parent_target_size = SLOT_target_size(parent);
        /* initialize done. **************************************************/

        /* 期待する結果を事前に計算。*****************************************/
        /* parent_slot_size は元々の大きさに、
         * padding_size 分増やした大きさになる。
         */
        expected_parent_slot_size = 1048576 + expected_padding_size_[i];
        /* column_size は全ての子供で一緒。*/
        expected_column_size = division * symbol_size;
        /* slot_size は全ての子供で一緒。*/
        expected_child_slot_size = expected_parent_slot_size / division;
        /* target_size は最後の子の直前までは一緒。*/
        expected_child_target_size = expected_child_slot_size;
        /* target_size は最後の子だけ異なる。padding_size 分、小さい。*/
        expected_last_child_target_size = \
            expected_child_slot_size - expected_padding_size_[i];

#if 0
fprintf(_f, "--\n");
fprintf(_f, "division = %u\n", division);
fprintf(_f, "expected_parent_slot_size = %u\n", expected_parent_slot_size);
fprintf(_f, "expected_column_size = %u\n", expected_column_size);
fprintf(_f, "expected_padding_size = %u\n", expected_padding_size);
fprintf(_f, "expected_child_slot_size = %u\n", expected_child_slot_size);
fprintf(_f, "expected_child_target_size = %u\n", expected_child_target_size);
fprintf(_f, "expected_last_child_target_size = %u\n", expected_last_child_target_size);
fprintf(_f, "\n");
#endif

        slot_calc_sb_by_division(parent, children,
                                 parent_target_size,
                                 division,
                                 symbol_size,
                                 0);

        assert_slot_size_brother(1048576, symbol_size,
                                  division, expected_parent_slot_size,
                                  expected_column_size,
                                  expected_padding_size_[i],
                                  SLOT_BUF_SIZE, 0,
                                  parent, 0,
                                 "test_slot_calc_sb_by_division() with parent");

        for(j=0;j<division;j++) {
            if (j < division - 1) {
                /* target_size は最後の子の直前までは一緒。*/
                /* padding_size は最後の子の直前までは、常に 0 */
                expected_child_target_size = expected_child_slot_size;
                expected_padding_size = 0;
            }
            else {
                /* target_size は最後の子だけ異なる。padding_size 分、小さい。*/
                expected_child_target_size = expected_last_child_target_size;
                expected_padding_size = expected_padding_size_[i];
            }

            /* slot_size は全ての子供で一緒。*/
            /* column_size は全ての子供で一緒。*/
            /* buf_size は全ての子供で一緒。*/
            assert_slot_size_brother(
                expected_child_target_size, symbol_size,
                division, expected_child_slot_size,
                expected_column_size, expected_padding_size,
                SLOT_BUF_SIZE, 0,
                children, j,
               "test_slot_calc_sb_by_division() "
               "with children"
            );
        }

        slot_file_fclose(SLF(parent));
    }
}

void test__slot_calc_division_by_child_slot_size()
{
    uint division;
    size_t append_size;

    _slot_calc_division_by_child_slot_size(&division, &append_size,
                                            1048576, 1024);
    assert_by_uint(1024, division, "test_slot_calc_division_"
                                    "by_slot_size() with division");
    assert_by_size(0, append_size, "test_slot_calc_append_size_"
                                     "by_slot_size() with append_size");

    _slot_calc_division_by_child_slot_size(&division, &append_size,
                                            1048576, 10000);
    assert_by_uint(105, division, "test_slot_calc_division_"
                                   "by_slot_size() with division");
    assert_by_size(1424, append_size, "test_slot_calc_append_size_"
                                       "by_slot_size() with append_size");

    _slot_calc_division_by_child_slot_size(&division, &append_size,
                                            4148, 1000);
    assert_by_uint(5, division, "test_slot_calc_division_"
                                 "by_slot_size() with division");
    assert_by_size(852, append_size, "test_slot_calc_append_size_"
                                      "by_slot_size() with append_size");

    _slot_calc_division_by_child_slot_size(&division, &append_size, 533, 100);
    assert_by_size(6, division, "test_slot_calc_division_"
                                 "by_slot_size() with division");
    assert_by_size(67, append_size, "test_slot_calc_append_size_"
                                     "by_slot_size() with append_size");

    _slot_calc_division_by_child_slot_size(&division, &append_size, 533, 99);
    assert_by_uint(6, division, "test_slot_calc_division_"
                                 "by_slot_size() with division");
    assert_by_size(61, append_size, "test_slot_calc_append_size_"
                                     "by_slot_size() with append_size");
}

void test__slot_divide_and_integrate(slot_t *parent, slot_t *children)
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

    _test_clean_parent_children(parent, children);
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, division);

    /* file to file で試してみる。
     * parent file を children file に分割。
     * children file を integrate file に統合。
     */
    SLOT_writing(parent) = slot_fwrite;
    SLOT_reading(parent) = slot_fread;
    for (i=0;i<division;i++) {
        SLOT_writing(children_i) = slot_fwrite;
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
    ret = _slot_divide(children, parent, division, NULL);
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
    ret = _slot_integrate(parent, children, division, NULL);
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

void test__slot_divide_and_integrate_mini_by_slot_size(
           slot_t *parent,
           slot_t *children)
{
    size_t expected_division, expected_column_size;
    size_t expected_parent_slot_size, expected_parent_padding_size;
    uint expected_index;
    uint i, division = 0, symbol_size = 0;
    size_t parent_target_size = 0, child_slot_size, child_target_size;
    size_t slot_buf_size, integrate_target_size, zero_size;
    int ret;
    uchar *expected_mem, *result_mem;
    FILE *fp;

    _test_clean_parent_children(parent, children);
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);

    slot_file_named(SLF(parent), tmp_dir, random_1048576_bin);
    slot_file_fopen(SLF(parent), "rb");

    /* auto calc and set in slot_calc_sb_by_child_slot_size() */
    expected_division = 6;
    expected_column_size = 18;
    expected_index = 0;
    expected_parent_slot_size = 594;
    expected_parent_padding_size = 61;
    expected_index = 0;

    parent_target_size = 533;
    child_slot_size = 99;
    symbol_size = 3; /* must be zero that child_slot_size mod symbol_size */

    sprintf(msg, "before slot_calc_sb_by_child_slot_size()");
    assert_by_uint(0, SLOT_division(parent), msg);
    ret = slot_calc_sb_by_child_slot_size(parent, children,
                                          parent_target_size,
                                          child_slot_size,
                                          symbol_size);
    sprintf(msg, "slot_calc_sb_by_child_slot_size(parent, children, "
                 "parent_target_size=%zu, child_slot_size=%zu, symbol_size=%u)",
                  parent_target_size, child_slot_size, symbol_size);
    assert_true(ret == SLOT_SUCCESS, msg);

    slot_buf_size = 10;
    SLOT_buf_size(parent) = slot_buf_size;
    for (i=0;i<TEST_MAX_SLOTS;i++) {
        SLOT_buf_size(children + i) = slot_buf_size;
    }
    assert_slot_size_brother(
        parent_target_size, symbol_size,
        expected_division, expected_parent_slot_size,
        expected_column_size, expected_parent_padding_size,
        slot_buf_size, expected_index,
        parent, expected_index,
       "test_slot_calc_sb_by_child_slot_size() "
       "with parent"
    );
    division = expected_division;

    /* file to file で試してみる。
     * parent file を children file に分割。
     * children file を mini file に統合。
     */

    SLOT_writing(parent) = slot_fwrite;
    SLOT_reading(parent) = slot_fread;
    for (i=0;i<division;i++) {
        SLOT_writing(children_i) = slot_fwrite;
        SLOT_reading(children_i) = slot_fread;
    }

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
        sprintf(msg, "test__slot_divide_and_integrate_mini_by_slot_size() "
                     "with SLOT_target(children+%u)", i);
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
    ret = _slot_divide(children, parent, division, NULL);
    sprintf(msg, "_slot_divide(children=%p, parent=%p, "
                 "division=%u, slot_fread, slot_fwrite)",
                  children, parent, division);
    assert_success(ret, msg);

    for (i=0;i<division;i++) {
        rewind(SLOT_target(children_i));
        fread(tmp, 1, child_slot_size,
                      SLOT_target_f(children_i));
        expected_mem = dump_1048576 + i * child_slot_size;
        result_mem = tmp;
        if (i < division - 1) {
            sprintf(msg, "test__slot_divide_and_integrate_mini_by_slot_size() "
                         "with SLOT_target(children+%u) == "
                         "dump_1048576 + i(=%u) * child_slot_size(=%zu)",
                          i, i, child_slot_size);
            assert_by_mem(expected_mem, result_mem,
                          child_slot_size, msg);
            sprintf(msg, "test__slot_divide_and_integrate_mini_by_slot_size() "
                         "with SLOT_target(children+%u), "
                         "SLOT_index(children+%u)", i, i);
            assert_by_size(SLOT_target_size(children_i),
                           SLOT_index(children_i),
                           msg);
        }
        else {
            /* 最終 slot, last slot */
            child_target_size = SLOT_target_size(children_i);
            zero_size = child_slot_size - child_target_size;
            sprintf(msg, "test__slot_divide_and_integrate_mini_by_slot_size() "
                         "with SLOT_padding_size(children_i), zero_size");
            assert_by_size(SLOT_padding_size(children_i), zero_size, msg);
            sprintf(msg, "test__slot_divide_and_integrate_mini_by_slot_size() "
                         "with assert_by_mem(expected_mem, result_mem)");
            assert_by_mem(expected_mem, result_mem,
                          child_target_size, msg);
           /* 簡易版 debug 関数
           sprintf(msg, "child_target_size=%u, zero_size=%u, "
                        "child_slot_size=%u, padding_size=%u\n",
                         child_target_size, zero_size,
                         child_slot_size, SLOT_padding_size(children_i));
            assert_true(0, msg);
           */
            sprintf(msg, "test__slot_divide_and_integrate_mini_by_slot_size() "
                         "with assert_by_00(result_mem=%p + "
                         "child_target_size=%zu, zero_size=%zu)",
                          result_mem, child_target_size, zero_size);
            assert_by_00(result_mem + child_target_size, zero_size, msg);
        }
    }

    /* children は使い回すので、file pointer を先頭に戻しておく。*/
    for (i=0;i<division;i++) {
        rewind(SLOT_target(children_i));
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

    /*
    _test_clean_parent_children(parent, children);
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);
    */

    /* children => parent の統合 */
    ret = _slot_integrate(parent, children, division, NULL);
    sprintf(msg, "_slot_divide_or_integrate(parent=%p, children=%p, "
                 "division=%u, slot_fread, slot_fwrite)",
                  parent, children, division);
    assert_success(ret, msg);

    integrate_target_size = slot_ask_target_size(parent, FROM_HEAD);
    sprintf(msg, "_slot_integrate(parent=%p, children=%p, "
                 "division=%u, slot_fread, slot_fwrite) "
                 "integrate_target_size=%zu",
                  parent, children, division, integrate_target_size);
    assert_by_size(parent_target_size, integrate_target_size, msg);

    rewind(SLOT_target(parent));
    fread(tmp, 1, parent_target_size, SLOT_target_f(parent));
    sprintf(msg, "_slot_divide_or_integrate(parent=%p, children=%p, "
                 "SLOT_INTEGRATE, ...", parent, children);
    assert_by_mem(dump_1048576, tmp, parent_target_size, msg);

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

void test_slot_file_named(slot_file_t *slf)
{
    memset(slf, '\0', slot_get_memory_size() * 1);

    slf = SLF(slot_set_memory((uchar *)slf, 1));
    slot_file_named(SLF(slf), tmp_dir, random_1048576_bin);

    sprintf(msg, "test_slot_file_named(slf=%p)", slf);
    assert_by_uint(SLOT_FILE, SLOT_type(slf), msg);

    memset(slf, '\0', slot_get_memory_size() * 1);
}

void test_slot_calc_sb_by_child_slot_size(slot_t *parent, slot_t *children)
{
    int ret = -1;
    size_t parent_target_size, child_slot_size, symbol_size;
    size_t expected_division, expected_column_size, expected_padding_size;
    size_t expected_parent_slot_size, expected_parent_padding_size;
    size_t expected_child_target_size, slot_buf_size;
    uint expected_index;
    uint i;

    /* first test ************************************************************/
    _test_clean_parent_children(parent, children);
    /* for mem */
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);

    /* auto calc and set in slot_calc_sb_by_child_slot_size() */
    expected_division = 6;
    expected_column_size = 6;
    expected_index = 0;
    expected_parent_slot_size = 600;
    expected_parent_padding_size = 67;
    expected_index = 0;

    /* three arguments for slot_calc_sb_by_child_slot_size() */
    parent_target_size = 533;
    child_slot_size = 100;
    symbol_size = 1; /* must be zero that child_slot_size mod symbol_size */

    ret = slot_calc_sb_by_child_slot_size(parent, children,
                                    parent_target_size,
                                    child_slot_size,
                                    symbol_size);
    sprintf(msg, "slot_calc_sb_by_child_slot_size(parent, children, "
                 "parent_target_size=%zu, child_slot_size=%zu, symbol_size=%zu)",
                  parent_target_size, child_slot_size, symbol_size);
    assert_true(ret == SLOT_SUCCESS, msg);

    assert_slot_size_brother(
        parent_target_size, symbol_size,
        expected_division, expected_parent_slot_size,
        expected_column_size, expected_parent_padding_size,
        SLOT_BUF_SIZE, expected_index,
        parent, expected_index,
       "test_slot_calc_sb_by_child_slot_size() "
       "with parent"
    );

    for (i=0;i<SLOT_division(children);i++) {
        if (i < SLOT_division(children) - 1) {
            /* auto calc and set in slot_calc_sb_by_child_slot_size() */
            expected_child_target_size = 100;
            expected_padding_size = 0;
        }
        else {
            /* auto calc and set in slot_calc_sb_by_child_slot_size() */
            expected_child_target_size = 33;
            expected_padding_size = expected_parent_padding_size;
        }

        assert_slot_size_brother(
            expected_child_target_size, symbol_size,
            expected_division, child_slot_size,
            expected_column_size, expected_padding_size,
            SLOT_BUF_SIZE, expected_index,
            children, i,
           "test_slot_calc_sb_by_child_slot_size() "
           "with child"
        );
    }

    /* second test ***********************************************************/
    _test_clean_parent_children(parent, children);
    /* for mem */
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);

    /* auto calc and set in slot_calc_sb_by_child_slot_size() */
    expected_division = 6;
    expected_column_size = 18;
    expected_index = 0;
    expected_parent_slot_size = 594;
    expected_parent_padding_size = 61;
    expected_index = 0;

    /* three arguments for slot_calc_sb_by_child_slot_size() */
    parent_target_size = 533;
    child_slot_size = 99;
    symbol_size = 3; /* must be zero that child_slot_size mod symbol_size */
    ret = slot_calc_sb_by_child_slot_size(parent, children,
                                          parent_target_size,
                                          child_slot_size,
                                          symbol_size);
    sprintf(msg, "slot_calc_sb_by_child_slot_size(parent, children, "
                 "parent_target_size=%zu, child_slot_size=%zu, symbol_size=%zu)",
                  parent_target_size, child_slot_size, symbol_size);
    assert_true(ret == SLOT_SUCCESS, msg);

    assert_slot_size_brother(
        parent_target_size, symbol_size,
        expected_division, expected_parent_slot_size,
        expected_column_size, expected_parent_padding_size,
        SLOT_BUF_SIZE, expected_index,
        parent, expected_index,
       "test_slot_calc_sb_by_child_slot_size() "
       "with parent"
    );

    for (i=0;i<SLOT_division(children);i++) {
        if (i < SLOT_division(children) - 1) {
            /* auto calc and set in slot_calc_sb_by_child_slot_size() */
            expected_child_target_size = 99;
            expected_padding_size = 0;
        }
        else {
            /* auto calc and set in slot_calc_sb_by_child_slot_size() */
            expected_child_target_size = 38;
            expected_padding_size = expected_parent_padding_size;
        }
        assert_slot_size_brother(
            expected_child_target_size, symbol_size,
            expected_division, child_slot_size,
            expected_column_size, expected_padding_size,
            SLOT_BUF_SIZE, expected_index,
            children, i,
           "test_slot_calc_sb_by_child_slot_size() "
           "with child"
        );
    }

    /* third test to fail ****************************************************/
    _test_clean_parent_children(parent, children);
    /* for mem */
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);

    child_slot_size = 98;
    symbol_size = 3; /* must be zero that child_slot_size mod symbol_size */
    ret = slot_calc_sb_by_child_slot_size(parent, children,
                                    parent_target_size,
                                    child_slot_size,
                                    symbol_size);
    sprintf(msg, "slot_calc_sb_by_child_slot_size(parent, children, "
                 "parent_target_size=%zu, child_slot_size=%zu, symbol_size=%zu)",
                  parent_target_size, child_slot_size, symbol_size);
    assert_true(ret == SLOT_SLOT_SIZE_ERROR, msg);

    /* fourth test ***********************************************************/
    _test_clean_parent_children(parent, children);
    /* for mem */
    parent = slot_set_memory((uchar *)parent, 1);
    children = slot_set_memory((uchar *)children, TEST_MAX_SLOTS);

    /* #299 絡みでtest する前の簡易実験。*/
    slot_buf_size = 10;
    SLOT_buf_size(parent) = slot_buf_size;
    for (i=0;i<TEST_MAX_SLOTS;i++) {
        SLOT_buf_size(children + i) = slot_buf_size;
    }

    /* auto calc and set in slot_calc_sb_by_child_slot_size() */
    expected_division = 6;
    expected_column_size = 18;
    expected_index = 0;
    expected_parent_slot_size = 594;
    expected_parent_padding_size = 61;
    expected_index = 0;

    /* three arguments for slot_calc_sb_by_child_slot_size() */
    parent_target_size = 533;
    child_slot_size = 99;
    symbol_size = 3; /* must be zero that child_slot_size mod symbol_size */
    ret = slot_calc_sb_by_child_slot_size(parent, children,
                                          parent_target_size,
                                          child_slot_size,
                                          symbol_size);
    sprintf(msg, "slot_calc_sb_by_child_slot_size(parent, children, "
                 "parent_target_size=%zu, child_slot_size=%zu, symbol_size=%zu)",
                  parent_target_size, child_slot_size, symbol_size);
    assert_true(ret == SLOT_SUCCESS, msg);

    assert_slot_size_brother(
        parent_target_size, symbol_size,
        expected_division, expected_parent_slot_size,
        expected_column_size, expected_parent_padding_size,
        slot_buf_size, expected_index,
        parent, expected_index,
       "test_slot_calc_sb_by_child_slot_size() "
       "with parent"
    );

    for (i=0;i<SLOT_division(children);i++) {
        if (i < SLOT_division(children) - 1) {
            /* auto calc and set in slot_calc_sb_by_child_slot_size() */
            expected_child_target_size = 99;
            expected_padding_size = 0;
        }
        else {
            /* auto calc and set in slot_calc_sb_by_child_slot_size() */
            expected_child_target_size = 38;
            expected_padding_size = expected_parent_padding_size;
        }
        assert_slot_size_brother(
            expected_child_target_size, symbol_size,
            expected_division, child_slot_size,
            expected_column_size, expected_padding_size,
            slot_buf_size, expected_index,
            children, i,
           "test_slot_calc_sb_by_child_slot_size() "
           "with child"
        );
    }

}

void test_slot(uchar *mem)
{
    slot_t *parent, *children;

    test__slot_calc_division_by_child_slot_size();

    parent = slot_set_memory(mem, 1);
        mem += slot_get_memory_size();
    children = slot_set_memory(mem, TEST_MAX_SLOTS);
        mem += slot_get_memory_size() * TEST_MAX_SLOTS;

#if 1
    test_slot_calc_sb_by_division(parent, children);
    test_slot_calc_sb_by_child_slot_size(parent, children);
#endif

#if 1
    test_slot_file_named(SLF(parent));
    test_slot_ask_target_size(parent);
    test_slot_set_memory((uchar *)parent);
    test_slot_set_memory((uchar *)children);
    test_slot_calc_sb_by_division(parent, children);

    test__slot_divide_and_integrate(parent, children);
#endif
    test__slot_divide_and_integrate_mini_by_slot_size(parent, children);
}

int main(void)
{
    size_t mem_size = 0;
    FILE *fp;
    uchar *mem = NULL, *mem_;

    set_logger(stderr);
    set_logger_level(DEBUG_);

    mem_size += 1048576 * 2;
    /* 1: parent, TEST_MAX_SLOTS: children */
    mem_size += (1 + TEST_MAX_SLOTS) * slot_get_memory_size();
    mem = (uchar *)malloc(mem_size);

    sprintf(random_path,
            SLOT_PARENT_FORMAT, tmp_dir, SLOT_SEP, random_1048576_bin);
    fp = fopen(random_path, "rb");
    if (fp == NULL || mem == NULL) {
        if (fp == NULL)
            fprintf(stderr, "cannot find random_path=\"%s\"\n", random_path);
        if (mem == NULL)
            fprintf(stderr, "cannot allocate 1048576 octets\n");
        exit(1);
    }
    mem_ = mem;
    memset(mem, '\0', mem_size);

    dump_1048576 = mem; mem += 1048576;
    fread(dump_1048576, sizeof(uchar), 1048576, fp);
    fclose(fp);

    tmp = mem; mem += 1048576;
    test_slot(mem);

    free(mem_);

    return 0;
}
