
static void _rsf_encode(rs_file_t *rsf,
                         rs_encode_t *rse,
                         uint symbol_num)
{
    if (rse->rs->register_size == 2)
        _rsf_encode16(rsf, rse, symbol_num);
    else
        _rsf_encode32(rsf, rse, symbol_num);
}

/* _rsf_solve_inverse(rsd) */
static void _rsf_decode(rs_file_t *rsf,
                         rs_decode_t *rsd,
                         uint symbol_num)
{
    if (rsd->rs->register_size == 2)
        _rsf_decode16(rsf, rsd, symbol_num);
    else
        _rsf_decode32(rsf, rsd, symbol_num);
}

static void _rsf_encode16(rs_file_t *rsf,
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
    _ptr_t vector = rse->_row;
    _ptr_t parity = rse->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += rsf->norm[j].slot[i * symbol_size + k];
            }
            vector.u16[j] = num;
        }

        _rs_mul_matrix_vector16(rs, parity, vandermonde, vector, \
                                 division);

        for (j=0;j<division;j++) {
            num = parity.u16[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                rsf->parity[j].slot[i * symbol_size + k] = unum;
            }
        }
    }
}

static void _rsf_encode32(rs_file_t *rsf,
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
    _ptr_t vector = rse->_row;
    _ptr_t parity = rse->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += rsf->norm[j].slot[i * symbol_size + k];
            }
            vector.u32[j] = num;
        }

        _rs_mul_matrix_vector32(rs, parity, vandermonde, vector, \
                                 division);

        for (j=0;j<division;j++) {
            num = parity.u32[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                rsf->parity[j].slot[i * symbol_size + k] = unum;
            }
        }
    }
}

static void _rsf_decode16(rs_file_t *rsf,
                           rs_decode_t *rsd,
                           uint symbol_num)
{
    reed_solomon_t *rs = rsd->rs;
    uint i, j, k;
    size_t symbol_size = rs->symbol_size;
    register uint division = rsd->division;
    uchar unum;
    uint num;
    _ptr_t vector = rsd->_row;
    _ptr_t recovery = rsd->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += rsf->merged[j].slot[i * symbol_size + k];
            }
            vector.u16[j] = num;
        }

        _rs_mul_matrix_vector16(rs, recovery, rsd->inverse, vector, \
                                 division);

        for (j=0;j<division;j++) {
            num = recovery.u16[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                rsf->recovery[j].slot[i * symbol_size + k] = unum;
            }
        }
    }
}

static void _rsf_decode32(rs_file_t *rsf,
                           rs_decode_t *rsd,
                           uint symbol_num)
{
    reed_solomon_t *rs = rsd->rs;
    uint i, j, k;
    size_t symbol_size = rs->symbol_size;
    register uint division = rsd->division;
    uchar unum;
    uint num;
    _ptr_t vector = rsd->_row;
    _ptr_t recovery = rsd->_row2;

    for (i=0;i<symbol_num;i++) {
        for (j=0;j<division;j++) {
            num = 0;
            for (k=0;k<symbol_size;k++) {
                num <<= 8;
                num += rsf->merged[j].slot[i * symbol_size + k];
            }
            vector.u32[j] = num;
        }

        _rs_mul_matrix_vector32(rs, recovery, rsd->inverse, vector, \
                                 division);

        for (j=0;j<division;j++) {
            num = recovery.u32[j];
            for (k=0;k<symbol_size;k++) {
                unum = (num >> 8 * (symbol_size - 1 - k)) & 0xff;
                rsf->norm[j].slot[i * symbol_size + k] = unum;
            }
        }
    }
}

/* parts functions */

static void _rsf_write_header_of_kernel(rs_file_t *rsf,
                                         uint bits,
                                         int poly,
                                         uint division)
{
    _slot_size_brother_t *ssb = &rsf->ssb_;

    /* never change info */
    /* because it's kernel info */
    /* reed solomon info */
    fprintf(rsf->header, "0x%x\n", bits);
    fprintf(rsf->header, "0x%x\n", poly);
    /* rs info */
    fprintf(rsf->header, "0x%x\n", division);
    /* about text size */
    fprintf(rsf->header, "0x%x\n", ssb->text_size);

    /* separate, one empty line */
    fprintf(rsf->header, "\n");
}

