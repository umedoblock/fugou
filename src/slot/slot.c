#include "slot.h"

/* proto type ******************************************************/
size_t _update_remained_size(size_t remained_size, size_t *available_size);
int slot_seek(slot_t *slt, size_t seek_size, int whence);
int slot_getpos(slot_t *slt, _pos_t *pos);

/*****************************************************************************/
/* for logger and debug ******************************************************/
/*****************************************************************************/
static void _slot_view_sb(_slot_size_brother_t *sb)
{
    LOGGER(INFO, "sb = %p\n", sb);
    LOGGER(INFO, "      padding_size = %zu\n", sb->padding_size);
    LOGGER(INFO, "         slot_size = %zu\n", sb->slot_size);
    LOGGER(INFO, "         buf_size = %zu\n", sb->buf_size);
    LOGGER(INFO, "         column_size = %zu\n", sb->column_size);
    LOGGER(INFO, "         target_size = %zu\n", sb->target_size);
    LOGGER(INFO, "\n");
}

static void slot_view_slt(slot_t *slt)
{
    LOGGER(INFO, "slt = %p\n", slt);
    LOGGER(INFO, "              buf = %p\n", SLOT_buf(slt));
    LOGGER(INFO, "            target = %p\n", SLOT_target(slt));
    LOGGER(INFO, "               sb = %p\n", SLOT_sb(slt));
    _slot_view_sb(SLOT_sb(slt));
}

static void slot_view_slts(slot_t *slts, uint division)
{
    uint i;

    for (i=0;i<division;i++) {
        slot_view_slt(slts + i);
    }
}

size_t _calc_snip_size(size_t num, size_t modulus)
{
    size_t snip_size;

    snip_size = num % modulus;
    _DEBUG("_calc_snip_size(num=%u, modulus=%u, snip_size=%u)\n",
                 num, modulus, snip_size);

    return snip_size;
}

size_t _calc_padding_size(size_t num, size_t modulus)
{
    size_t padding_size, snip_size;

    snip_size = _calc_snip_size(num, modulus);
    if (snip_size == 0)
        padding_size = 0;
    else
        padding_size = modulus - snip_size;

    _DEBUG("_calc_padding_size(num=%u, modulus=%u, "
                "snip_size=%u, padding_size=%u)\n",
                 num, modulus, snip_size, padding_size);

    return padding_size;
}

size_t _update_remained_size(size_t remained_size, size_t *available_size)
{
_DEBUG("start _update_remained_size().\n");

   _DEBUG("remained_size = %u, *available_size = %u\n",
                remained_size, *available_size);

    remained_size -= *available_size;
    if (remained_size < *available_size) {
        *available_size = remained_size;
    }

   _DEBUG("remained_size = %u, *available_size = %u\n",
                remained_size, *available_size);

_DEBUG(" done _update_remained_size().\n");
    return remained_size;
}

int slot_file_named(slot_file_t *file,
                    const char *dir_name,
                    const char *base_name)
{
_DEBUG("start slot_file_named().\n");

    sprintf(SLOT_name(file),
            SLOT_PARENT_FORMAT,
            dir_name,
            SLOT_SEP,
            base_name);
    SLOT_type(file) = SLOT_FILE;

_DEBUG(" done slot_file_named().\n");
    return SLOT_SUCCESS;
}

int slot_children_named(slot_file_t *children,
                        slot_t *parent,
                        uint division)
{
    uint i;

_DEBUG("start slot_children_named().\n");
    for (i=0;i<division;i++) {
        sprintf(SLOT_name(children_i),
                SLOT_CHILDLEN_FORMAT,
                SLOT_name(parent),
                i);
        SLOT_type(children_i) = SLOT_FILE;
    }

_DEBUG(" done slot_children_named().\n");
    return SLOT_SUCCESS;
}

int slot_file_fopen(slot_file_t *file,
                    const char *mode)
{
    FILE *target;
    int ret = SLOT_SUCCESS;

    target = fopen(SLOT_name(file), mode);
_DEBUG("slot_file_fopen(file=%p,target=%p,name=\"%s\",mode=\"%s\")\n",
             file, target, SLOT_name(file), mode);
    SLOT_target(file) = target;
    if (target == NULL) {
        ret = SLOT_FOPEN_ERROR;
        LOGGER(ERROR,
                   "slot_file_fopen(file=%p,target=%p,"
                   "name=\"%s\",mode=\"%s\")\n",
                    file, target, SLOT_name(file), mode);
    }

    return ret;
}

int slot_children_fopen(slot_file_t *children,
                        const char *mode,
                        uint division)
{
    uint i;
    int ret = SLOT_SUCCESS, ret2;

_DEBUG("start slot_children_fopen().\n");
    for (i=0;i<division;i++) {
        ret2 = slot_file_fopen(children_i, mode);
        if (ret2 != SLOT_SUCCESS)
            ret = ret2;

        /* fopen() では、以下の理屈が成り立たないと思うけど、
         * rs.c との兼ね合いで、parity なんかの事を考えるとどうしようかな。
         * 数えられるようにすればいいか。
         */
    }

_DEBUG(" done slot_children_fopen().\n");
    return ret;
}

