#ifndef RSF_H
#define RSF_H
#ifdef __cplusplus
extern "C" {
#endif

#include "rs.h"

#define RSF_BREATH_SIZE 1024
#define RSF_PATH_MAX_SIZE 256
#define RSF_HASH_BIT_LENGTH SHA1SUM_HASH_BITS
#define RSF_HASH_SIZE SHA1SUM_HASH_SIZE
/* please check your environment PATH_MAX_SIZE.
 * you may got disaster if it is smaller than RSF_PATH_MAX_SIZE.
 * however it's none of my business.
 */
#define MODE_ENCODE 0
#define MODE_RECOVERY 1

#define RSF_FP 1
#define RSF_SS 2

#define RSF_NORM_SIZE_ERROR (-10)
#define RSF_LACK_OF_SLOTS_ERROR (-9)
#define RSF_TEXT_SIZE_ERROR (-4)
#define RSF_INVALID_PATH_ERROR (-7)
#define RSF_INVALID_MODE_ERROR (-8)
#define RSF_FREAD_ERROR (-12)
#define RSF_FWRITE_ERROR (-13)
#define RSF_P2F_ERROR (-14)

/* #288: recover, restore, redo, back, ... を決めよう。*/
/* file */
typedef struct {
    char *mem;

    FILE *text;
    FILE *restored;
    sha1sum_t sha1sum[1];

    size_t text_size;
    size_t symbols_in_slot;
    size_t symbols_in_buffer;

    int mode; /* MODE_ENCODE or MODE_DECODE */
    char *temp_path; /* file path or "__memory__" */
    char *temp_tail; /*  */
    char *base_name;
    char *dir_name;
    FILE *header;

    rs_encode_t *rse;
    rs_decode_t *rsd;
    rs_slot_t *norm, *parity, *merged, *recover;

    size_t allocate_size;
    size_t hash_size;
    size_t breath_size;
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

#ifdef __cplusplus
}
#endif
#endif /* RSF_H */
