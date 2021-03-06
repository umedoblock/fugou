/* camellia.h
 * Copyright (C) 平成17-20(2005-2008)年 梅濁酒(umedoblock)
 */

#ifndef __CAMELLIA_H__
#define __CAMELLIA_H__
#include "../libfugou_base.h"

#include <memory.h>
#ifdef __cplusplus
extern "C"{
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>

#define CM_BLOCKSIZE (128/8) /* 不可侵, 過去の資産大事に */

#define CAMELLIA_BLOCK_BITS (128)
#define CAMELLIA_BLOCK_SIZE (CAMELLIA_BLOCK_BITS / OCTET)
#define CAMELLIA_NORMED_SIZE(s) (s + (CAMELLIA_BLOCK_SIZE - ((s % CAMELLIA_BLOCK_SIZE) ? s%CAMELLIA_BLOCK_SIZE: 0)))

#ifndef __CRYPT__
#define __CRYPT__
#define ENCRYPT 0
#define DECRYPT 1
#endif

typedef unsigned int unt;
typedef unsigned char uchar;

typedef struct {
	unt key[4][2];
	unt subkey[8][2];
	unt subkey_all[34][2];
	unt keysize;
} CAMELLIA_KEY;

/*
#define camellia_
_asm{
}
*/

#define _rotl(data, bit) (((data)<<(bit))|((data)>>(32-(bit))))
#define _rotr(data, bit) (((data)>>(bit))|((data)<<(32-(bit))))
#define camellia_copy(x, y); (*(x))=(*(y));(*(x+1))=(*(y+1));
#define camellia_xor(z, x, y); (*(z))=((*(x))^(*(y)));(*(z+1))=(*(x+1))^(*(y+1));
#define camellia_swap(x, y); camellia_xor(x,x,y);camellia_xor(y,x,y);camellia_xor(x,x,y);
#define camellia_not(x, y); (*(x))=~(*(y));(*(x+1))=~(*(y+1));
#define camellia_zero(x); (*(x))=(0);(*((x)+1))=(0);
#define camellia_FL(x, y); x[0]^=_rotl((x[1]&y[1]),1);x[1]^=(x[0]|y[0]);
#define camellia_FL_1(x, y); x[1]^=((x[0])|y[0]);x[0]^=_rotl((x[1]&y[1]),1);

void camellia_hkaiten(unt *hkaiten, int bLen);

void camellia_keyset(CAMELLIA_KEY *ck, uchar *data, int kLen);
void camellia_encrypt(uchar *c, uchar *m, CAMELLIA_KEY *cmENkey);
void camellia_decrypt(uchar *m, uchar *c, CAMELLIA_KEY *cmDEkey);

size_t camellia_encrypt_cbc(uchar *c,
                            uchar *m,
                            uchar *iv,
                            CAMELLIA_KEY *key,
                            size_t text_size);
size_t camellia_decrypt_cbc(uchar *m,
                            uchar *c,
                            CAMELLIA_KEY *key,
                            size_t cipher_size);

/****************************ENCRYPT*****************************/
int camellia_encrypt_cbc_DataData(uchar *c, uchar *m, uchar *iv, unt Byte, CAMELLIA_KEY *cmENkey);
int camellia_encrypt_cbc_FileFile(FILE *fc, FILE *fm, uchar *iv, ulong Hi, ulong Low, CAMELLIA_KEY *cmENkey);

/****************************DECRYPT*****************************/
unt camellia_decrypt_cbc_DataData(uchar *m, uchar *c, uchar *iv, unt Byte, CAMELLIA_KEY *cmDEkey);
unt camellia_decrypt_cbc_FileFile(FILE *fm, FILE *fc, uchar *iv, ulong Hi, ulong Low, CAMELLIA_KEY *cmDEkey);
/*************************DECRYPT&ENCRYPT************************/
uchar *camellia_decenc_cbc_block(uchar *newc, uchar *newiv, CAMELLIA_KEY *cmNEWkey, 
								 uchar *oldc, uchar *oldiv, CAMELLIA_KEY *cmOLDkey, 
								 unt blocks);
/****************************BASE64******************************/
int camellia_encrypt_cbc_ssBase64(char *base64, char *ss, CAMELLIA_KEY *cmENkey);
int camellia_decrypt_cbc_Base64ss(char *ss, char *base64, CAMELLIA_KEY *cmDEkey);
/*****************************DATA*******************************/
int camellia_encrypt_cbc_DataBase64(char *base64, uchar *data, unt nBytes, CAMELLIA_KEY *cmENkey);
unt camellia_decrypt_cbc_Base64Data(uchar *data, char *base64, CAMELLIA_KEY *cmDEkey);
/****************************************************************/

unt camellia_cBytes(unt nByte);

void camellia_subkey(unt subkey[][2], unt KEY[][2], int keysize);
void camellia_subkey_all(unt subkey_all[][2], unt subkey[][2], int keysize);
void camellia_keygen(CAMELLIA_KEY *ck);
void camellia_keycopy(CAMELLIA_KEY *cka, CAMELLIA_KEY *ckb);
void camellia_keyset(CAMELLIA_KEY *ck, uchar *data, int kLen);
//void camellia_hashset(CAMELLIA_KEY *ck, SHA512 *H);
void camellia_destroy(CAMELLIA_KEY *ck);

void camellia_padding(uchar *pad, int nByte);
void camellia_iv(uchar *iv);

void camellia_putx(unsigned int *x, int n);

#ifdef __cplusplus
}
#endif

#endif
