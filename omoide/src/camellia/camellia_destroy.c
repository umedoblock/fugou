/* camellia_destroy.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

void camellia_destroy(CAMELLIA_KEY *ck)
{
	unt *p;
	int i;

	ck->keysize = 0;
	p = ck->key[0];
	for(i=0;i<8;i++){
		*p = 0;
		p++;
	}
	p = ck->subkey[0];
	for(i=0;i<16;i++){
		*p = 0;
		p++;
	}
	p = ck->subkey_all[0];
	for(i=0;i<68;i++){
		*p = 0;
		p++;
	}
}

