#include "rsf.h"

/*****************************************************************************/
/* for logger and debug ******************************************************/
/*****************************************************************************/
static void _rsf_debug(const char *fmt, ...)
{
    #ifdef DEBUG
    va_list ap;

    va_start(ap, fmt);
    vlogger("rsf", DEBUG_, fmt, ap);
    va_end(ap);
    #endif
}

static void rsf_logger(const int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vlogger("rsf", level, fmt, ap);
    va_end(ap);
}

/* parts functions */

static void _rsf_write_header_of_kernel(rs_file_t *rsf,
                                         uint bits,
                                         int poly,
                                         uint division)
{
    /* never change info */
    /* because it's kernel info */
    /* reed solomon info */
    fprintf(rsf->header, "0x%x\n", bits);
    fprintf(rsf->header, "0x%x\n", poly);
    /* rs info */
    fprintf(rsf->header, "0x%x\n", division);
    /* about text size */
    fprintf(rsf->header, "0x%x\n", rsf->text_size);

    /* separate, one empty line */
    fprintf(rsf->header, "\n");
}

static int _rsf_read_header_of_kernel(
                 FILE *header,
                 uint *bits,
                 uint *poly,
                 uint *division,
                 size_t *text_size)
{
    int ret = 0;
    char ss[RSF_BUFFER_SIZE], *msg = NULL;

    /* fprintf(rsf->header, "0x%x\n", bits); */
    _fgets_(ss, RSF_BUFFER_SIZE, header);
    sscanf(ss, "0x%x", bits);
    /* fprintf(rsf->header, "0x%x\n", poly); */
    _fgets_(ss, RSF_BUFFER_SIZE, header);
    sscanf(ss, "0x%x", poly);
    /* fprintf(rsf->header, "0x%x\n", division); */
    _fgets_(ss, RSF_BUFFER_SIZE, header);
    sscanf(ss, "0x%x", division);
    /* fprintf(rsf->header, "0x%x\n", ssb->text_size); */
    _fgets_(ss, RSF_BUFFER_SIZE, header);
    sscanf(ss, "0x%x", text_size);

    /* fprintf(rsf->header, "\n"); */
    msg = fgets(ss, RSF_BUFFER_SIZE, header);
    if (msg != NULL && strcmp(ss, "\n") == 0) {
        /* OK */
    }
    else {
        rsf_logger(ERROR,
                   "failed to read new line code (='\\n'). "
                   "%s is invalid format.\n", ss);
        ret--;
    }

    return ret;
}

static int _rsf_read_header_of_hash(
                 char *header_hashed_name,
                 rs_file_t *rsf)
{
    char ss[RSF_BUFFER_SIZE];
    int ret = 0;

    /* fprintf(rsf->header, "sha1\n"); */
    _fgets_(ss, RSF_BUFFER_SIZE, rsf->header);
    if (strcmp("sha1", ss) == 0) {
        /* OK */
    }
    else {
        rsf_logger(ERROR,
                   "_rsf_read_header_of_hash() failed.\n"
                   "failed to read \"sha1\" %s is "
                   "invalid format.\n\n", ss);
        ret--;
    }
    /* fprintf(rsf->header, "%s\n", ss); */
    _fgets_(ss, RSF_BUFFER_SIZE, rsf->header);
    if (_ishashstring(ss, RSF_HASH_BIT_LENGTH)) {
        strcpy(header_hashed_name, ss);
    }
    else {
        rsf_logger(ERROR,
                   "_rsf_read_header_of_hash() failed.\n"
                   "failed to read hashed_name. "
                   "%s is invalid format.\n\n", ss);
        ret--;
    }

    return ret;
}

rs_file_t *rsf_construct(uint bits, uint division, int build_type)
{
    reed_solomon_t *rs = NULL;
    char *mem = NULL, *mem_ = NULL;
    size_t grown_size = 0;
    rs_file_t rsf_, *rsf = NULL;
    rs_encode_t rse_, *rse = NULL;
    rs_decode_t rsd_, *rsd = NULL;
    int ret;

    rsf_logger(DEBUG_, "in rsf_construct(bits=%u, division=%u, "
                        "build_type=%d).\n", bits, division, build_type);

    if (build_type != MODE_ENCODE && build_type != MODE_RECOVERY) {
        rsf_logger(ERROR, "in rsf_construct() UNKNOWN build_type = %d\n",
                            build_type);
        return NULL;
    }

    ret = _rs_take_rs(&rs, bits, division);
    if (ret < 0)
        return NULL;

    rsf = &rsf_;
    rse = &rse_;
    rsd = NULL;
    if (build_type == MODE_RECOVERY) {
        rsd = &rsd_;
    }

    mem = _rsf_allocate(rsf, rsd, rse, rs, division);

    if (mem == NULL) {
        rsf_logger(ERROR, "rsf_construct(bits=%u, division=%u, "
                           "build_type=%d) failed.\n",
                            bits, division, build_type);
        return NULL;
    }
    mem_ = mem;

    if (build_type == MODE_RECOVERY) {
        rsd = (rs_decode_t *)mem; mem += sizeof(rs_decode_t);
        _rs_calc_rsd_memory_size(rsd, rs, division);
        grown_size = _rs_init_rsd(rsd, mem, rs);
        mem += grown_size;
    }

    rse = (rs_encode_t *)mem; mem += sizeof(rs_encode_t);
    _rs_calc_rse_memory_size(rse, rs, division);
    grown_size = _rs_init_rse(rse, mem, rs);
    mem += grown_size;

    rsf = (rs_file_t *)mem; mem += sizeof(rs_file_t);
    rsf->mem = mem_;
    _rsf_calc_rsf_memory_size(rsf, division);
    grown_size = _rsf_init(rsf, mem, rsd, rse, build_type);
    mem += grown_size;

    #ifdef DEBUG
    _rsf_debug("grown_size = %u\n", grown_size);
    _rsf_debug("mem = %p\n", mem);

    _rs_view_rse(rse);
    _rsf_view_rsf(rsf);
    _rsf_debug("mem - mem_ is %zu\n", mem - mem_);
    #endif

    if (rsd != NULL)
        _rs_view_rsd(rsd);
    if (rse != NULL)
        _rs_view_rse(rse);
    if (rsf != NULL)
        _rsf_view_rsf(rsf);

    _rs_make_vandermonde(rse);

    _rsf_debug("\n\n");

    return rsf;
}

