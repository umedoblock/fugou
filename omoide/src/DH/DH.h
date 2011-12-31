/* DH.h
 * Copyright (C) 2008 梅どぶろく umedoblock
 */


#ifndef __DH_H__
#define __DH_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "../../utbi_sanjutsu/src/utbi_sanjutsu.h"
#include "../../sha/src/sha.h"

typedef struct{
	unt *p;
	unt *g;
	unt *gx;
	unt pLen;
} DH_PUBLICKEY;

typedef struct{
	unt *p;
	unt *g;
	unt *x;
	unt pLen;
} DH_PRIVATEKEY;

typedef struct{
	unt *y;
	unt *gy;
	unt *gxy;
} DH_SHAREKEY;

/*2005/04/11*/
int dh_malloc(DH_PUBLICKEY *pubkey, DH_PRIVATEKEY *prikey, DH_SHAREKEY *shrkey);
void dh_free(DH_PUBLICKEY *pubkey, DH_PRIVATEKEY *prikey, DH_SHAREKEY *shrkey);

/*2005/04/10*/
int dh_pubmalloc(DH_PUBLICKEY *pubkey);
/*2005/04/10*/
int dh_primalloc(DH_PRIVATEKEY *prikey);
/*2005/04/10*/
int dh_shrmalloc(DH_SHAREKEY *shrkey);

/*2005/04/10*/
void dh_pubfree(DH_PUBLICKEY *pubkey);
/*2005/04/10*/
void dh_prifree(DH_PRIVATEKEY *prikey);
/*2005/04/10*/
void dh_shrfree(DH_SHAREKEY *shrkey);

/*2005/04/10*/
void dh_priclear(DH_PRIVATEKEY *prikey);
/*2005/04/10*/
void dh_shrclear(DH_SHAREKEY *shrkey);

/*2005/04/02*/
void dh_keygen(DH_PUBLICKEY *pubkey, DH_PRIVATEKEY *prikey, unt mode);
/*2005/04/02*/
void dh_enkey(DH_SHAREKEY *shrkey, DH_PUBLICKEY *pubkey, unt mode);
/*2005/04/02*/
void dh_dekey(DH_SHAREKEY *shrkey, DH_PRIVATEKEY *prikey);
/*2005/04/11*/
int dh_keyhash(SHA512 *ks, DH_SHAREKEY *shrkey);
#ifdef __cplusplus
}
#endif

#endif /*__DH_H__*/
