#include "libfugou_base.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
#define PACKAGE_NAME "libfugou"
#define PACKAGE_STRING "libfugou 0.0.0"
*/
#include <stdarg.h>

/*****************************************************************************/
/* for libfugou **************************************************************/
/*****************************************************************************/
void set_logger(FILE *log);
void set_logger_level(int log_level);
void logger(char *log_name, int level, char *fmt, ...);

void bury_memory(void *ptr, size_t length);
void dump(void *ptr, int length, int width);

/*****************************************************************************/
/* for CAMELLIA **************************************************************/
/*****************************************************************************/
#include "camellia/camellia.h"

/*****************************************************************************/
/* for SHA *******************************************************************/
/*****************************************************************************/
#include "sha/sha.h"

/*****************************************************************************/
/* for LIBFUGOU **************************************************************/
/*****************************************************************************/
#include "sha/sha.h"

/*2005/03/25*/
/* omoide/src/base64/base64.h */

unt base64_encode(char *ss, uchar *data, unt n);
unt base64_decode(uchar *data, char *ss);
int base64_equal(uchar *x, uchar *y, unt n);

/* omoide/src/base64/ss.c */
void strtodata(uchar *data, char *ss, unt n);
void datatostr(char *ss, uchar *data, unt n);

#ifdef __cplusplus
}
#endif
