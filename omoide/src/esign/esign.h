/* esign.h
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#ifndef __ESIGN_H__
#define __ESIGN_H__


#ifdef __cplusplus
extern "C"{
#endif

#include "../utbi_sanjutsu\utbi_sanjutsu.h"
#include "../sha\sha.h"
#define INVALID 0
#define GOOD 1

typedef struct {
	unt pLen;
	unt *n;
	unt e;
} ESIGN_PUBLICKEY;

typedef struct {
	unt pLen;
	unt *p;
	unt *q;
	unt *pq;
	unt *n;
	unt e;
} ESIGN_PRIVATEKEY;

typedef struct {
	unt *s;
	unt lLen;
	FILE *fm;
	wchar_t fname[80];
	SHA512 H;
	unt hLen;
} ESIGN_SIGN;

typedef struct {
	unt *s;
	unt lLen;
	FILE *fm;
	wchar_t fname[80];
	SHA512 H;
	unt hLen;
} ESIGN_VERIFY;

/*2004/11/28*/
int esign_kagiseisei(ESIGN_PUBLICKEY *pubkey, ESIGN_PRIVATEKEY *prikey, unt k, unt op);
/*2004/12/04*/
/************************署名関係*************************/
int ssa_esign_sign(ESIGN_SIGN *sign, ESIGN_PRIVATEKEY *prikey);
int emsa_esign_encode(unt *f, ESIGN_SIGN *sign);
int SP_esign(ESIGN_SIGN *sign, ESIGN_PRIVATEKEY *prikey, unt *f);
/*2004/12/04*/
/************************検証関係*************************/
int ssa_esign_verify(ESIGN_VERIFY *verify, ESIGN_PUBLICKEY *pubkey);
int VP_esign(unt *f_, ESIGN_VERIFY *verify, ESIGN_PUBLICKEY *pubkey);
int emsa_esign_verify(ESIGN_VERIFY *verify, unt *f_);
/*2005/04/09*/
/*********************入出力検証関係**********************/
void esign_PubkeyFout(FILE *fpub, ESIGN_PUBLICKEY pubkey);
void esign_PrikeyFout(FILE *fpri, ESIGN_PRIVATEKEY prikey);
int esign_PubkeyFin(ESIGN_PUBLICKEY *pubkey, FILE *fpub);
int esign_PrikeyFin(ESIGN_PRIVATEKEY *prikey, FILE *fpri);
int esign_VerifyFin(ESIGN_VERIFY *verify, FILE *fin);
void esign_SignFout(FILE *fout, ESIGN_SIGN sign);

#ifdef __cplusplus
}
#endif

#endif