/* fopen() mode is "encode" or "decode" */
int rsf_decode_restored(char *restored_file_name, char *header)
{
    int ret = 0;
    rs_file_t *rsf = NULL;
    rs_decode_t *rsd = NULL;
    rs_encode_t *rse = NULL;
    reed_solomon_t *rs = NULL;
    _slot_size_brother_t *ssb = NULL;
    uint symbols_in_slot, symbols_in_buffer;
    uint remained_symbols, available_symbols;
    size_t r, w;

    restored_file_name[0] = '\0';

    ret = _rsf_get_up_rsf_for_restored(&rsf, header);
    if (ret < 0)
        return ret;
    _rsf_debug("in rsf_decode_restored()\n");
    _rsf_debug("rs_get_up_rsf_for_recover() = %d ok.\n\n", ret);

    rse = rsf->rse;
    rsd = rsf->rsd;
    rs = rsd->rs;
    ssb = rsf->ssb;

    /* rank error, row swap を試してみること。 */
    _rsf_merge_slots(rsd, rsf, rse);
    _rs_solve_inverse(rsd->inverse, rsd->merged, rsd);

    symbols_in_slot = rsf->symbols_in_slot;
    symbols_in_buffer = rsf->symbols_in_buffer;

    rsf_logger(ERROR, "in rsf_decode_restored().\n");

    if (symbols_in_slot < symbols_in_buffer) {
        available_symbols = symbols_in_slot;
    }
    else {
        available_symbols = symbols_in_buffer;
    }
    remained_symbols = symbols_in_slot;
    while (remained_symbols) {
        r = _fread_from_slots(rsf->merged,
                              rs->symbol_size,
                              available_symbols,
                              rsd->division);
        if (r != available_symbols) {
            rsf_logger(ERROR, "rsf_decode_restored() failed.\n");
            rsf_logger(ERROR, "cause of that _fread_from_slots()\n");
            ret = RSF_FREAD_ERROR;
            goto err_fread;
        }
        _rs_decode_slots(rsf, rsd, available_symbols);
        w = _fwrite_to_slots(rsf->recover,
                             rs->symbol_size, available_symbols,
                             rsd->division);
        if (w != available_symbols) {
            rsf_logger(ERROR, "rsf_decode_restored() failed.\n");
            rsf_logger(ERROR, "cause of that _fwrite_to_slots()\n");
            ret = RSF_FWRITE_ERROR;
            goto err_fwrite;
        }
        remained_symbols = _update_remained_symbols(
                                remained_symbols, &available_symbols);
    }

    ret = _rsf_recover_restored(rsf, symbols_in_slot, symbols_in_buffer);
    if (ret < 0) {
        rsf_logger(ERROR, "_rsf_recover_restored(symbols_in_slot=%u, "
                           "symbols_in_buffer=%u) failed.\n",
                            symbols_in_slot, symbols_in_buffer);
        goto err_recover;
    }

    fflush(rsf->restored);
    fseek(rsf->restored, 0L, SEEK_SET);
    sha1_as_file(rsf->sha1sum, rsf->restored);
    _to_hashed_name(restored_file_name, rsf->sha1sum);
    ret = RSF_SCUCCESS;

    _rsf_debug("in rsf_decode_restored()\n");
    _rsf_debug("rs_recover_file() done.\n\n");

err_recover:
err_fwrite:
err_fread:
    _rsf_good_night_rsf_for_recover(rsf, restored_file_name);

    return ret;
}

