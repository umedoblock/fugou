/* sample_slot_of_rs.c *******************************************************/
    /* in sample_slot_divide(slot_t *parent, slot_t *children, uchar *mem) */
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
/******************************************************** sample_slot_of_rs.c /
