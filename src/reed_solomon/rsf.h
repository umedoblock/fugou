#ifndef __RSF_H__
#define __RSF_H__

#include "rs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUF_SIZE
#define RSF_BUF_SIZE (1024)
#else
#define RSF_BUF_SIZE BUF_SIZE
#endif
#define RSF_PATH_MAX_SIZE (256)
#define RSF_HASH_BIT_LENGTH SHA1SUM_HASH_BITS
#define RSF_HASH_SIZE SHA1SUM_HASH_SIZE
#define RSF_SS_SIZE (80)

/* please check your environment PATH_MAX_SIZE.
 * you may got disaster if it is smaller than RSF_PATH_MAX_SIZE.
 * however it's none of my business.
 */
#define MODE_ENCODE (0)
#define MODE_RECOVERY (1)

#define RSF_FP (1)
#define RSF_SS (2)

#define RSF_SCUCCESS (0)
#define RSF_INVALID_PATH_ERROR (-1)
#define RSF_INVALID_MODE_ERROR (-2)
#define RSF_LACK_OF_SLOTS_ERROR (-3)
#define RSF_CONSTRUCT_ERROR (-4)
#define RSF_FREAD_ERROR (-5)
#define RSF_FWRITE_ERROR (-6)
#define RSF_DIVISION_ERROR (-7)

/* #288: recover, restore, redo, back, ... を決めよう。*/
/* file */
typedef struct {
    char *mem;

    slot_file_t *file;
    sha1sum_t sha1sum[1];
    _slot_size_sister_t sss_last[1];
    _slot_size_brother_t ssb_last[1];

    size_t text_size;

    int mode; /* MODE_ENCODE or MODE_DECODE */
    char *temp_path; /* file path or "__memory__" */
    char *temp_tail; /*  */
    char *dir_name;
    char *base_name;
    FILE *header;

    rs_encode_t *rse;
    rs_decode_t *rsd;
    slot_t *norm, *parity, *merged, *recover;

    size_t allocate_size;
    size_t hash_size;
    size_t slots_size;
    size_t temp_path_max_size;
    size_t dir_name_max_size;
    size_t base_name_size;
} rs_file_t;

int rsf_encode_text(char *hashed_header,
                    const char *path,
                    uint bits,
                    uint division);
int rsf_decode_restored(char *restored_file_name, char *header);

/* proto type ****************************************************************/
static int _rsf_files_open(uint *len_available_slots,
                           rs_file_t *rsf,
                           const char *text_path,
                           const char *mode);
static int _rsf_make_decoder(rs_file_t **_rsf, uint bits, uint division);
static int _rsf_good_night_rsf_for_recover(rs_file_t *rsf,
                                           char *recovery_file_name);
static int _calc_slot_size(_slot_size_brother_t *ssb,
                            rs_file_t *rsf,
                            size_t text_size,
                            size_t buf_size,
                            uint division,
                            size_t symbol_size);
static int _ishashstring(char *hashed_string, uint hash_len);
static void _str2hash(uchar *hash, char *ss, uint len_hash);
static int _rename_to_hashed_name(char *hashed_name, rs_file_t *rsf, int no);
static int _rsf_recover_restored(rs_file_t *rsf,
                                 uint symbols_in_slot,
                                 uint symbols_in_buffer);
static int _rsf_merge_slots(rs_decode_t *rsd,
                            rs_file_t *rsf,
                            rs_encode_t *rse);
static uint _update_remained_symbols(uint remained_symbols,
                                     uint *available_symbols);
static void _rsf_files_close(rs_file_t *rsf, uint division);
static char *_rsf_allocate(
                   rs_file_t *rsf,
                   rs_decode_t *rsd,
                   rs_encode_t *rse,
                   reed_solomon_t *rs,
                   uint division);
static size_t _rsf_init(rs_file_t *rsf,
                        char *mem,
                        rs_decode_t *rsd,
                        rs_encode_t *rse,
                        int mode);
static void _rsf_view_rsf(rs_file_t *rsf);
static uint _rsf_get_division(rs_file_t *rsf);
static char *_fgets_(char *s, int size, FILE *stream);
static void _to_hashed_name(char *ss, sha1sum_t *sha1sum);
static int _rsf_free(rs_file_t *rsf);
static size_t _rsf_calc_rsf_memory_size(rs_file_t *rsf, uint division);
int _rsf_calc_symbols(rs_file_t *rsf,
                      size_t symbol_size,
                      size_t slot_size,
                      size_t buf_size);

#ifdef __cplusplus
}
#endif
#endif /* __RSF_H__ */