int slot_file_fclose(slot_file_t *file)
{
    int ret2, ret = SLOT_SUCCESS;
    FILE *target;

    target = SLOT_target(file);
    if (target == NULL) {
        ret = SLOT_FCLOSE_ERROR;
        LOGGER(ERROR, "cannot fclose(SLOT_target(%p)=NULL)\n", file);
    }
    else {
        ret2 = fclose(target);
        if (ret2 == EOF) {
            ret = SLOT_FCLOSE_ERROR;
            LOGGER(ERROR, "fclose()=%d, SLOT_target(%p)=%p\n",
                        ret2, file, target);
        }
    }

    return ret;
}

int slot_children_fclose(slot_file_t *children,
                         uint division)
{
    uint i;
    int ret2, ret = SLOT_SUCCESS;
    FILE *target;

_DEBUG("start slot_children_fclose().\n");
    for (i=0;i<division;i++) {
        target = SLOT_target(children_i);
        ret2 = slot_file_fclose(children_i);
        if (ret != SLOT_SUCCESS) {
            ret = SLOT_FCLOSE_ERROR;
            LOGGER(ERROR, "slot_file_fclose()=%d, SLOT_target(%p + "
                               "%u)=%p\n", ret2, children, i, target);
        }
        /* fclose(), remove() の場合は、あるchild一人失敗した場合に、
         * 後続の children も失敗するとは限らないので、
         * 後続の children にfclose(), remove() を実行する必要がある。
         * よって、ここで ret != SLOT_SUCCESS を検知可能であるが、
         * 検知しようともしないし、break しない。
         */
    }

_DEBUG(" done slot_children_fclose().\n");
    return ret;
}

int slot_file_remove(slot_file_t *file)
{
    int ret;

    ret = remove(SLOT_name(file));

    return ret;
}

int slot_children_remove(slot_file_t *children,
                         uint division)
{
    uint i;
    int ret2, ret = SLOT_SUCCESS;
    FILE *target;

_DEBUG("start slot_children_remove().\n");
    for (i=0;i<division;i++) {
        ret2 = slot_file_remove(children_i);
        if (ret2 == -1) {
            target = SLOT_target(children_i);
            LOGGER(ERROR, "remove()=%d, SLOT_target(children + "
                               "%u)=\"%s\"=%p\n", ret2, i,
                                SLOT_name(children_i), target);
            ret = SLOT_REMOVE_ERROR;
        }
        /* fclose(), remove() の場合は、あるchild一人失敗した場合に、
         * 後続の children も失敗するとは限らないので、
         * 後続の children にfclose(), remove() を実行する必要がある。
         * よって、ここで ret != SLOT_SUCCESS を検知可能であるが、
         * 検知しようともしないし、break しない。
         */
    }

_DEBUG(" done slot_children_remove().\n");
    return ret;
}

int slot_children_set_first_pos(slot_t *parent,
                                slot_t *children,
                                uint division)
{
    size_t slot_size;
    _pos_t *pos;
    int i;
    slot_t *child;

_DEBUG("start slot_children_first_setpos().\n");

    slot_size = SLOT_slot_size(children);
    for (i=0;i<division;i++) {
        child = children_i;
        SLOT_index(child) = 0;
        pos = &SLOT_pos(child);
        slot_getpos(parent, pos);
_DEBUG("slot_getpos(parent=%p, pos=%p) i=%u\n", parent, pos, i);

        slot_seek(parent, slot_size * (i + 1), SEEK_SET);
_DEBUG("slot_seek(parent=%p, slot_size=%d * (i=%u + 1), SEEK_SET).\n", parent, slot_size, i);
    }
    slot_seek(parent, 0L, SEEK_SET);

_DEBUG(" done slot_children_first_setpos().\n");
    return SLOT_SUCCESS;
}

size_t slot_get_memory_size(void)
{
    return sizeof(slot_t) + SLOT_BUF_SIZE;
}

/* TODO: 親と子のmemory領域共有について
 * 親が一度に全ての data を読み込んでしまいたい。
 * 子は，親の読んだ data のaddressを使うようにしたい。

 * なので，親から見た子のbufは一連である必要がある。

 * そのため，memの先頭にはslot_t構造体のmemory領域が並ぶ。
 * その後，子のbufが途切れることなく連続してnum個分並ぶ。
 * 親から見れば，子のbufは一つの大きな領域(buf*num)に見える。
 * 親と子は，memory領域を共有する。

 * ということを，お迎えの時に思いついた。
 * 思い出して書いておいた。

 * 注意1:
 * 親が読み込める領域の大きさと，
 * target_size の大きさの比較結果によって，
 * 処理を変える必要が出てくる。
 * 注意2:
 * 以下のTODOによる高速化を用いる場合，
 * 子のmemory領域のoffsetを調整する必要がある。
 */

