/* base64.h
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

/*2005/03/25*/
/*base64.h*/
#ifndef __BASE64__
#define __BASE64__


#ifdef __cplusplus
extern "C"{
#endif

#ifndef _UNICODE
#define _UNICODE
#endif /*_UNICODE*/

#include <string.h>

#ifndef __TYPEDEF_UNSIGNED__
#define __TYPEDEF_UNSIGNED__
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int unt;
#endif /*__TYPEDEF_UNSIGNED__*/

unt base64_encode(char *ss, uchar *data, unt n);
unt base64_decode(uchar *data, char *ss);

int base64_equal(uchar *x, uchar *y, unt n);

void strtodata(uchar *data, char *ss, unt n);
void datatostr(char *ss, uchar *data, unt n);

#ifdef __cplusplus
}
#endif

#endif /*__BASE64__*/