static int _rsf_read_header_of_kernel(
                 FILE *header, \
                 uint *bits,
                 uint *poly,
                 uint *division, \
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
                   "failed to read \"sha1\" %s is " \
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
                   "failed to read hashed_name. " \
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

    ret = _rsf_set_rs(&rs, bits, division);
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
        _rsf_calc_rsd_memory_size(rsd, rs, division);
        grown_size = _rsf_init_rsd(rsd, mem, rs);
        mem += grown_size;
    }

    rse = (rs_encode_t *)mem; mem += sizeof(rs_encode_t);
    _rsf_calc_rse_memory_size(rse, rs, division);
    grown_size = _rsf_init_rse(rse, mem, rs);
    mem += grown_size;

    rsf = (rs_file_t *)mem; mem += sizeof(rs_file_t);
    rsf->mem = mem_;
    _rsf_calc_rsf_memory_size(rsf, division);
    grown_size = _rsf_init_rsf(rsf, mem, rsd, rse, build_type);
    mem += grown_size;

    #ifdef DEBUG
    _rsf_debug("grown_size = %u\n", grown_size);
    _rsf_debug("mem = %p\n", mem);

    _rsf_view_encode(rse);
    _rsf_view_file(rsf);
    _rsf_debug("mem - mem_ is %zu\n", mem - mem_);
    #endif

    if (rsd != NULL)
        _rsf_view_decode(rsd);
    if (rse != NULL)
        _rsf_view_encode(rse);
    if (rsf != NULL)
        _rsf_view_file(rsf);

    _rsf_make_vandermonde(rse);

    _rsf_debug("\n\n");

    return rsf;
}

/* fopen() mode is "encode" or "decode" */
int rsf_decode_redo(char *redo_file_name, char *header)
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

    redo_file_name[0] = '\0';

    ret = rsf_get_up_rsf_for_redo(&rsf, header);
    if (ret < 0)
        return ret;
    _rsf_debug("in rsf_decode_redo()\n");
    _rsf_debug("rs_get_up_rsf_for_recover() = %d ok.\n\n", ret);

    rse = rsf->rse;
    rsd = rsf->rsd;
    rs = rsd->rs;
    ssb = &rsf->ssb_;

    /* rank error, row swap を試してみること。 */
    _rsf_merge_slots(rsd, rsf, rse);
    _rsf_solve_inverse(rsd->inverse, rsd->merged, rsd);

    symbols_in_slot = ssb->symbols_in_slot;
    symbols_in_buffer = rsf->breath_size / rs->symbol_size;

    rsf_logger(ERROR, "in rsf_decode_redo().\n");

    if (symbols_in_slot < symbols_in_buffer) {
        available_symbols = symbols_in_slot;
    }
    else {
        available_symbols = symbols_in_buffer;
    }
    remained_symbols = symbols_in_slot;
    while (remained_symbols) {
        r = _fread_from_merged_fp(rsf,
                                  rs->symbol_size,
                                  available_symbols,
                                  rsd->division);
        if (r != available_symbols) {
            rsf_logger(ERROR, "rs_recover_file() failed.\n");
            rsf_logger(ERROR, "cause of that _fread_from_merged_fp()\n");
            ret = RSF_FREAD_ERROR;
            goto err_fread;
        }
        _rsf_decode(rsf, rsd, available_symbols);
        w = _fwrite_to_recovery_fp(rsf,
                                   rs->symbol_size, available_symbols,
                                   rsd->division);
        if (w != available_symbols) {
            rsf_logger(ERROR, "rs_recover_file() failed.\n");
            rsf_logger(ERROR, "cause of that _fwrite_to_recovery_fp()\n");
            ret = RSF_FWRITE_ERROR;
            goto err_fwrite;
        }
        remained_symbols = _update_remained_symbols(
                                remained_symbols, &available_symbols);
    }

    ret = _rsf_recover_come_back(rsf, ssb->symbols_in_slot, symbols_in_buffer);
    if (ret < 0) {
        rsf_logger(ERROR, "_rsf_recover_come_back(ssb->symbols_in_slot=%u, "
                           "symbols_in_buffer=%u) failed.\n",
                            ssb->symbols_in_slot, symbols_in_buffer);
        goto err_recover;
    }

    fflush(rsf->come_back);
    fseek(rsf->come_back, 0L, SEEK_SET);
    sha1_as_file(rsf->sha1sum, rsf->come_back);
    _to_hashed_name(redo_file_name, rsf->sha1sum);
    ret = RSF_SCUCCESS;

    _rsf_debug("in rsf_decode_redo()\n");
    _rsf_debug("rs_recover_file() done.\n\n");

err_recover:
err_fwrite:
err_fread:
    rsf_good_night_rsf_for_recover(rsf, redo_file_name);

    return ret;
}