/* TODO: 最速の2bytes endian変換(rs16 で必要)
 * 一度，test program を書いて，速度検証をする必要がある。

 * xy0123456789abcdef....
   separate each 2 bytes
   xy 01 23 45 67 89 ab cd ef
    . . . . . . . . .

 * case ordinary
 * swap 0 and 1, every step 2.
 * 0123456789abcdef...
 * 1032547698badcfe...
   for (i=0;i<n;i+=2){
     // 3 steps
     tmp = buf[i];
     buf[i] = buf[i+1]
     buf[i+1] = tmp;
   }

 * case smart
 * mv 0 to y and 1 to x, every step 2.
 * xy0123456789abcdef...
 * 1032547698badcfe...
   10 32 54 76 98 ba dc fe
   for (i=0;i<n;i+=2){
     // 2 steps
     buf[i] = buf[i+3];
     buf[i+1] = buf[i+2];
   }

 * case excellent
 * the fastest !!!
 * but bad alignment...
   対策としては，1回目は先頭1byteのみ読み込んで，
   2回めで残り全て読み込むとか？
   とにかく，一つ分ずらす必要がある。
 * mv 1 to y, every step 2
 * xy0123456789abcdef...
     / / / / / / / /
 * x1032547698badcfe...
    10 32 54 76 98 ba dc fe
   for (i=0;i<n;i+=2){
     // 1 step !!!!!!! no prural [s] !!!
     buf[i+1] = buf[i+3];
   }
 */
/* TODO: 4 bytes endian converter
 * こちらも検証する必要がある。
 * xyzw0123456789abcdef...
   x32107654ba98fedc
   for (i=0;i<n;i+=4){
     buf[i+1] = buf[i+7];
     buf[i+2] = buf[i+6];
     buf[i+3] = buf[i+5];
     // 3 steps, not smart
   }

 * xyzw0123456789abcdef...
      1032547698badcfe
   for (i=0;i<n;i+=2){
     buf[i+3] = buf[i+5];
   }
      1032547698badcfe
   x32107654ba98fedc
   buf++;
   buf16 = buf;
   for (i=0;i<n/4;i+=2){
     buf16[i] = buf16[i+2];
   }
   // 2 steps, so smart
 */

slot_t *slot_set_memory(uchar *mem, int num)
{
    slot_t *slt;
    int i;

    slt = SLT(mem); mem += num * sizeof(slot_t);
    for (i=0;i<num;i++) {
        SLOT_buf_size(slt + i) = SLOT_BUF_SIZE;
        SLOT_buf(slt + i) = mem; mem += SLOT_BUF_SIZE;
    }
    return slt;
}

size_t slot_ask_target_size(slot_t *slt, int whence)
{
    FILE *fp;
    fpos_t fixed_pos;
    long marker, tail;
    size_t ret = 0;

_DEBUG("start slot_ask_target_size().\n");
    if ((SLOT_type(slt) == SLOT_FILE) &&
        (whence == FROM_HEAD || whence == FROM_CURRENT)) {
        fp = SLOT_target(slt);

        /* 現在地を調べて記録する。*/
        fgetpos(fp, &fixed_pos);

        if (whence == FROM_HEAD) {
        /* 頭から〜 */
        fseek(fp, 0L, SEEK_SET);
        marker = ftell(fp);
        }
        else if (whence == FROM_CURRENT) {
        /* 現在地から〜 */
        marker = ftell(fp);
        }

        /* お尻まで */
        fseek(fp, 0L, SEEK_END);
        tail = ftell(fp);

        /* 移動して移動距離を調べた後、*/
        ret = (size_t )(tail - marker);

        /* 元いた場所に戻る。*/
        fsetpos(fp, &fixed_pos);
    }
    else if (SLOT_type(slt) == SLOT_SOCKET) {
        /* pass */
    }
    else if (SLOT_type(slt) == SLOT_MEMORY) {
        /* pass */
    }
    else {
        LOGGER(ERROR, "SLOT_type(slt=%p) is %d, "
                      "whence is %d\n",
                       slt, SLOT_type(slt), whence);
    }

    SLOT_target_size(slt) = ret;
_DEBUG(" done slot_ask_target_size().\n");
    return ret;
}

int _slot_calc_sb_children_by_division(slot_t *children,
                                        size_t target_size,
                                        size_t slot_size,
                                        size_t symbol_size,
                                        size_t column_size,
                                        size_t padding_size,
                                        uint division)
{
    size_t padding_size_ = 0;
    uint d;

    for (d=0;d<division;d++) {
        /* target_size -= padding_size when last slot, slots[division-1].
         * division must be 2 or more.
         */
        if (d != division - 1)
            padding_size_ = 0;
        else
            padding_size_ = padding_size;

        target_size = slot_size - padding_size_;

        SLOT_target_size(children + d) = target_size;
        SLOT_slot_size(children + d) = slot_size;
        SLOT_symbol_size(children + d) = symbol_size;
        SLOT_column_size(children + d) = column_size;
        SLOT_padding_size(children + d) = padding_size_;
        SLOT_division(children + d) = division;
    }

    _DEBUG("in slot_calc_children_by_division(children=%p)\n", children);
    _DEBUG("target_size = %u\n", target_size);
    _DEBUG("symbol_size = %u\n", symbol_size);
    _DEBUG("slot_size = %u\n", slot_size);
    _DEBUG("column_size = %u\n", column_size);
    _DEBUG("padding_size = %u\n", padding_size);
    _DEBUG("buf_size = %u\n", SLOT_buf_size(children));
    _DEBUG("padding_size_ = %u\n", padding_size_);
    _DEBUG("division = %u\n", division);
    _DEBUG("\n");

    return SLOT_SUCCESS;
}

