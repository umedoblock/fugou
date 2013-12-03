#ifndef SLOT_H
#define SLOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include "../libfugou_base.h"
#include "../sha/sha.h"

#define LOGGED_ERRORNO() do { \
        errnum = errno; \
        strerror_r(errnum, errmsg, SS_SIZE); \
        LOGGER(ERROR, "errnum=%d\nerrmsg=\"%s\"\n", errnum, errmsg); \
    } while(0)

/*
$ ./stat a.out
File type:                regular file
Preferred I/O block size: 4096 bytes
File size:                3808 bytes
*/
#ifdef SLOT_BUF_SIZE
    /* なにもしない*/
#elif defined LIBFUGOU_SLOT_BUF_SIZE
    #define SLOT_BUF_SIZE LIBFUGOU_SLOT_BUF_SIZE
#else
    #define SLOT_BUF_SIZE (1024)
#endif

#define SLOT_SUCCESS             (0)
#define SLOT_FREAD_ERROR        (-1)
#define SLOT_FWRITE_ERROR       (-2)
#define SLOT_NULL_ERROR         (-3)
#define SLOT_OUT_OF_RANGE_ERROR (-4)
#define SLOT_slot_size_ERROR    (-5)
#define SLOT_UNKOWN_VERV_ERROR  (-6)
#define SLOT_FOPEN_ERROR        (-7)
#define SLOT_FCLOSE_ERROR       (-8)
#define SLOT_REMOVE_ERROR       (-9)
#define SLOT_READING_ERROR     (-10)
#define SLOT_reading_ERROR     (-10)
#define SLOT_WRITING_ERROR     (-11)
#define SLOT_writing_ERROR     (-11)
#define SLOT_FGETPOS_ERROR     (-12)
#define SLOT_FSETPOS_ERROR     (-13)
#define SLOT_UNKNOWN_TYPE_ERROR (-14)
#define SLOT_FSEEK_ERROR       (-15)
#define SLOT_SLOT_SIZE_ERROR   (-16)

#define FROM_HEAD                (1)
#define FROM_CURRENT             (2)

#define SLOT_FILE   (1)
#define SLOT_SOCKET (2)
#define SLOT_MEMORY (3)

#define SLOT_DIVIDE              (1)
#define SLOT_INTEGRATE           (2)

#define SLOT_SEP ("/")
#define SLOT_PARENT_FORMAT ("%s%s%s")
#define SLOT_CHILDLEN_FORMAT ("%s.0x%08x") /* PARENT_NAME + ".0x00000000" */

#define SLT(s) ((slot_t *)s)
#define SLT_(s) ((struct slot_t *)s)
#define SLF(s) ((slot_file_t *)s)
#define SLS(s) ((slot_socket_t *)s)
#define SLM(s) ((slot_memory_t *)s)

#define SLOT_ptr(s) (SLT(s))
#define SLOT_slt(s) (SLOT_ptr(s)->slt[0])
#define SLOT_sb(s) (SLOT_slt(s).sb)
/*
#define SLOT_target(s) (SLOT_ptr(s)->target)
*/
#define SLOT_target(s) ((SLOT_ptr(s)->target[0].ptr))
#define SLOT_target_f(s) ((SLOT_ptr(s)->target + 0)->f)
#define SLOT_pos(s) (SLOT_slt(s).pos)
#define SLOT_pos_f(s) (SLOT_slt(s).pos.f)
#define SLOT_buf(s) (SLOT_slt(s).buf)
#define SLOT_reading(s) (SLOT_slt(s).reading)
#define SLOT_writing(s) (SLOT_slt(s).writing)
#define SLOT_computing(s) (SLOT_slt(s).computing)
#define SLOT_parent(s) (SLOT_slt(s).parent)
#define SLOT_type(s) (SLOT_slt(s).type)
#define SLOT_buf_size(s) (SLOT_sb(s)->buf_size)
#define SLOT_target_size(s) (SLOT_sb(s)->target_size)
#define SLOT_index(s) (SLOT_sb(s)->index)
#define SLOT_symbol_size(s) (SLOT_sb(s)->symbol_size)
#define SLOT_division(s) (SLOT_sb(s)->division)
#define SLOT_slot_size(s) (SLOT_sb(s)->slot_size)
#define SLOT_column_size(s) (SLOT_sb(s)->column_size)
#define SLOT_padding_size(s) (SLOT_sb(s)->padding_size)
#define SLOT_name(s) (SLOT_slt(s).name)