int _rsf_get_up_rsf_for_restored(rs_file_t **_rsf, char *header_path)
{
    char text_path[RSF_BUFFER_SIZE];
    char header_hashed_name[RSF_BUFFER_SIZE];
    int ret = 0;
    uint bits = 0, poly = 0, division = 0;
    uint len_available_slots = 0;
    FILE *header = NULL;
    size_t text_size = 0;
    rs_file_t *rsf = NULL;
    rs_decode_t *rsd = NULL;
    reed_solomon_t *rs = NULL;

    _rsf_debug("in _rsf_get_up_rsf_for_restored()\n");

    header = fopen(header_path, "r");
    _rsf_debug("fopen(header_path=\"%s\", \"r\")=%p\n", header_path, header);
    if (header == NULL) {
        rsf_logger(ERROR, "rs_get_up_rsf_for_recover() failed.\n");
        /* tekito */
        ret = -203;
        goto err_no_resource;
    }

    ret = _rsf_read_header_of_kernel(header,
                           &bits, &poly, &division, &text_size);
    if (ret < 0) {
        rsf_logger(ERROR, "rs_get_up_rsf_for_recover() failed.\n");
        rsf_logger(ERROR, "ret = %d, header = %p\n", ret, header);
        rsf_logger(ERROR, "bits=%u, poly=%u, division=%u, "
                           "text_size=%zu\n\n",
                            bits, poly, division, text_size);
        ret = -204;
        goto err_after_open_header;
    }
    _rsf_debug("_rsf_read_header_of_kernel() = %d ok.\n", ret);

    ret = _rsf_make_decoder(_rsf, bits, division);
    if (ret < 0) {
        rsf_logger(ERROR, "rs_get_up_rsf_for_recover() failed.\n");
        rsf_logger(ERROR, "ret = %d, header = %p\n", ret, header);
        rsf_logger(ERROR, "cannot make recoveryer. "
                           "bits=%u, poly=%u, division=%u, "
                           "text_size=%zu\n",
                            bits, poly, division, text_size);
        ret = -205;
        goto err_after_open_header;
    }
    _rsf_debug("_rsf_make_decoder() = %d ok.\n", ret);

    /* must be here to set *rsf->header after _rsf_make_decoder() */
    rsf = *_rsf;
    rsf->header = header;

    rsd = rsf->rsd;
    rs = rsd->rs;

    ret = _rsf_read_header_of_hash(header_hashed_name, rsf);
    if (ret < 0) {
        rsf_logger(ERROR, "_rsf_read_header_of_hash() failed.\n");
        rsf_logger(ERROR,
                   "bits=%u, poly=%u, division=%u, "
                   "text_size=%zu, hash=\"%s\"\n",
                    bits, poly, division, text_size, header_hashed_name);
        goto err_after_open_header;
    }
    else {
        _rsf_debug("_rsf_read_header_of_hash() = %d\n", ret);
    }
    _rsf_debug("text hashed name is\n");
    _rsf_debug("\"%s\"\n", header_hashed_name);

    strcpy(text_path, header_hashed_name);
    ret = _rsf_files_open(&len_available_slots, rsf, text_path, "wb+");
    if (ret < 0) {
        goto err_after_open_header;
    }
    if (len_available_slots < rsd->division) {
        rsf_logger(ERROR, "_rsf_files_open() failed.\n");
        rsf_logger(ERROR, "len_available_slots < rsd->division\n");
        rsf_logger(ERROR, "len_available_slots=%u\n", len_available_slots);
        rsf_logger(ERROR, "rsd->division=%u\n", rsd->division);
        ret = RSF_LACK_OF_SLOTS_ERROR;
        goto err_after_rsf_file_open;
    }

    /* BUG: 多分ここで死ぬ。だって、mergedって実態がないもの */
    ret = slot_calc_and_set_slot_size(rsf->merged,
                                    rs->symbol_size,
                                    text_size,
                                    division);
    ret = _rsf_calc_symbols(rsf, rs->symbol_size, rsf->slot_size, breath_size);
    if (ret < 0)
        goto err_after_rsf_file_open;

    return RSF_SCUCCESS;

    err_after_rsf_file_open:
        _rsf_files_close(rsf, rsd->division);
    err_after_open_header:
        fclose(rsf->header);
        rsf->header = NULL;
        _rsf_debug("fclose(header=%p) header_path=\"%s\"\n",
                          header,  header_path);
    err_no_resource:
    rsf_logger(ERROR, "header_path=\"%s\"\n", header_path);
    _rsf_debug("\n\n");

    return ret;
}

static int _rsf_good_night_rsf_for_recover(rs_file_t *rsf,
                                           char *recovery_file_name)
{
    rs_decode_t *rsd = rsf->rsd;

    fclose(rsf->header);
    _rsf_files_close(rsf, rsd->division);
    _rsf_free(rsf);
    _rsf_debug("in _rsf_good_night_rsf_for_recover()\n");
    _rsf_debug("closed recovery_file=\"%s\"\n\n", recovery_file_name);

    return RSF_SCUCCESS;
}

static uint _rsf_get_division(rs_file_t *rsf)
{
    uint division = 0;

    if (rsf->rsd != NULL)
        division = rsf->rsd->division;
    else if (rsf->rse != NULL)
        division = rsf->rse->division;
    else {
        rsf_logger(ERROR,  "_rsf_get_division() failed.\n"
                            "cause of that rsd and rse are NULL pointer.\n\n");
        return RSF_DIVISION_ERROR;
    }

    return division;
}