int slot_calc_sb_by_division(slot_t *parent,
                             slot_t *children,
                             size_t parent_target_size,
                             uint division,
                             size_t symbol_size,
                             size_t append_size)
{
    size_t column_size = division * symbol_size, padding_size;
    size_t parent_slot_size, child_slot_size;;
    int ret = SLOT_SUCCESS;

    if (append_size > 0) {
        padding_size = append_size;
    } else {
        padding_size = _calc_padding_size(parent_target_size, column_size);
    }

    column_size = division * symbol_size;
    parent_slot_size = parent_target_size + padding_size;
    if (parent_slot_size < parent_target_size  ||
        parent_slot_size < padding_size ) {
        LOGGER(ERROR,
                 "slot_calc_sb_by_division() failed.\n"
                 "overflowed parent_slot_size(=%u) for\n"
                 "adding parent_target_size(=%u), append_size(=%u), "
                 "padding_size(=%u).\n",
                  parent_slot_size, parent_target_size, append_size,
                  padding_size);
        LOGGER(ERROR, "\n");
        ret = SLOT_slot_size_ERROR;
    }
    if (ret == SLOT_SUCCESS) {
        SLOT_target_size(parent) = parent_target_size;
        SLOT_slot_size(parent) = parent_slot_size;
        SLOT_column_size(parent) = column_size;
        SLOT_padding_size(parent) = padding_size;
        SLOT_division(parent) = division;
        SLOT_symbol_size(parent) = symbol_size;
        child_slot_size = parent_slot_size / division;

        _slot_calc_sb_children_by_division(children,
                                           parent_target_size,
                                           child_slot_size,
                                           symbol_size,
                                           column_size,
                                           padding_size,
                                           division);
    }

    return ret;
}

void _slot_calc_division_by_child_slot_size(uint *division,
                                      size_t *append_size,
                                      size_t parent_target_size,
                                      size_t child_slot_size)
{
    *append_size = _calc_padding_size(parent_target_size, child_slot_size);
    *division = parent_target_size / child_slot_size;
    if (*append_size)
        (*division)++;
}

size_t _round_up(size_t num, size_t round)
{
    return ((num / round) + 1) * round;
}

int slot_calc_sb_by_child_slot_size(slot_t *parent,
                              slot_t *children,
                              size_t parent_target_size,
                              size_t child_slot_size,
                              size_t symbol_size)
{
    int ret = SLOT_SUCCESS;
    size_t append_size;
    uint division;

    if (child_slot_size % symbol_size) {
        return SLOT_SLOT_SIZE_ERROR;
    }
    _slot_calc_division_by_child_slot_size(&division,
                                     &append_size,
                                      parent_target_size,
                                      child_slot_size);
    ret = slot_calc_sb_by_division(parent,
                                   children,
                                   parent_target_size,
                                   division,
                                   symbol_size,
                                   append_size);

    return ret;
}

size_t _fread_from_slots(slot_file_t *slf,
                                size_t available_size,
                                uint division)
{
    size_t r = 0;
    uint i;
    FILE *target = NULL;
    void *buf = NULL;

_DEBUG("start _fread_from_slots().\n");
    for (i=0;i<division;i++){
        target = SLOT_target(slf + i);
        buf = SLOT_buf(slf + i);
        r = slot_fread(SLT(slf) + i, buf, available_size);

        if (r != available_size) {
            LOGGER(ERROR, "_fread_from_slots() failed.\n"
                             "cause of fread().\n"
                             "i = %u, buf = %p, sizeof(uchar)=%u, target = %p\n"
                             "r(=%u) != available_size(=%u)\n\n",
                              i, buf, sizeof(uchar), target,
                              r, available_size);
            break;
        }
    }

_DEBUG(" done _fread_from_slots().\n");
    return r;
}

int _slot_realize_case(size_t index, size_t target_size, size_t available_size)
{
    /* n: slot_size, t: target_size, z: zero_size
     * a: available_size, i: index
     *        |nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn...|
     *        |ttttttttttttttttttttttttzzzzzzzzzzzzzzzzzzz...|
     * case 0 |iiiiiiiiiiaaaaaaaaaa
     * case 1 |iiiiiiiiiiiiiiiiiiiiaaaaaaaaaa
     * case 2 |iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiaaaaaaaaaa
     */
    int case_ = -1;

    if (index <= target_size - available_size) {
        /* index + available_size <= target_size
         * (index + available_size) maybe overflow,
         * so use substract to compare.
         */
        case_ = 0;
    }
    else if (index > target_size) {
        case_ = 2;
    }
    else {
        /* add index and available_size greater than target_size
         * but index smaller than target_size
         */
        case_ = 1;
    }

    return case_;
}

