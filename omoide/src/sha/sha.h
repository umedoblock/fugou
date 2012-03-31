/* sha.h
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#ifndef __SHA_H__
#define __SHA_H__


#ifdef __cplusplus
extern "C"{
#endif


#ifndef _UNICODE
#define _UNICODE
#endif

#include <stdio.h>

#ifndef __TYPEDEF_UNSIGNED__
#define __TYPEDEF_UNSIGNED__
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int unt;
#endif /*__TYPEDEF_UNSIGNED__*/

typedef struct {
	unt hash[8][2];
} SHA512;

#define sha_add_64(z, x, y); *(z)=*(x)+*(y);*((z)+1)=*((x)+1)+*((y)+1)+(*(z)<*(y));
#define sha_copy_64(z, x); *(z)=*(x);*((z)+1)=*((x)+1);
#define sha_xor_64(z, x, y); *(z)=*(x)^*(y);*((z)+1)=*((x)+1)^*((y)+1);
#define sha_swap_64(x, y); sha_xor(x, x, y);sha_xor(y, x, y);sha_xor(x, x, y);sha_xor(y, x, y);

void sha512_init(SHA512 *H, unt *bits);
void sha512(SHA512 *H, uchar *data, SHA512 *HH);
int sha512_padding(uchar *pad, int flg, unt n, unt *bits);

void sha512_FileName(SHA512 *H, char *filename);
void sha512_FileData(SHA512 *H, FILE *fin, unt Byte);
void sha512_Data(SHA512 *H, uchar *data, unt Byte);

void sha512_clear(SHA512 *H);
void sha512_randomData(uchar *data, unt Byte, SHA512 H);
void sha512_hashhash(SHA512 *x, SHA512 *y);
void sha512_hashtodata(uchar *data, SHA512 *x);
void sha512_randomFile(char *filename, unt Byte, SHA512 H);
int sha512_equal(SHA512 a, SHA512 b);
void sha512_puthash(SHA512 a);
void sha512_RandomData(uchar *out, unt outsize, uchar *seed, unt seedsize);

void sha384_init(SHA512 *H, unt *bits);
void sha384_file_name(SHA512 *H, char *filename);
void sha384_file_Byte(SHA512 *H, FILE *fin, unt Byte);
void sha384_Byte(SHA512 *H, uchar *data, unt Byte);


void sha1_file(unt *Hash, char *filename);
void sha256_file(unt *Hash, char *filename);


void sha1_str(unt *Hash, char *ss);
void sha256_str(unt *Hash, char *ss);

/* sha_func.c */
unt sha_ch_32(unt x, unt y, unt z);
unt sha_parity_32(unt x, unt y, unt z);
unt sha_maj_32(unt x, unt y, unt z);
unt sha_ROTR_32(unt x, int bLen);
unt sha_ROTL_32(unt x, int bLen);
unt sha_SIGMA0_32(unt x);
unt sha_SIGMA1_32(unt x);
unt sha_sigma0_32(unt x);
unt sha_sigma1_32(unt x);

void sha_ch_64(unt *w, unt *x, unt *y, unt *z);
void sha_maj_64(unt *w, unt *x, unt *y, unt *z);
void sha_ROTL_64(unt *w, unt *x, int bLen);
void sha_ROTR_64(unt *w, unt *x, int bLen);
void sha_SHR_64(unt *w, unt *x, int bLen);
void sha_SIGMA0_64(unt *w, unt *x);
void sha_SIGMA1_64(unt *w, unt *x);
void sha_sigma0_64(unt *w, unt *x);
void sha_sigma1_64(unt *w, unt *x);

void putb(unt *x, int bLen);
void sha_puthash(unt *x, int hLen);

//酷い酷い酷い酷い酷い酷いSHAは全部酷い
//平成20年  1月 31日 木曜日 22:37:52
/* 改めてそう思いました。
 * 平成24年  3月 31日 土曜日 11:46:52
 */
/* 思うだけでは無責任なので特に酷いsha1()を直しておきました。
 * 最初、これ本当に俺が書いたか？と疑いましたが、
 * 何となく見覚えがあったので私が書いたのだと認めました。
 * 平成24年  3月 31日 土曜日 16:50:48
 */
int sha(uchar *hash, uchar *data, unt datasize, int shatype);
int sprintfsha(char *ss, uchar *data, unt datasize, int shatype);
void sharandom(uchar *random, unt randomsize, uchar *seed, unt seedsize);
#ifdef __cplusplus
}
#endif

#endif