static void _rsf_view_rsf(rs_file_t *rsf)
{
    uint i, division = 0;
    char ss[RSF_BUFFER_SIZE];
    sha1sum_t sha1sum[1];

    division = _rsf_get_division(rsf);

    rsf_logger(INFO, "rsf = %p\n", rsf);
    rsf_logger(INFO, "                 mem = %p\n", rsf->mem);
    rsf_logger(INFO, "                 text = %p\n", rsf->text);
    rsf_logger(INFO, "             restored = %p\n", rsf->restored);
    _to_hashed_name(ss, sha1sum);
    rsf_logger(INFO, "      sha1sum = %p = %s\n", rsf->sha1sum, ss);
    rsf_logger(INFO, "\n");

    rsf_logger(INFO, "        text_size = %u\n", rsf->text_size);
    rsf_logger(INFO, "      symbols_in_slot = %zu\n", rsf->symbols_in_slot);
    rsf_logger(INFO, "    symbols_in_buffer = %zu\n", rsf->symbols_in_buffer);
    rsf_logger(INFO, "\n");

    rsf_logger(INFO, "              mode = %d is ", rsf->mode);
    if (rsf->mode == MODE_ENCODE)
        rsf_logger(INFO, "ENCODE");
    else if (rsf->mode == MODE_RECOVERY)
        rsf_logger(INFO, "RECOVERY");
    else
        rsf_logger(INFO, "UNKNOWN");
    rsf_logger(INFO, "         temp_path = %p, \"%s\"\n",
                          rsf->temp_path, rsf->temp_path);
    rsf_logger(INFO, "         temp_tail = %p, \"%s\"\n",
                          rsf->temp_tail, rsf->temp_tail);
    rsf_logger(INFO, "          dir_name = \"%s\"\n", rsf->dir_name);
    rsf_logger(INFO, "         base_name = \"%s\"\n", rsf->base_name);
    rsf_logger(INFO, "            header = %p\n", rsf->header);
    rsf_logger(INFO, "\n");

    rs_view_rse(rsf->rse);
    rs_view_rsd(rsf->rsd);
    for (i=0;i<division;i++)
        rsf_logger(INFO, "    norm[%5d].fp = %p\n", i, rsf->norm[i].fp);
    for (i=0;i<division;i++)
        rsf_logger(INFO, "  parity[%5d].fp = %p\n", i, rsf->parity[i].fp);
    for (i=0;i<division;i++)
        rsf_logger(INFO, "  merged[%5d].fp = %p\n", i, rsf->merged[i].fp);
    for (i=0;i<division;i++)
        rsf_logger(INFO, " recover[%5d].fp = %p\n", i, rsf->recover[i].fp);

    rsf_logger(INFO, "     allocate_size = %u\n", rsf->allocate_size);
    rsf_logger(INFO, "        slots_size = %u\n", rsf->slots_size);
    rsf_logger(INFO, "temp_path_max_size = %u\n", rsf->temp_path_max_size);
    rsf_logger(INFO, " dir_name_max_size = %u\n", rsf->dir_name_max_size);
    rsf_logger(INFO, "    base_name_size = %u\n", rsf->base_name_size);
    rsf_logger(INFO, "\n");
}

/* must set rsf->breath_size before below function call. */
static size_t _rsf_calc_rsf_memory_size(rs_file_t *rsf, uint division)
{
    size_t hex_name_size = 0;

    rsf->hash_size = RSF_HASH_SIZE;
    rsf_logger(FATAL, "rsf->breath_size = %u\n", rsf->breath_size);
    rsf->breath_size = RSF_BREATH_SIZE;
    /* buffer and slots need three type. there are norm, parity, merged */
    rsf->slots_size = division * sizeof(slot_t);
    rsf->temp_path_max_size = RSF_PATH_MAX_SIZE;

    rsf->hash_size = aligned_size(rsf->hash_size);
    rsf->breath_size = aligned_size(rsf->breath_size);
    rsf->slots_size = aligned_size(rsf->slots_size);
    rsf->temp_path_max_size = aligned_size(rsf->temp_path_max_size);

    /* SHA1("abc") is equal to "a9993e364706816aba3e25717850c26c9cd0d89d"  */
    /* hash_name = SHA1("abc") */
    hex_name_size = rsf->hash_size * 2;
    /* base_name = tmpnam() + "." + "0x04x" + '\0'  = L_tmpnam(=20) + 1 + 6 + 1
     *             or
     *             "h" + SHA1("abc") + '\0' = 1 + 40 + 1
     */
    rsf->base_name_size = 1 + hex_name_size + 1;
    rsf->base_name_size = aligned_size(rsf->base_name_size);
    /* temp_path = dir_name + "/" + base_name */
    /* therefore need to decrement dir_name_max_size for "/" */
    rsf->dir_name_max_size = rsf->temp_path_max_size - rsf->base_name_size - 1;
    rsf->dir_name_max_size = aligned_size(rsf->dir_name_max_size);

    rsf->allocate_size = sizeof(rs_file_t) +
                         rsf->slots_size * 4 +
                         rsf->breath_size * division * 4 +
                         rsf->temp_path_max_size +
                         rsf->base_name_size +
                         rsf->dir_name_max_size +
                         0; /* no mean */
    return rsf->allocate_size;
}