int _slot_adjust_available_size_by_case(size_t *read_size,
                                        size_t *zero_size,
                                        size_t index,
                                        size_t target_size,
                                        size_t available_size,
                                        int case_)
{
    /* available_size == read_size + zero_size */

    /* index + available_size <= target_size */
    switch(case_) {
    case 0:
        /* add index and available_size smaller than target_size */
        *read_size = available_size;
        *zero_size = 0;
        break;
    case 1:
        /* 法のお話で、index + *available_size の時に
         * over flow しても、別にイッカーって感じ。
         *
         * add index and available_size greater than target_size
         * but index smaller than target_size
         */
        *read_size = target_size - index;
        *zero_size = (index + available_size) - target_size;
        break;
    case 2:
        /* index greater than target_size */
        *read_size = 0;
        *zero_size = available_size;
        break;
    }

_DEBUG("_slot_adjust_available_size_by_case(case_=%d)\n"
            "index = %u, target_size = %u, available_size = %u\n"
            "*read_size=%u, *zero_size = %u\n",
             case_, index, target_size, available_size,
             *read_size, *zero_size);

    return SLOT_SUCCESS;
}

size_t _slot_divide_now_reading(slot_t *children,
                               slot_t *parent,
                               uint division,
                               size_t available_size)
{
    size_t r = 0, read_size, zero_size;
    uint i;
    uchar *child_buf = NULL;
    slot_t *child;
    int case_;

_DEBUG("start _slot_divide_now_reading(available_size=%u, "
            "division=%u).\n", available_size, division);
    for (i=0;i<division;i++){
        r = 0;
        read_size = available_size;
        zero_size = 0;

        child = children_i;
        child_buf = SLOT_buf(child);

        case_ = _slot_realize_case(SLOT_index(child),
                                   SLOT_target_size(child),
                                   available_size);
        _slot_adjust_available_size_by_case(&read_size,
                                            &zero_size,
                                             SLOT_index(child),
                                             SLOT_target_size(child),
                                             available_size,
                                             case_);

        if (read_size > 0) {
            slot_setpos(parent, &SLOT_pos(child));
            r += SLOT_reading(parent)(parent, child_buf, read_size);
            slot_getpos(parent, &SLOT_pos(child));
        }
        if (zero_size > 0) {
            slot_dev_null_r(parent, child_buf + read_size, zero_size);
            r += zero_size;
        }

        SLOT_index(child) += r;
        if (available_size != r) {
            LOGGER(ERROR, "_slot_divide_now_reading() failed.\n"
                               "i=%u, child_buf=%p, sizeof(uchar)=%u, "
                               "parent = %p, parent_target=%p\n"
                               "r(=%u) != available_size(=%u)\n"
                               "read_size=%u, zero_size=%u",
                                i, child_buf, sizeof(uchar),
                                parent, SLOT_target(parent),
                                r, available_size,
                                read_size, zero_size);
            break;
        }
    }

_DEBUG(" done _slot_divide_now_reading().\n");
    return r;
}

size_t _slot_divide_now_computing(slot_t *children,
                                  slot_t *parent,
                                  uint division,
                                  void *args)
{
    size_t compute_size = 0;

_DEBUG("start _slot_divide_now_computing(children=%p, parent=%p, "
            "args=%p), SLOT_computing(parent)=%p.\n",
             children, parent, args, SLOT_computing(parent));
    if (SLOT_computing(parent)) {
        compute_size = SLOT_computing(parent)(children, parent, division, args);
    }
    else {
        #if 0
        LOGGER(ERROR, "SLOT_computing(parent) is NULL pointer "
                           "in _slot_divide_now_computing(children=%p, "
                           "parent=%p, args=%p).\n",
                           children, parent, args);
        #endif
        compute_size = 0; /* BUG? compute_size should be ... */
    }

_DEBUG(" done _slot_divide_now_computing(), compute_size=%u\n",
              compute_size);
    return compute_size;
}

size_t _slot_divide_now_writing(slot_t *children,
                                slot_t *parent,
                                uint division,
                                size_t available_size)
{
    size_t w = 0;
    uint i;
    uchar *child_buf = NULL;
    slot_t *child;

_DEBUG("start _slot_divide_now_writing(available_size=%u, division=%u).\n", available_size, division);
    for (i=0;i<division;i++){
        child = children_i;
        child_buf = SLOT_buf(child);

        w = SLOT_writing(child)(child, child_buf, available_size);
        if (available_size != w) {
            LOGGER(ERROR, "_slot_divide_now_writing() failed.\n"
                               "i=%u, child_buf=%p, sizeof(uchar)=%u, "
                               "child = %p, child_target = %p\n"
                               "w(=%u) != available_size(=%u)\n",
                                i, child_buf, sizeof(uchar),
                                child, SLOT_target(child),
                                w, available_size);
            break;
        }
    }

_DEBUG(" done _slot_divide_now_writing().\n");
    return w;
}

size_t _slot_integrate_now_reading(slot_t *parent,
                                   slot_t *children,
                                   uint division,
                                   size_t available_size)
{
    size_t r = 0;
    uint i;
    uchar *child_buf = NULL;
    slot_t *child;

_DEBUG("start _slot_integrate_now_reading(available_size=%u, division=%u).\n", available_size, division);
    for (i=0;i<division;i++){
        child = children_i;
        child_buf = SLOT_buf(child);

        r = SLOT_reading(child)(child, child_buf, available_size);
        if (available_size != r) {
            LOGGER(ERROR, "_slot_integrate_now_reading() failed.\n"
                               "i = %u, child_buf = %p, sizeof(uchar)=%u, "
                               "child = %p, child_target = %p\n"
                               "r(=%u) != available_size(=%u)\n",
                                i, child_buf, sizeof(uchar),
                                child, SLOT_target(child),
                                r, available_size);
            break;
        }
    }

_DEBUG(" done _slot_integrate_now_reading().\n");
    return r;
}