static void _rsf_view_size_brother(_rsf_slot_size_brother_t *rssb)
{
    rsf_logger(INFO, "                rssb = %p\n", rssb);
    rsf_logger(INFO, "         text_size = %zu\n", rssb->text_size);
    rsf_logger(INFO, "         remainder_size = %zu\n", rssb->remainder_size);
    rsf_logger(INFO, "      padding_size = %zu\n", rssb->padding_size);
    rsf_logger(INFO, "       norm_size = %zu\n", rssb->norm_size);
    rsf_logger(INFO, "         slot_size = %zu\n", rssb->slot_size);
    rsf_logger(INFO, "    symbols_in_slot = %u\n", rssb->symbols_in_slot);
}

int rsf_get_up_rsf_for_redo(rs_file_t **_rsf, char *header)
{
    /* えっ、待ったするんですか？ */
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

    _rsf_debug("in rsf_get_up_rsf_for_redo()\n");

    /* _rsf_debug("header = \"%s\"\n", header); */
    header = fopen(header, "r");
    if (header == NULL) {
        rsf_logger(ERROR, "rs_get_up_rsf_for_recover() failed.\n");
        rsf_logger(ERROR, "cannot open header file \"%s\"\n", header);
        /* tekito */
        ret = -203;
        goto err_no_resource;
    }
    _rsf_debug("opened header file \"%s\"\n", header);

    ret = _rsf_read_header_of_kernel(header, \
                           &bits, &poly, &division, &text_size);
    if (ret < 0) {
        rsf_logger(ERROR, "rs_get_up_rsf_for_recover() failed.\n");
        rsf_logger(ERROR, "ret = %d, header = %p\n", \
                            ret, header);
        rsf_logger(ERROR, "bits=%u, poly=%u, division=%u, " \
                           "text_size=%zu\n\n", \
                            bits, poly, division, text_size);
        ret = -204;
        goto err_after_open_header;
    }
    _rsf_debug("_rsf_read_header_of_kernel() = %d ok.\n", ret);

    ret = _rsf_make_decoder(_rsf, bits, division);
    if (ret < 0) {
        rsf_logger(ERROR, "rs_get_up_rsf_for_recover() failed.\n");
        rsf_logger(ERROR, "ret = %d, header = %p\n", \
                            ret, header);
        rsf_logger(ERROR, "cannot make recoveryer. " \
                           "bits=%u, poly=%u, division=%u, " \
                           "text_size=%zu\n", \
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
                   "bits=%u, poly=%u, division=%u, " \
                   "text_size=%zu, hash=\"%s\"\n", \
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
    ret = _calculate_size(&rsf->ssb_,
                           text_size,
                           rsf->breath_size,
                           rsd->division,
                           rs->symbol_size);
    if (ret < 0)
        goto err_after_rsf_file_open;

    return RSF_SCUCCESS;

    _rsf_debug("\n\n");

    err_after_rsf_file_open:
        _rsf_files_close(rsf, rsd->division);
    err_after_open_header:
        fclose(header);
    err_no_resource:

    return ret;
}

int rsf_good_night_rsf_for_recover(rs_file_t *rsf, char *recovery_file_name)
{
    rs_decode_t *rsd = rsf->rsd;

    fclose(rsf->header);
    _rsf_files_close(rsf, rsd->division);
    _rsf_free(rsf);
    _rsf_debug("in rsf_good_night_rsf_for_recover()\n");
    _rsf_debug("closed recovery_file \"%s\"\n\n", recovery_file_name);

    return RSF_SCUCCESS;
}

static uint _rsf_get_rsf_division(rs_file_t *rsf)
{
    uint division = 0;

    if (rsf->rsd != NULL)
        division = rsf->rsd->division;
    else if (rsf->rse != NULL)
        division = rsf->rse->division;
    else {
        rsf_logger(ERROR,  "_rsf_get_rsf_division() failed.\n"
                            "cause of that rsd and rse are NULL pointer.\n\n");
        return RSF_RSD_AND_RSE_SET_ERROR;
    }

    return division;
}

static void _rsf_view_file(rs_file_t *rsf)
{
    uint i, division = 0;
    char ss[RSF_BUFFER_SIZE];
    sha1sum_t sha1sum[1];

    division = _rsf_get_rsf_division(rsf);

    rsf_logger(INFO, "rsf = %p\n", rsf);
    rsf_logger(INFO, "             text = %p\n", rsf->text);
    rsf_logger(INFO, "             come_back = %p\n", rsf->come_back);
    rsf_logger(INFO, "              mode = %d is ", rsf->mode);
    if (rsf->mode == MODE_ENCODE)
        rsf_logger(INFO, "ENCODE");
    else if (rsf->mode == MODE_RECOVERY)
        rsf_logger(INFO, "RECOVERY");
    else
        rsf_logger(INFO, "UNKNOWN");
    rsf_logger(INFO, "\n");
    rsf_logger(INFO, "         temp_path = \"%s\"(=%p)\n", \
                                    rsf->temp_path, rsf->temp_path);
    rsf_logger(INFO, "         temp_tail = \"%s\"(=%p)\n", \
                                    rsf->temp_tail, rsf->temp_tail);
    rsf_logger(INFO, "       header = %p\n", rsf->header);
    _rsf_view_size_brother(&rsf->ssb_);
    rsf_logger(INFO, "              norm = %p\n", rsf->norm);
    for (i=0;i<division;i++) {
        rsf_logger(INFO, "    norm[%5d].fp = %p\n", i, rsf->norm[i].fp);
    }
    rsf_logger(INFO, "            parity = %p\n", rsf->parity);
    for (i=0;i<division;i++) {
        rsf_logger(INFO, "  parity[%5d].fp = %p\n", i, rsf->parity[i].fp);
    }
    rsf_logger(INFO, "            merged = %p\n", rsf->merged);
    for (i=0;i<division;i++) {
        rsf_logger(INFO, "  merged[%5d].fp = %p\n", i, rsf->merged[i].fp);
    }
    rsf_logger(INFO, "          recovery = %p\n", rsf->recovery);
    for (i=0;i<division;i++) {
        rsf_logger(INFO, "recovery[%5d].fp = %p\n", i, rsf->recovery[i].fp);
    }
    _to_hashed_name(ss, sha1sum);
    rsf_logger(INFO, "              sha1sum = %p = %s\n", rsf->sha1sum, ss);
    rsf_logger(INFO, "     allocate_size = %zu\n", rsf->allocate_size);
    rsf_logger(INFO, "        slots_size = %zu\n", rsf->slots_size);
    rsf_logger(INFO, "temp_path_max_size = %zu\n", rsf->temp_path_max_size);
    rsf_logger(INFO, " dir_name_max_size = %zu\n", rsf->dir_name_max_size);
    rsf_logger(INFO, "    base_name_size = %zu\n", rsf->base_name_size);
    rsf_logger(INFO, "\n");
}

/* must set rsf->breath_size before below function call. */
static size_t _rsf_calc_rsf_memory_size(rs_file_t *rsf, uint division)
{
    size_t hex_name_size = 0;

    rsf->hash_size = RSF_HASH_SIZE;
    rsf->hash_size = _aligned_size(rsf->hash_size);
    rsf_logger(FATAL, "rsf->breath_size = %u\n", rsf->breath_size);
    rsf->breath_size = RSF_BREATH_SIZE;
    rsf->breath_size = _aligned_size(rsf->breath_size);
    /* buffer and slots need three type. there are norm, parity, merged */
    rsf->slots_size = division * sizeof(_slot_t);
    rsf->slots_size = _aligned_size(rsf->slots_size);
    rsf->temp_path_max_size = RSF_PATH_MAX_SIZE;
    rsf->temp_path_max_size = _aligned_size(rsf->temp_path_max_size);
    /* SHA1("abc") is equal to "a9993e364706816aba3e25717850c26c9cd0d89d"  */
    /* hash_name = SHA1("abc") */
    hex_name_size = rsf->hash_size * 2;
    /* base_name = tmpnam() + "." + "0x04x" + '\0'  = L_tmpnam(=20) + 1 + 6 + 1
     *             or
     *             "h" + SHA1("abc") + '\0' = 1 + 40 + 1
     */
    rsf->base_name_size = 1 + hex_name_size + 1;
    rsf->base_name_size = _aligned_size(rsf->base_name_size);
    /* temp_path = dir_name + "/" + base_name */
    /* therefore need to decrement dir_name_max_size for "/" */
    rsf->dir_name_max_size = rsf->temp_path_max_size - rsf->base_name_size - 1;
    rsf->dir_name_max_size = _aligned_size(rsf->dir_name_max_size);

    rsf->allocate_size = rsf->slots_size * 4 + \
                         rsf->breath_size * division * 4 + \
                         rsf->temp_path_max_size + \
                         rsf->base_name_size + \
                         rsf->dir_name_max_size + \
                         0; /* no mean */
    return rsf->allocate_size;
}

static size_t _rsf_init_rsf(rs_file_t *rsf,
                             char *mem,
                             rs_decode_t *rsd,
                             rs_encode_t *rse,
                             int mode)
{
    uint i, division = 0;
    char *mem_ = mem;

    rsf->rsd = rsd;
    rsf->rse = rse;

    division = _rsf_get_rsf_division(rsf);

    rsf->mode = mode;

    rsf->norm = (_slot_t *)mem;     mem += rsf->slots_size;
    rsf->parity = (_slot_t *)mem;   mem += rsf->slots_size;
    rsf->merged = (_slot_t *)mem;   mem += rsf->slots_size;
    rsf->recovery = (_slot_t *)mem; mem += rsf->slots_size;

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
        rsf->recovery[i].slot = (uchar *)mem;
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
        allocate_size += sizeof(rs_file_t) + rsf->allocate_size;
        rsf_logger(DEBUG_, "sizeof(rs_file_t) = %u\n",
                             sizeof(rs_file_t));
        rsf_logger(DEBUG_, "rsf->allocate_size = %u\n", rsf->allocate_size);
    }
    if (rsd != NULL) {
        _rsf_calc_rsd_memory_size(rsd, rs, division);
        allocate_size += sizeof(rs_decode_t) + rsd->allocate_size;
        rsf_logger(DEBUG_, "sizeof(rs_decode_t) = %u\n",
                             sizeof(rs_decode_t));
        rsf_logger(DEBUG_, "rsd->allocate_size = %u\n", rsd->allocate_size);
    }
    if (rse != NULL) {
        _rsf_calc_rse_memory_size(rse, rs, division);
        allocate_size += sizeof(rs_encode_t) + rse->allocate_size;
        rsf_logger(DEBUG_, "sizeof(rs_encode_t) = %u\n",
                             sizeof(rs_encode_t));
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
        rsf_logger(ERROR, "mem = 0x%p, allocate_size=%u\n",
                            mem, allocate_size);
    }
    if (mem != NULL)
        memset(mem, '\0', allocate_size);

    return mem;
}

static int _rsf_make_encoder(rs_file_t **_rsf, uint bits, uint division)
{
    rs_file_t *rsf;

    rsf = rsf_construct(bits, division, MODE_ENCODE);
    if (rsf == NULL) {
        rsf_logger(ERROR, "rsf_construct(bits=%u, division=%u, build_type=MODE_ENCODE) failed.\n", bits, division);
        return RSF_P2F_ERROR;
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
        return RSF_P2F_ERROR;
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

static int _rsf_get_index_of_slots(_slot_t *hunter,
                                    _slot_t target,
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

static int _rsf_merge_slots(rs_decode_t *rsd,
                             rs_file_t *rsf,
                             rs_encode_t *rse)
{
    uint i, j = 0, i_index, j_index;
    reed_solomon_t *rs = rsd->rs;

    _rsf_make_e_matrix(rsd->merged, rs, rsd->division);
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

    tmpnam(rsf->temp_path);
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
                rsf->recovery[i].fp = fopen(hashed_name, "wb+");
            }
            else {
                rsf->recovery[i].fp = NULL;
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
    _slot_size_brother_t *ssb = &rsf->ssb_;
    uint division = 0;
    uint len_norm_slots, len_parity_slots;

    _rsf_debug("in _rsf_files_open()\n");
    if (rsf->mode == MODE_ENCODE) {
        _rsf_debug("rsf->mode == MODE_ENCODE.\n");
    }
    else {
        _rsf_debug("rsf->mode == MODE_RECOVERY.\n");
    }

    division = _rsf_get_rsf_division(rsf);
    if (rsf->mode == MODE_ENCODE ) {
        rsf->text = fopen(text_path, mode);
        if (rsf->text == NULL) {
            rsf_logger(ERROR,
                       "_rsf_files_open() failed.\n"
                       "cannot open \"%s\" " \
                       "to make rs file.\n", text_path);
            return RSF_INVALID_PATH_ERROR;
        }
    }
    else if (rsf->mode == MODE_RECOVERY) {
        rsf->come_back = fopen(text_path, mode);
        _rsf_debug("rsf->come_back = fopen(text_path=\"%s\", mode=\"%s\")\n",
                                            text_path, mode);
        _rsf_debug("rsf->come_back = %p\n", rsf->come_back);
        if (rsf->come_back == NULL) {
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
        ssb->text_size = rsf->sha1sum->message_size;
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
        if (rsf->recovery[i].fp != NULL)
            fclose(rsf->recovery[i].fp);
    }

    if (rsf->text != NULL)
        fclose(rsf->text);
    if (rsf->come_back != NULL)
        fclose(rsf->come_back);

    _rsf_debug("\n\n");
}

static size_t _fread_from_norm_fp(rs_file_t *rsf,
                                  size_t symbol_size,
                                  uint available_symbols,
                                  uint division)
{
    size_t r = 0;
    uint i;
    FILE *fp = NULL;
    uchar *buf = NULL;

    for (i=0;i<division;i++){
        fp = rsf->norm[i].fp;
        buf = rsf->norm[i].slot;
        r = fread(buf, symbol_size, available_symbols, fp);

        if (r != available_symbols) {
            rsf_logger(ERROR,
                       "_fread_from_norm_fp() failed.\n"
                       "cause of fread().\n"
                       "i = %u, buf = %p, symbol_size=%u, fp = %p\n"
                       "r(=%u) != available_symbols(=%u)\n\n",
                        i, buf, symbol_size, fp,
                        r, available_symbols);
            return r;
        }
    }

    return r;
}

static size_t _fwrite_to_parity_fp(rs_file_t *rsf,
                                   size_t symbol_size,
                                   uint available_symbols,
                                   uint division)
{
    size_t w = 0;
    uint i;
    FILE *fp = NULL;
    uchar *buf = NULL;

    for (i=0;i<division;i++){
        fp = rsf->parity[i].fp;
        buf = rsf->parity[i].slot;
        w = fwrite(buf, symbol_size, available_symbols, fp);
        if (w != available_symbols) {
            rsf_logger(ERROR,
                       "_fwrite_to_parity_fp() failed.\n"
                       "cause of fwrite().\n"
                       "i = %u, buf = %p, symbol_size=%u, fp = %p\n"
                       "w(=%u) != available_symbols(=%u)\n\n",
                        i, buf, symbol_size, fp,
                        w, available_symbols);
            return w;
        }
    }

    return w;
}

static size_t _fread_from_merged_fp(rs_file_t *rsf,
                                    size_t symbol_size,
                                    uint available_symbols,
                                    uint division)
{
    size_t r = 0;
    uint i;
    FILE *fp = NULL;
    uchar *buf = NULL;

    for (i=0;i<division;i++){
        fp = rsf->merged[i].fp;
        buf = rsf->merged[i].slot;
        r = fread(buf, symbol_size, available_symbols, fp);
        if (r != available_symbols) {
            rsf_logger(ERROR,
                       "_fread_from_merged_fp() failed.\n"
                       "cause of fread().\n"
                       "i = %u buf = %p, symbol_size=%u, fp = %p\n"
                       "r(=%u) != available_symbols(=%u)\n\n",
                        i, buf, symbol_size, fp,
                        r, available_symbols);
            return r;
        }
    }

    return r;
}

static size_t _fwrite_to_recovery_fp(rs_file_t *rsf,
                                     size_t symbol_size,
                                     uint available_symbols,
                                     uint division)
{
    size_t w = 0;
    uint i;
    FILE *fp = NULL;
    uchar *buf = NULL;

    for (i=0;i<division;i++){
        fp = rsf->recovery[i].fp;
        _rsf_debug("in _fwrite_to_recovery_fp() rsf->recovery[%d].fp = %p\n\n", i, fp);
        if (fp == NULL) {
            continue;
        }
        buf = rsf->recovery[i].slot;
        w = fwrite(buf, symbol_size, available_symbols, fp);
        if (w != available_symbols) {
            rsf_logger(ERROR,
                       "_fwrite_to_recovery_fp() failed.\n"
                       "cause of fwrite().\n"
                       "i = %d buf = %p, symbol_size=%u, fp = %p\n"
                       "w(=%u) != available_symbols(=%u)\n\n",
                        i, buf, symbol_size, fp,
                        w, available_symbols);
            return w;
        }
    }

    return w;
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

/* similar _rsf_recover_come_back() */
static int _rsf_split_text(rs_file_t *rsf,
                            size_t symbol_size,
                            uint division,
                            uint symbols_in_slot,
                            uint symbols_in_buffer)
{
    uint i;
    size_t r = 0, w = 0;
    FILE *fp = NULL;
    uchar *buf = NULL;
    uint remained_symbols, available_symbols;

    rsf_logger(ERROR, "symbol_size=%u, division=%u, "
                       "symbols_in_slot=%u, symbols_in_buffer=%u\n",
                        symbol_size, division,
                        symbols_in_slot, symbols_in_buffer);

    for (i=0;i<division;i++) {
        fp = rsf->norm[i].fp;
        buf = rsf->norm[i].slot;
        rsf_logger(ERROR, "in _rsf_split_text().\n");

        if (symbols_in_slot < symbols_in_buffer) {
            available_symbols = symbols_in_slot;
        }
        else {
            available_symbols = symbols_in_buffer;
        }
        remained_symbols = symbols_in_slot;
        while (remained_symbols) {
            r = fread(buf, symbol_size, available_symbols, rsf->text);
            if (r != available_symbols) {
                rsf_logger(ERROR,
                           "_rsf_split_text() failed.\n"
                           "cause of fread().\n"
                           "i = %d buf = %p, symbol_size=%u, rsf->text = %p\n"
                           "r(=%u) != available_symbols(=%u)\n\n",
                            i, buf, symbol_size, rsf->text,
                            r, available_symbols);
                return RSF_FREAD_ERROR;
            }
            w = fwrite(buf, symbol_size, available_symbols, fp);
            if (w != available_symbols) {
                rsf_logger(ERROR,
                           "_rsf_split_text() failed.\n"
                           "cause of fwrite().\n"
                           "i = %d buf = %p, symbol_size=%u, fp = %p\n"
                           "w(=%u) != available_symbols(=%u)\n\n",
                            i, buf, symbol_size, fp,
                            w, available_symbols);
                return RSF_FWRITE_ERROR;
            }
            remained_symbols = _update_remained_symbols(
                                    remained_symbols, &available_symbols);
        }
        fflush(fp);
        rewind(fp);
    }
    return RSF_SCUCCESS;
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
static int rsf_encode_file(char *hashed_header,
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

    hashed_header[0] = '\0';

    ret = _rsf_make_encoder(&rsf, bits, division);
    if (ret < 0)
        return ret;

    rse = rsf->rse;
    rs = rse->rs;
    ssb = &rsf->ssb_;

    ret = _rsf_files_open(&len_available_slots, rsf, path, "rb");
    if (ret < 0) {
        _rsf_free(rsf);
        return ret;
    }
    ret = _calculate_size(ssb,
                          ssb->text_size,
                          rsf->breath_size,
                          rse->division,
                          rs->symbol_size);
    if (ret < 0){
        return ret;
    }

    rsf->header = _fopen_rse(rsf, "wb", 2 * rse->division);
    _rsf_write_header_of_kernel(rsf, \
                                rs->bits, rs->poly, rse->division);

    /* PETA(too large) size file and slots make too large size slot.
     * symbols_in_slot = TERA = PETA / number of slots
     * symbols_in_beffer = also from 1024 to 4096 octets.
     * then, symbols_in_slot >>>>>>> symbols_in_buffer
     */
    symbols_in_slot = ssb->symbols_in_slot;
    symbols_in_buffer = rsf->breath_size / rs->symbol_size;

    ret = _rsf_split_text(rsf, rs->symbol_size, rse->division,
                          symbols_in_slot, symbols_in_buffer);
    if (ret < 0) {
        _rsf_free(rsf);
        return ret;
    }

    rsf_logger(WARN, "in _rsf_encode_file()\n");
    if (symbols_in_slot < symbols_in_buffer) {
        available_symbols = symbols_in_slot;
    }
    else {
        available_symbols = symbols_in_buffer;
    }
    remained_symbols = symbols_in_slot;
    while (remained_symbols) {
        _fread_from_norm_fp(rsf, rs->symbol_size, available_symbols,
                           rse->division);
        _rsf_encode(rsf, rse, available_symbols);
        _fwrite_to_parity_fp(rsf, rs->symbol_size, available_symbols,
                            rse->division);
        remained_symbols = _update_remained_symbols(
                                remained_symbols, &available_symbols);
    }

    _rsf_files_close(rsf, rse->division);
    _rsf_write_header_of_hash(rsf, rse->division);
    fclose(rsf->header);

    _rename_to_hashed_name(hashed_header, rsf, 2 * rse->division);

    _rsf_free(rsf);

    return RSF_SCUCCESS;
}

/* similar _rsf_split_text() */
static int _rsf_recover_come_back(rs_file_t *rsf,
                                   uint symbols_in_slot,
                                   uint symbols_in_buffer)
{
    uint i;
    FILE *fp = NULL;
    rs_decode_t *rsd = rsf->rsd;
    reed_solomon_t *rs = rsd->rs;
    size_t symbol_size = rs->symbol_size;
    size_t r, w;
    uint remained_symbols, available_symbols;
    _slot_t slot_n, slot_r, tmp;
    uchar *buf = NULL;

    for (i=0;i<rsd->division;i++) {
        slot_n = rsf->norm[i];
        slot_r = rsf->recovery[i];
        if (slot_n.fp != NULL)
            tmp = slot_n;
        else
            /* slot_r.fp != NULL */
            tmp = slot_r;
        rewind(tmp.fp);

        buf = tmp.slot;
        fp = tmp.fp;
        rsf_logger(WARN, "in _rsf_recover_come_back()\n");

        if (symbols_in_slot < symbols_in_buffer) {
            available_symbols = symbols_in_slot;
        }
        else {
            available_symbols = symbols_in_buffer;
        }
        remained_symbols = symbols_in_slot;
        while (remained_symbols) {
            r = fread(buf, symbol_size, available_symbols, fp);
            if (r != available_symbols) {
                rsf_logger(ERROR,
                           "_rsf_recover_come_back() failed.\n"
                           "cause of fread().\n"
                           "i = %d buf = %p, symbol_size=%u, fp = %p\n"
                           "r(=%u) != available_symbols(=%u)\n\n",
                            i, buf, symbol_size, fp,
                            r, available_symbols);
                return RSF_FREAD_ERROR;
            }
            w = fwrite(buf, symbol_size, available_symbols, rsf->come_back);
            if (w != available_symbols) {
                rsf_logger(ERROR,
                           "_rsf_recover_come_back() failed.\n"
                           "cause of fwrite().\n"
                           "i = %d buf = %p, symbol_size=%u, "
                           "rsf->come_back = %p\n"
                           "w(=%u) != available_symbols(=%u)\n\n",
                            i, buf, symbol_size, rsf->come_back,
                            w, available_symbols);
                return RSF_FREAD_ERROR;
            }
            remained_symbols = _update_remained_symbols(
                                    remained_symbols, &available_symbols);
        }
    }

    return RSF_SCUCCESS;
}

int _calculate_size(_slot_size_brother_t *ssb,
                    size_t text_size, size_t breath_size,
                    uint division, size_t symbol_size)
{
    size_t _column_size = division * symbol_size;
    /*
    if not 1 <= text_size <= TEXT_SIZE_MAX:
        ValueError('text_size must be 1 <= text_size <= 2 ** 64 - 1')
    return RSF_TEXT_SIZE_ERROR;
    */

    ssb->text_size = text_size;
    ssb->remainder_size = text_size % _column_size;
    if (ssb->remainder_size == 0)
        ssb->padding_size = 0;
    else
        ssb->padding_size = _column_size - ssb->remainder_size;

    ssb->norm_size = text_size + ssb->padding_size;
    if (ssb->norm_size < text_size) {
        rsf_logger(ERROR,
                "_calculate_size() failed.\n"
                "overflowed norm_size(=%u) for "
                "adding text_size(=%u) and padding_size(=%u)\n",
                 ssb->norm_size, text_size, ssb->padding_size);
        rsf_logger(ERROR, "\n");
        return RSF_NORM_SIZE_ERROR;
    }

    ssb->slot_size = ssb->norm_size / division;
    ssb->symbols_in_slot = ssb->slot_size / symbol_size;
    ssb->symbols_in_buffer = breath_size / symbol_size;

    _rsf_debug("in _calculate_size()\n");
    _rsf_debug("division = %u\n", division);
    _rsf_debug("symbol_size = %u\n", symbol_size);
    _rsf_debug("_column_size = %u\n", _column_size);
    _rsf_debug("text_size = %u\n", ssb->text_size);
    _rsf_debug("breath_size = %u\n", breath_size);
    _rsf_debug("remainder_size = %u\n", ssb->remainder_size);
    _rsf_debug("padding_size = %u\n", ssb->padding_size);
    _rsf_debug("norm_size = %u\n\n", ssb->norm_size);
    _rsf_debug("slot_size = %u\n", ssb->slot_size);
    _rsf_debug("symbols_in_slot = %u\n", ssb->symbols_in_slot);
    _rsf_debug("symbols_in_buffer = %u\n", ssb->symbols_in_buffer);

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
        rsf_logger(ERROR, "rename(\"%s\", \"%s\") failed.\n\n", \
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

static int _tohex(int ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    else
        return -1;
}

static void _str2hash(uchar *hash, char *ss, uint len_hash)
{
    uint i;
    for (i=0;i<len_hash / 8;i++) {
        hash[i] = (_tohex(ss[2*i]) << 4) + _tohex(ss[2*i+1]);
    }
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