static size_t _rsf_init(rs_file_t *rsf,
                        char *mem,
                        rs_decode_t *rsd,
                        rs_encode_t *rse,
                        int mode)
{
    uint i, division = 0;
    char *mem_ = mem;

    rsf->rsd = rsd;
    rsf->rse = rse;

    division = _rsf_get_division(rsf);

    rsf->mode = mode;

    rsf->norm = (slot_t *)mem;     mem += rsf->slots_size;
    rsf->parity = (slot_t *)mem;   mem += rsf->slots_size;
    rsf->merged = (slot_t *)mem;   mem += rsf->slots_size;
    rsf->recover = (slot_t *)mem; mem += rsf->slots_size;

    rsf->temp_path = mem;         mem += rsf->temp_path_max_size;
    rsf->dir_name = mem;          mem += rsf->dir_name_max_size;
    rsf->base_name = mem;         mem += rsf->base_name_size;

    for (i=0;i<division;i++) {
        rsf->norm[i].slot = (uchar *)mem;
        mem += rsf->breath_size;
    }
    for (i=0;i<division;i++) {
        rsf->parity[i].slot = (uchar *)mem;
        mem += rsf->breath_size;
    }
    for (i=0;i<division;i++) {
        rsf->merged[i].slot = (uchar *)mem;
        mem += rsf->breath_size;
    }
    for (i=0;i<division;i++) {
        rsf->recover[i].slot = (uchar *)mem;
        mem += rsf->breath_size;
    }

    return (size_t )(mem - mem_);
}

static size_t _rsf_calc_memory_size(
                    rs_file_t *rsf,
                    rs_decode_t *rsd,
                    rs_encode_t *rse,
                    reed_solomon_t *rs,
                    uint division)
{
    size_t allocate_size = 0;

    rsf_logger(DEBUG_, "in _rsf_calc_memory_size().\n");
    if (rsf != NULL) {
        _rsf_calc_rsf_memory_size(rsf, division);
        allocate_size += rsf->allocate_size;
        rsf_logger(DEBUG_, "rsf->allocate_size = %u\n", rsf->allocate_size);
    }
    if (rsd != NULL) {
        _rs_calc_rsd_memory_size(rsd, rs, division);
        allocate_size += rsd->allocate_size;
        rsf_logger(DEBUG_, "rsd->allocate_size = %u\n", rsd->allocate_size);
    }
    if (rse != NULL) {
        _rs_calc_rse_memory_size(rse, rs, division);
        allocate_size += rse->allocate_size;
        rsf_logger(DEBUG_, "rse->allocate_size = %u\n", rse->allocate_size);
    }
    rsf_logger(DEBUG_, "allocate_size = %u\n", allocate_size);

    return allocate_size;
}

static char *_rsf_allocate(
                   rs_file_t *rsf,
                   rs_decode_t *rsd,
                   rs_encode_t *rse,
                   reed_solomon_t *rs,
                   uint division)
{
    size_t allocate_size = 0;
    char *mem = NULL;

    allocate_size = _rsf_calc_memory_size(rsf, rsd, rse, rs, division);

    mem = (char *)malloc(allocate_size);
    if (mem == NULL) {
        rsf_logger(ERROR, "_rsf_allocate() failed.\n");
        rsf_logger(ERROR, "mem = %p, malloc(allocate_size=%u)\n",
                           mem, allocate_size);
    }
    else {
        memset(mem, '\0', allocate_size);
        rsf_logger(INFO, "_rsf_allocate() success.\n");
        rsf_logger(INFO, "mem = %p, malloc(allocate_size=%u)\n",
                          mem, allocate_size);
    }

    return mem;
}

static int _rsf_make_encoder(rs_file_t **_rsf, uint bits, uint division)
{
    rs_file_t *rsf;

    rsf = rsf_construct(bits, division, MODE_ENCODE);
    if (rsf == NULL) {
        rsf_logger(ERROR, "rsf_construct(bits=%u, division=%u, build_type=MODE_ENCODE) failed.\n", bits, division);
        return RSF_CONSTRUCT_ERROR;
    }
    *_rsf = rsf;
    return RSF_SCUCCESS;
}

static int _rsf_make_decoder(rs_file_t **_rsf, uint bits, uint division)
{
    rs_file_t *rsf = NULL;

    _rsf_debug("in _rsf_make_decoder()\n");

    rsf = rsf_construct(bits, division, MODE_RECOVERY);
    if (rsf == NULL) {
        rsf_logger(ERROR, "rsf_construct(bits=%u, division=%u, build_type=MODE_RECOVERY) failed.\n", bits, division);
        return RSF_CONSTRUCT_ERROR;
    }

    *_rsf = rsf;
    return RSF_SCUCCESS;
}

static int _rsf_free(rs_file_t *rsf)
{
    rsf_logger(DEBUG_, "in _rsf_free(rsf=%p).\n", rsf);
    rsf_logger(DEBUG_, "free(rsf->mem=%p).\n\n", rsf->mem);
    free(rsf->mem);

    return RSF_SCUCCESS;
}

static int _rsf_get_index_of_slots(slot_t *hunter,
                                   slot_t target,
                                   uint division)
{
    uint i;
    int index = -1;

    for (i=0;i<division;i++) {
        if (target.slot == hunter[i].slot) {
            index = i;
            break;
        }
    }

    return index;
}

