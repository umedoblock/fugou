/* dh_malloc.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "DH.h"

/*2005/04/10*/
int dh_pubmalloc(DH_PUBLICKEY *pubkey)
{
	extern int yousosuu;
	utbi_bitset(pubkey->pLen);
	if(utbi_memory(&pubkey->p, 3)==0){
		exit(1);
	}

	pubkey->g = pubkey->p + yousosuu;
	pubkey->gx = pubkey->g + yousosuu;
	
	return 1;
}
/*2005/04/10*/
int dh_primalloc(DH_PRIVATEKEY *prikey)
{
	extern int yousosuu;
	utbi_bitset(prikey->pLen);
	if(utbi_memory(&prikey->p, 3)==0){
		exit(1);
	}

	prikey->g = prikey->p + yousosuu;
	prikey->x = prikey->g + yousosuu;

	return 1;
}
/*2005/04/10*/
int dh_shrmalloc(DH_SHAREKEY *shrkey)
{
	extern int yousosuu;
	if(utbi_memory(&shrkey->y, 3)==0){
		exit(1);
	}

	shrkey->gy = shrkey->y + yousosuu;
	shrkey->gxy = shrkey->gy + yousosuu;

	return 1;
}

/*2005/04/11*/
int dh_malloc(DH_PUBLICKEY *pubkey, DH_PRIVATEKEY *prikey, DH_SHAREKEY *shrkey)
{
	int flg = 0;
	if(pubkey != NULL){
		flg |= ((dh_pubmalloc(pubkey)) << 2);
	}
	if(prikey != NULL){
		flg |= ((dh_primalloc(prikey)) << 1);
	}
	if(shrkey != NULL){
		flg |= dh_shrmalloc(shrkey);
	}

	return flg;
}