#define children_i (children + i)

typedef struct {
    size_t target_size;
    size_t symbol_size;
    uint division;          /* i so row */
    size_t slot_size;       /* = target_size + padding_size */
    size_t column_size;     /* = division * symbol_size */
    size_t padding_size;    /* = column_size - remainder_size */
    size_t buf_size;
    size_t index;
} _slot_size_brother_t;

typedef union {
    fpos_t f[1]; /* file, sizeof(fpos_t) = 12 */
    /* int s[1]; socket */
    uint m[1];    /* memory sizeof(uint) = 4 */
} _pos_t;

typedef union {
    void *ptr;
    FILE *f;
    int s[1];
    uint m[1];
    /* sock_t s[1]; */
    uchar *u;
} _target_t;

typedef struct _slot_t _slot_t;
typedef struct slot_t slot_t;

typedef size_t (*compute_function)(slot_t *, slot_t *, uint, size_t);
typedef size_t (*io_function)(slot_t *, void *, size_t );
typedef size_t (*pos_function)(slot_t *, void * );

struct _slot_t {
    int type;
    uchar *buf;
    _slot_size_brother_t sb[1];
    io_function reading;
    io_function writing;
    compute_function computing;
    _pos_t pos;
    pos_function getpos;
    pos_function setpos;
    char name[SS_SIZE];
} ;

struct slot_t {
    _target_t target[1]; /* target is often too long. */
    struct _slot_t slt[1]; /* buffer is short length. */
} ; /* slt */

typedef struct {
    _target_t target[1];
    _slot_t slt[1];
} slot_file_t; /* slf */

typedef struct {
    _target_t target[1];
    _slot_t slt[1];
} slot_socket_t; /* sls */

typedef struct {
    _target_t target[1];
    _slot_t slt[1];
} slot_memory_t; /* slm */

int _slot_calc_sb_children_by_division(slot_t *children,
                                        size_t parent_target_size,
                                        size_t parent_slot_size,
                                        size_t symbol_size,
                                        size_t column_size,
                                        size_t padding_size,
                                        uint division);
int slot_calc_sb_by_division(slot_t *parent,
                             slot_t *children,
                             size_t parent_target_size,
                             uint division,
                             size_t symbol_size,
                             size_t append_size);
void _slot_calc_division_by_child_slot_size(uint *division,
                                      size_t *append_size,
                                      size_t parent_target_size,
                                      size_t child_slot_size);
int slot_calc_sb_by_child_slot_size(slot_t *parent,
                              slot_t *children,
                              size_t parent_target_size,
                              size_t child_slot_size,
                              size_t symbol_size);

size_t slot_get_memory_size(void);
slot_t *slot_set_memory(uchar *mem, int num);
size_t slot_ask_target_size(slot_t *slt, int whence);
size_t slot_dev_null_r(slot_t *read_, void *buf, size_t read_size);
size_t slot_dev_null_w(slot_t *writ, void *buf, size_t writ_size);
size_t slot_fread(slot_t *read_, void *buf, size_t read_size);
size_t slot_fwrite(slot_t *writ, void *buf, size_t writ_size);
size_t slot_read(slot_t *read_, void *buf, size_t read_size);
size_t slot_write(slot_t *writ, void *buf, size_t writ_size);
size_t slot_memcpy_r(slot_t *read_, void *buf, size_t read_size);
size_t slot_memcpy_w(slot_t *writ, void *buf, size_t writ_size);
int slot_setpos(slot_t *slt, _pos_t *pos);

int _slot_divide(slot_t *children, slot_t *parent, uint division);
int _slot_integrate(slot_t *parent, slot_t *children, uint division);

int slot_children_set_first_pos(slot_t *parent,
                                slot_t *children,
                                uint division);
int slot_file_named(slot_file_t *file,
                    const char *dir_name,
                    const char *base_name);
int slot_file_fopen(slot_file_t *file,
                    const char *mode);
int slot_file_fclose(slot_file_t *file);
int slot_file_remove(slot_file_t *file);

int slot_children_named(slot_file_t *children,
                        slot_t *parent,
                        uint division);
int slot_children_fopen(slot_file_t *children,
                        const char *mode,
                        uint division);
int slot_children_fclose(slot_file_t *children,
                         uint division);
int slot_children_remove(slot_file_t *children,
                         uint division);

#ifdef __cplusplus
}
#endif
#endif /* SLOT_H */