#if 0
size_t _cat_mem(void *arg, uchar *buf, size_t available_size)
{
    size_t i;
    hist_t *h = (hist_t *)arg;

    for (i=0;i<available_size;i++) {
        buf[i] ^= 0xff;
        h->values[buf[i]]++;
    }

    return i;
}
#endif

size_t _slot_integrate_now_computing(slot_t *parent,
                                     slot_t *children,
                                     uint division,
                                     void *args)
{
    size_t compute_size = 0;
    slot_t *child;
    int i;

    for (i=0;i<division;i++) {
        child = children_i;
        if (SLOT_computing(child)) {
            /* 以下は、bug の可能性高し。適当にしている。*/
            SLOT_computing(child)(parent, children, division, args);
            _DEBUG("start _slot_integrate_now_computing(children=%p, "
                   "parent=%p, args=%p), SLOT_computing(parent)=%p.\n",
                         children, parent, args, SLOT_computing(parent));
        }
    }


_DEBUG(" done _slot_integrate_now_computing(), compute_size=%u\n",
              compute_size);
    return compute_size;
}

size_t _slot_integrate_now_writing(slot_t *parent,
                                  slot_t *children,
                                  uint division,
                                  size_t available_size)
{
    size_t w = 0, write_size, zero_size;
    uint i;
    uchar *child_buf = NULL;
    slot_t *child;
    int case_;

_DEBUG("start _slot_integrate_now_writing(available_size=%u, "
            "division=%u).\n", available_size, division);
    for (i=0;i<division;i++) {
        w = 0;
        write_size = available_size;
        zero_size = 0;

        child = children_i;
        child_buf = SLOT_buf(child);

        case_ = _slot_realize_case(SLOT_index(child),
                                   SLOT_target_size(child),
                                   available_size);
        _slot_adjust_available_size_by_case(&write_size,
                                            &zero_size,
                                             SLOT_index(child),
                                             SLOT_target_size(child),
                                             available_size,
                                             case_);

        if (write_size > 0) {
            slot_setpos(parent, &SLOT_pos(child));
            w += SLOT_writing(parent)(parent, child_buf, write_size);
            slot_getpos(parent, &SLOT_pos(child));
        }
        if (zero_size > 0) {
            slot_dev_null_w(child, child_buf + write_size, zero_size);
            w += zero_size;
        }

        SLOT_index(child) += w;
        if (available_size != w) {
            LOGGER(ERROR, "_slot_integrate_now_writing() failed.\n"
                               "i=%u, child_buf=%p, sizeof(uchar)=%u, "
                               "parent = %p, parent_target=%p\n"
                               "w(=%u) != available_size(=%u)\n"
                               "write_size=%u, zero_size=%u",
                                i, child_buf, sizeof(uchar),
                                parent, SLOT_target(parent),
                                w, available_size,
                                write_size, zero_size);
            break;
        }
    }

_DEBUG(" done _slot_integrate_now_writing().\n");
    return w;
}

size_t _slot_determine_doing_size(slot_t *slt)
{
    size_t doing_size, slot_size, buf_size;

    slot_size = SLOT_slot_size(slt);
    buf_size = SLOT_buf_size(slt);

    if (slot_size < buf_size) {
        doing_size = slot_size;
    }
    else {
        doing_size = buf_size;
    }
    doing_size -= _calc_snip_size(doing_size, SLOT_symbol_size(slt));

    return doing_size;
}

int _slot_divide(slot_t *children, slot_t *parent, uint division, void *args)
{
    size_t child_slot_size, parent_slot_size, consumed_size, doing_size;
    size_t child_remained_size;
    int ret = SLOT_SUCCESS;
    slot_t *child;

    /* こっちは分割 parent => children */
_DEBUG("start _slot_divide().\n");

    parent_slot_size = SLOT_slot_size(parent);
    child = children;
    child_slot_size = SLOT_slot_size(child);
    child_remained_size = child_slot_size;
    consumed_size = 0;

    doing_size = _slot_determine_doing_size(children + 0);

    _DEBUG("child_slot_size = %u.\n", child_slot_size);
    _DEBUG("parent_slot_size = %u.\n", parent_slot_size);
    /*********************************************************/
    while (consumed_size < parent_slot_size) {
       _DEBUG("consumed_size=%u < parent_slot_size=%u, doing_size=%u\n",
                    consumed_size, parent_slot_size, doing_size);

       /*********************************************************/
       _slot_divide_now_reading(children, parent, division, doing_size);
       _slot_divide_now_computing(children, parent, division, args);
       _slot_divide_now_writing(children, parent, division, doing_size);

       consumed_size += doing_size * division;
       child_remained_size = \
            _update_remained_size(child_remained_size, &doing_size);
    }
    _DEBUG("consumed_size=%u >= parent_slot_size=%u.\n",
                 consumed_size, parent_slot_size);

_DEBUG(" done _slot_divide().\n");
    return ret;
}

