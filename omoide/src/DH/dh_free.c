/* dh_free.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "DH.h"

/*2005/04/10*/
void dh_pubfree(DH_PUBLICKEY *pubkey)
{
	free(pubkey->p);
}

/*2005/04/10*/
void dh_prifree(DH_PRIVATEKEY *prikey)
{
	int i;
	unt *p;
	extern int yousosuu;

	p = prikey->p;	

	for(i=0;i<3*yousosuu;i++){
		*p = 0;
		p++;
	}

	p = NULL;
	free(prikey->p);
}

/*2005/04/10*/
void dh_shrfree(DH_SHAREKEY *shrkey)
{
	int i;
	unt *p;
	extern int yousosuu;

	p = shrkey->y;	

	for(i=0;i<3*yousosuu;i++){
		*p = 0;
		p++;
	}

	p = NULL;
	free(shrkey->y);
}

/*2005/04/11*/
void dh_free(DH_PUBLICKEY *pubkey, DH_PRIVATEKEY *prikey, DH_SHAREKEY *shrkey)
{
	if(pubkey != NULL){
		dh_pubfree(pubkey);
	}
	if(prikey != NULL){
		dh_prifree(prikey);
	}
	if(shrkey != NULL){
		dh_shrfree(shrkey);
	}

}