#if 0
static int _rsf_merge_slots(rs_decode_t *rsd,
                            rs_file_t *rsf,
                            rs_encode_t *rse)
{
    uint i, j = 0, i_index, j_index;
    reed_solomon_t *rs = rsd->rs;

    _rs_make_e_matrix(rsd->merged, rs, rsd->division);
    for (i=0;i<rse->division;i++) {
        if (rsf->norm[i].fp != NULL) {
            rsf->merged[i] = rsf->norm[i];
        }
        else {
            while (rsf->parity[j].fp == NULL)
                j++;

            i_index = i * rse->_row_size;
            j_index = j * rse->_row_size;
            memcpy(rsd->merged.ptr + i_index,
                   rse->vandermonde.ptr + j_index,
                   rse->_row_size);

            rsf->merged[i] = rsf->parity[j];
            j++;
        }
    }

   return RSF_SCUCCESS;
}
#endif

static void _to_hashed_name(char *ss, sha1sum_t *sha1sum)
{
    sprintf(ss, "h");
    sha1_get_hex(ss + 1, sha1sum);
}

static int _refine_base_name(char *temp_path)
{
    int i;
    char *last_sep = NULL;
    for (i=0;temp_path[i] != '\0';i++) {
        if (temp_path[i] == '/' ||
            temp_path[i] == '\\' )
            last_sep = temp_path + i;
    }
    if (last_sep)
        strcpy(temp_path, last_sep + 1);
    return RSF_SCUCCESS;
}

static char *_make_tmpnam(rs_file_t *rsf)
{
    char *p = NULL;

    p = tmpnam(rsf->temp_path);
    /*
    tempnam();
    mkstemp();
    tmpfile();
    */
    _refine_base_name(rsf->temp_path);
    rsf->temp_tail = rsf->temp_path + strlen(rsf->temp_path);
    return p;
}

static FILE *_fopen_rse(rs_file_t *rsf,
                        const char *mode,
                        int no)
{
    FILE *fp;
    if (rsf->temp_path[0] == '\0')
        _make_tmpnam(rsf);

    sprintf(rsf->temp_tail, ".0x%08x", no);
    fp = fopen(rsf->temp_path, mode);
    return fp;
}

/* header file にて、改行codeとするのは、'\n' == 0x0a のみ。
header fileのsha1sumの値をfile名に含める関係上、
改行codeについて明確にしなければならなかった。

'\r' == 0x0d は改行codeとしてheader file内に存在してはいけない。
header file 内に '\r' == 0x0d が含まれていたのならば、
そのheaderは不正なheader fileである。
*/
static char *_fgets_(char *s, int size, FILE *stream)
{
    int i;
    char *p = NULL;
    p = fgets(s, size, stream);
    for (i=0;p && i<size;i++) {
        if (s[i] == '\n') {
            s[i] = '\0';
            break;
        }
    }
    return p;
}

static uint _rsf_norm_files_open(
                 rs_file_t *rsf,
                 uint division)
{
    uint i, available = 0;
    char hashed_name[RSF_BUFFER_SIZE];
    for (i=0;i<division;i++) {
        if (rsf->mode == MODE_ENCODE)
            rsf->norm[i].fp = _fopen_rse(rsf, "wb+", i);
        else {
            /* rsf->mode == MODE_RECOVERY */
            _fgets_(hashed_name, RSF_BUFFER_SIZE, rsf->header);
            rsf->norm[i].fp = fopen(hashed_name, "rb");
            if (rsf->norm[i].fp == NULL) {
                rsf->recover[i].fp = fopen(hashed_name, "wb+");
            }
            else {
                rsf->recover[i].fp = NULL;
            }
        }
        if (rsf->norm[i].fp != NULL)
            available++;
    }
    return available;
}

static uint _rsf_parity_files_open(rs_file_t *rsf,
                                   uint division)
{
    uint i, available = 0;
    char hashed_name[RSF_BUFFER_SIZE];
    for (i=0;i<division;i++) {
        if (rsf->mode == MODE_ENCODE)
            rsf->parity[i].fp = _fopen_rse(rsf, "wb+", division + i);
        else {
            /* rsf->mode == MODE_RECOVERY */
            _fgets_(hashed_name, RSF_BUFFER_SIZE, rsf->header);
            rsf->parity[i].fp = fopen(hashed_name, "rb");
        }
        if (rsf->parity[i].fp != NULL)
            available++;
    }
    return available;
}