int _slot_integrate(slot_t *parent, slot_t *children, uint division, void *args)
{
    size_t child_slot_size, parent_slot_size, consumed_size, doing_size;
    size_t child_remained_size;
    int ret = SLOT_SUCCESS;
    slot_t *child;

    /* こっちは統合 children => parent */
_DEBUG("start _slot_integrate().\n");

    parent_slot_size = SLOT_slot_size(parent);
    child = children;
    child_slot_size = SLOT_slot_size(child);
    child_remained_size = child_slot_size;
    consumed_size = 0;

    doing_size = _slot_determine_doing_size(children + 0);

    _DEBUG("child_slot_size = %u.\n", child_slot_size);
    _DEBUG("parent_slot_size = %u.\n", parent_slot_size);
    /*********************************************************/
    while (consumed_size < parent_slot_size) {
       _DEBUG("consumed_size=%u < parent_slot_size=%u, doing_size=%u\n",
                    consumed_size, parent_slot_size, doing_size);

       /*********************************************************/
       _slot_integrate_now_reading(parent, children, division, doing_size);
       _slot_integrate_now_computing(parent, children, division, args);
       _slot_integrate_now_writing(parent, children, division, doing_size);

       consumed_size += doing_size * division;
       child_remained_size = \
            _update_remained_size(child_remained_size, &doing_size);
    }
    _DEBUG("consumed_size=%u >= parent_slot_size=%u.\n",
                 consumed_size, parent_slot_size);

_DEBUG(" done _slot_integrate().\n");
    return ret;
}

size_t slot_dev_null_r(slot_t *read_, void *buf, size_t read_size)
{
_DEBUG("slot_dev_null_r(read_=%p, buf=%p, read_size=%d)\n",
                             read_, buf, read_size);
    memset(buf, '\0', read_size);
    return read_size;
}

size_t slot_dev_null_w(slot_t *writ, void *buf, size_t writ_size)
{
    return writ_size;
}

size_t slot_fread(slot_t *read_, void *buf, size_t read_size)
{
    size_t r;
    char errmsg[SS_SIZE];
    int errnum;
    r = fread(buf, sizeof(uchar), read_size, (FILE *)SLOT_target(read_));
    if (r != read_size)
        LOGGED_ERRORNO();
    return r;
}

size_t slot_fwrite(slot_t *writ, void *buf, size_t writ_size)
{
    size_t w;
    char errmsg[SS_SIZE];
    int errnum;
    w = fwrite(buf, sizeof(uchar), writ_size, (FILE *)SLOT_target(writ));
    if (w != writ_size)
        LOGGED_ERRORNO();
    return w;
}

int slot_fgetpos(slot_t *slt, _pos_t *pos)
{
    char errmsg[SS_SIZE];
    int errnum;
    if (fgetpos(SLOT_target(slt), pos->f) == -1) {
        LOGGED_ERRORNO();
        return SLOT_FGETPOS_ERROR;
    }
    return SLOT_SUCCESS;
}

int slot_fsetpos(slot_t *slt, _pos_t *pos)
{
    char errmsg[SS_SIZE];
    int errnum;
    if (fsetpos(SLOT_target(slt), pos->f) == -1) {
        LOGGED_ERRORNO();
        return SLOT_FSETPOS_ERROR;
    }
    return SLOT_SUCCESS;
}

int slot_getpos(slot_t *slt, _pos_t *pos)
{
    int ret = SLOT_SUCCESS;

    if (SLOT_type(slt) == SLOT_FILE) {
        ret = slot_fgetpos(slt, pos);
    }
    else {
        ret = SLOT_UNKNOWN_TYPE_ERROR;
    }

    return ret;
}

int slot_fseek(slot_t *slt, size_t seek_size, int whence)
{
    char errmsg[SS_SIZE];
    int errnum;
    if (fseek(SLOT_target(slt), seek_size, whence) == -1) {
        LOGGED_ERRORNO();
        return SLOT_FSEEK_ERROR;
    }
    return SLOT_SUCCESS;
}

int slot_seek(slot_t *slt, size_t seek_size, int whence)
{
    int ret = SLOT_SUCCESS;

    if (SLOT_type(slt) == SLOT_FILE) {
        ret = slot_fseek(slt, seek_size, whence);
    }
    else {
        ret = SLOT_UNKNOWN_TYPE_ERROR;
    }

    return ret;
}

int slot_setpos(slot_t *slt, _pos_t *pos)
{
    int ret = SLOT_SUCCESS;

    if (SLOT_type(slt) == SLOT_FILE) {
        ret = slot_fsetpos(slt, pos);
    }
    else {
        ret = SLOT_UNKNOWN_TYPE_ERROR;
        LOGGER(ERROR, "slot_setpos() failed.\n"
                           "SLOT_type(slt) = %d\n",
                            SLOT_type(slt));
    }

if (ret != SLOT_SUCCESS)
_DEBUG("slot_setpos() debug.\n"
            "ret=%d\n", ret);

    return ret;
}

