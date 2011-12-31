/* camellia_keycopy.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

void camellia_keycopy(CAMELLIA_KEY *cka, CAMELLIA_KEY *ckb)
{
	int i;

	cka->keysize = ckb->keysize;
	for(i=0;i<4;i++){
		camellia_copy(cka->key[i], ckb->key[i]);
	}
	for(i=0;i<8;i++){
		camellia_copy(cka->subkey[i], ckb->subkey[i]);
	}
	for(i=0;i<34;i++){ 
		camellia_copy(cka->subkey_all[i], ckb->subkey_all[i]);
	}
}