static int _rsf_files_open(uint *len_available_slots,
                           rs_file_t *rsf,
                           const char *text_path,
                           const char *mode)
{
    uint division = 0;
    uint len_norm_slots, len_parity_slots;

    _rsf_debug("in _rsf_files_open()\n");
    if (rsf->mode == MODE_ENCODE) {
        _rsf_debug("rsf->mode == MODE_ENCODE.\n");
    }
    else {
        _rsf_debug("rsf->mode == MODE_RECOVERY.\n");
    }

    division = _rsf_get_division(rsf);
    if (rsf->mode == MODE_ENCODE ) {
        rsf->text = fopen(text_path, mode);
        if (rsf->text == NULL) {
            rsf_logger(ERROR,
                       "_rsf_files_open() failed.\n"
                       "cannot open \"%s\" "
                       "to make rs file.\n", text_path);
            return RSF_INVALID_PATH_ERROR;
        }
    }
    else if (rsf->mode == MODE_RECOVERY) {
        rsf->restored = fopen(text_path, mode);
        _rsf_debug("rsf->restored = fopen(text_path=\"%s\", mode=\"%s\")\n",
                                            text_path, mode);
        _rsf_debug("rsf->restored = %p\n", rsf->restored);
        if (rsf->restored == NULL) {
            rsf_logger(ERROR, "_rsf_files_open() failed.\n");
            rsf_logger(ERROR, "cannot open \"%s\"\n", text_path);
            rsf_logger(ERROR, "to recover rs file.\n");
            return RSF_INVALID_PATH_ERROR;
        }
    }
    else {
        rsf_logger(ERROR, "_rsf_files_open() failed.\n");
        rsf_logger(ERROR, "rsf->mode value(=%d) is unknown.\n", rsf->mode);
        rsf_logger(ERROR, "\n");
        return RSF_INVALID_MODE_ERROR;
    }

    if (rsf->mode == MODE_ENCODE) {
        sha1_as_file(rsf->sha1sum, rsf->text);
        rsf->text_size = rsf->sha1sum->message_size;
        /* need to move file top position for making norm slot. */
        rewind(rsf->text);
    }

    len_norm_slots = _rsf_norm_files_open(rsf, division);
    len_parity_slots = _rsf_parity_files_open(rsf, division);
    *len_available_slots = len_norm_slots + len_parity_slots;

    _rsf_debug("\n\n");

    return RSF_SCUCCESS;
}

static void _rsf_files_close(rs_file_t *rsf, uint division)
{
    uint i;

    _rsf_debug("in _rsf_files_close()\n");
    for (i=0;i<division;i++) {
        if (rsf->norm[i].fp != NULL)
            fclose(rsf->norm[i].fp);
    }
    for (i=0;i<division;i++) {
        if (rsf->parity[i].fp != NULL)
            fclose(rsf->parity[i].fp);
    }
    /* no need to call fclose() for rsf->merged[i].fp.
     * rsf->mode == MODE_ENCODE then
     * no use rsf->merged[i].
     *
     * rsf->mode == MODE_RECOVERY then
     * since
     * rsf->merged[i] = rsf->norm[i];
     * or
     * rsf->merged[i] = rsf->parity[j];
     * in _rsf_merge_slots().
     */
    for (i=0;i<division;i++) {
        if (rsf->recover[i].fp != NULL)
            fclose(rsf->recover[i].fp);
    }

    if (rsf->text != NULL)
        fclose(rsf->text);
    if (rsf->restored != NULL)
        fclose(rsf->restored);

    _rsf_debug("\n\n");
}

static uint _update_remained_symbols(uint remained_symbols,
                                     uint *available_symbols)
{
    rsf_logger(ERROR, "in _update_remained_symbols()\n");
    _rsf_debug("in _update_remained_symbols()\n");
    _rsf_debug("remained_symbols = %u, *available_symbols = %u\n\n",
                 remained_symbols, *available_symbols);

    rsf_logger(ERROR,
               "remained_symbols = %u, *available_symbols = %u\n",
                remained_symbols, *available_symbols);

    remained_symbols -= *available_symbols;
    if (remained_symbols < *available_symbols) {
        *available_symbols = remained_symbols;
    }

    rsf_logger(ERROR,
               "remained_symbols = %u, *available_symbols = %u\n\n",
                remained_symbols, *available_symbols);

    return remained_symbols;
}

static int _rsf_write_header_of_hash(rs_file_t *rsf,
                                     uint division)
{
    char hashed_name[RSF_BUFFER_SIZE];
    uint i;

    /* hash part */
    /* maybe change below value if cracked hash function. */

    /* about text hashed function */
    fprintf(rsf->header, "sha1\n");

    /* about text hash value */
    _to_hashed_name(hashed_name, rsf->sha1sum);
    fprintf(rsf->header, "%s\n", hashed_name);

    /* about norm slot sha1sum */
    for (i=0;i<division;i++) {
        _rename_to_hashed_name(hashed_name, rsf, i);
        fprintf(rsf->header, "%s\n", hashed_name);
    }
    /* about parity slot sha1sum */
    for (i=0;i<division;i++) {
        _rename_to_hashed_name(hashed_name, rsf, division + i);
        fprintf(rsf->header, "%s\n", hashed_name);
    }

    return RSF_SCUCCESS;
}