size_t slot_read(slot_t *read_, void *buf, size_t read_size)
{
    size_t r, total = 0;
    char errmsg[SS_SIZE];
    int errnum;

    while (total < read_size) {
        r = read(SLOT_target_fd(read_), buf, read_size - total);
        if (r == -1) {
            LOGGED_ERRORNO();
            break;
        }
        total += r;
    }
    return total;
}

size_t slot_recv(slot_t *read_, void *buf, size_t read_size)
{
    size_t r, total = 0;
    char errmsg[SS_SIZE];
    int errnum;

    while (total < read_size) {
        r = recv(SLOT_target_fd(read_), buf, read_size - total, 0);
        if (r == -1) {
            LOGGED_ERRORNO();
            break;
        }
        total += r;
    }
    return total;
}

size_t slot_write(slot_t *writ, void *buf, size_t writ_size)
{
    size_t w, total = 0;
    char errmsg[SS_SIZE];
    int errnum;

    while (total < writ_size) {
        w = write(SLOT_target_fd(writ), buf, writ_size - total);
        if (w == -1) {
            LOGGED_ERRORNO();
            break;
        }
        total += w;
    }
    return total;
}

/*
SEND(2)                    Linux Programmer's Manual                   SEND(2)

en: The only difference between send() and write(2) is the presence  of  flags.
ja: send() と write(2) の 違いは、引き数に flags があるかどうかだけである。

ssize_t send(int sockfd, const void *buf, size_t len, int flags);

size_t slot_iterator(void *iterator,
                     slot_t *target, void *buf, size_t target_size)
{
}
*/

size_t slot_send(slot_t *writ, void *buf, size_t writ_size)
{
    size_t w, total = 0;
    char errmsg[SS_SIZE];
    int errnum;

    while (total < writ_size) {
        w = send(SLOT_target_fd(writ), buf, writ_size - total, 0);
        if (w == -1) {
            LOGGED_ERRORNO();
            break;
        }
        total += w;
    }
    return total;
}

/* slot_memcpy_r() では、memcpy() と、
 * dst, src の位置関係が逆になっている事に注意。
 * fread(), read() との一貫性を持たせるために、
 * 逆にせざるを得なかった。
 */
size_t slot_memcpy_r(slot_t *read_, void *buf, size_t read_size)
{
    if (SLOT_buf(read_) != buf)
        memcpy(buf, SLOT_buf(read_), read_size);
    return read_size;
}

/* slot_memcpy_w() では、memcpy() と、
 * dst, src の位置関係が同じになっている事に注意。
 * slot_memcpy_r() と逆で書いている自分が混乱してくるっつーのっ！
 */
size_t slot_memcpy_w(slot_t *writ, void *buf, size_t writ_size)
{
    if (SLOT_buf(writ) != buf)
        memcpy(SLOT_buf(writ), buf, writ_size);
    return writ_size;
}

#define HETAKUSO (-1)
static int _slot_get_index_of_slots(slot_t *hunter,
                                    slot_t *capture,
                                    uint division)
{
    uint i;
    int index = HETAKUSO;

    for (i=0;i<division;i++) {
        if (SLOT_buf(capture) == SLOT_buf(hunter + i)) {
            index = i;
            break;
        }
    }

    return index;
}

static int _slot_merge_slots(slot_t *merged,
                             slot_t *norm,
                             slot_t *parity,
                             uint division)
{
    uint i, j = 0;
    int ret = SLOT_SUCCESS;

_DEBUG("start _slot_merge_slots().\n");
    if (merged == NULL ||
        norm == NULL ||
        parity == NULL ||
        division == 0) {
        LOGGER(ERROR, "failed in _slot_merge_slots().\n");
        LOGGER(ERROR, "because\n");
        if (merged == NULL)
            LOGGER(ERROR, "merged is NULL.\n");
        if (norm == NULL)
            LOGGER(ERROR, "norm is NULL.\n");
        if (parity == NULL)
            LOGGER(ERROR, "parity is NULL.\n");
        if (division == 0)
            LOGGER(ERROR, "division is 0.\n");
        ret = SLOT_NULL_ERROR;
    }
    /*
    reed_solomon_t *rs = rsd->rs;
    _rs_make_e_matrix(rsd->merged, rs, rsd->division);
    */

    for (i=0,j=0;i<division && ret == SLOT_SUCCESS;i++) {
        if (SLOT_target(norm + i) != NULL) {
            merged[i] = norm[i];
        }
        else {
            while (SLOT_target(parity + j) == NULL) {
                j++;
                if (j >= division) {
                    ret = SLOT_OUT_OF_RANGE_ERROR;
                    break;
                }
            }
            if (ret == SLOT_OUT_OF_RANGE_ERROR)
                break;

            /*
            uint i, j = 0, i_index, j_index;
            i_index = i * rse->_row_size;
            j_index = j * rse->_row_size;
            memcpy(rsd->merged.ptr + i_index,
                   rse->vandermonde.ptr + j_index,
                   rse->_row_size);
            */

            merged[i] = parity[j];
            j++;
        }
    }

_DEBUG(" done _slot_merge_slotsj=%d().\n", j);
   return ret;
}