/* fopen() mode is "encode" or "decode" */
int rsf_encode_text(char *hashed_header,
                    const char *path,
                    uint bits,
                    uint division)
{
    int ret;
    rs_file_t *rsf = NULL;
    rs_encode_t *rse = NULL;
    reed_solomon_t *rs = NULL;
    _slot_size_brother_t *ssb = NULL;
    uint len_available_slots = 0, available_symbols, remained_symbols;
    uint symbols_in_slot = 0, symbols_in_buffer = 0;
    size_t r = 0, w = 0;

    hashed_header[0] = '\0';

    ret = _rsf_make_encoder(&rsf, bits, division);
    if (ret < 0)
        return ret;

    rse = rsf->rse;
    rs = rse->rs;

    ret = _rsf_files_open(&len_available_slots, rsf, path, "rb");
    if (ret < 0) {
        _rsf_free(rsf);
        return ret;
    }

    ret = slot_calc_and_set_slot_size(rsf->norm,
                                    rs->symbol_size,
                                    rsf->text_size,
                                    division);
    ret = _rsf_calc_symbols(rsf,
                            rs->symbol_size,
                            ssb->slot_size,
                            rsf->breath_size);

    rsf->header = _fopen_rse(rsf, "wb", 2 * division);
    _rsf_write_header_of_kernel(rsf,
                                rs->bits, rs->poly, division);

    /* PETA(too large) size file and slots make too large size slot.
     * symbols_in_slot = TERA = PETA / number of slots
     * symbols_in_beffer = also from 1024 to 4096 octets.
     * then, symbols_in_slot >>>>>>> symbols_in_buffer
     */
    symbols_in_slot = rsf->symbols_in_slot;
    symbols_in_buffer = rsf->symbols_in_buffer;

    ret = _rsf_split_text(rsf, rs->symbol_size, division,
                          symbols_in_slot, symbols_in_buffer);
    if (ret < 0) {
        _rsf_free(rsf);
        return ret;
    }

    rsf_logger(WARN, "in _rs_encode_slots_file()\n");
    if (symbols_in_slot < symbols_in_buffer) {
        available_symbols = symbols_in_slot;
    }
    else {
        available_symbols = symbols_in_buffer;
    }
    remained_symbols = symbols_in_slot;
    /* slot.c に移植するつもり。*/
    while (remained_symbols) {
        r = _fread_from_slots(rsf->norm,
                              rs->symbol_size,
                              available_symbols,
                              division);
        rs_encode_slots(rsf->parity,
                        rsf->norm,
                        rse,
                        available_symbols);
        w = _fwrite_to_slots(rsf->parity,
                             rs->symbol_size,
                             available_symbols,
                             division);
        remained_symbols = _update_remained_symbols(remained_symbols,
                                                   &available_symbols);
    }

    _rsf_files_close(rsf, division);
    _rsf_write_header_of_hash(rsf, division);
    fclose(rsf->header);

    _rename_to_hashed_name(hashed_header, rsf, 2 * division);

    _rsf_free(rsf);

    return RSF_SCUCCESS;
}

int _rsf_calc_symbols(rs_file_t *rsf,
                      size_t symbol_size,
                      size_t slot_size,
                      size_t breath_size)
{
    rsf->symbols_in_slot = slot_size / symbol_size;
    rsf->symbols_in_buffer = breath_size / symbol_size;

    _rsf_debug("in _rsf_calc_symbols()\n");
    _rsf_debug("breath_size = %u\n", breath_size);
    _rsf_debug("slot_size = %u\n", slot_size);
    _rsf_debug("symbol_size = %u\n", symbol_size);
    _rsf_debug("symbols_in_slot = %u\n", rsf->symbols_in_slot);
    _rsf_debug("symbols_in_buffer = %u\n", rsf->symbols_in_buffer);
    _rsf_debug("\n");

    return RSF_SCUCCESS;
}

static int _rename_to_hashed_name(char *hashed_name,
                                  rs_file_t *rsf,
                                  int no)
{
    sha1sum_t sha1sum[1];
    FILE *fp;
    int ret;

    fp = _fopen_rse(rsf, "rb", no);
    if (fp == NULL) {
        rsf_logger(ERROR,
                   "_fopen_rse(%p, \"rb\", %d) = %p "
                   "in _rename_to_hashed_name()\n",
                    rsf, no, fp);
        return -1;
    }
    sha1_as_file(sha1sum, fp);
    fclose(fp);
    _to_hashed_name(hashed_name, sha1sum);
    ret = rename(rsf->temp_path, hashed_name);
    if (ret < 0) {
        rsf_logger(ERROR, "rename(\"%s\", \"%s\") failed.\n\n",
                            rsf->temp_path, hashed_name);
        perror(NULL);
    }
    else {
        _rsf_debug("in _rename_to_hashed_name()\n");
        _rsf_debug("rename(\"%s\", \"%s\")\n\n", rsf->temp_path, hashed_name);
    }
    return RSF_SCUCCESS;
}

/* ground functions */

/* int(math.log(gf_max, 10)) + 1 */
int _calc_log10(int n)
{
    int digits = 0;

    while (n > 0){
        n /= 10;
        digits++;
    }

    return digits;
}

static int _ishashstring(char *hashed_string, uint hash_len)
{
    int ch, ret = -1;
    size_t len = 0, i;

    /* hashed_string = "h" + SHA1() */
    len = strlen(hashed_string);
    if (len == 1 + hash_len / 8 * 2 && hashed_string[0] == 'h') {
        /* OK */

        for(i=1;i<len;i++) {
            ch = hashed_string[i];
            if ((ch >= '0' && ch <= '9') ||
                (ch >= 'a' && ch <= 'f'))
                /* OK */
                ;
            else {
                ret = 0;
                break;
            }
        }
        if (i == len)
            ret = 1;
    }
    else {
        ret = 0;
    }

    return ret;
}

