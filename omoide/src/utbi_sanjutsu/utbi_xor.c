/* utbi_xor.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_xor(unt *mdr_xor, unt *xor_iroha, unt *xor_nihohe)
{
	int i;
	extern int yousosuu;

	for(i=0; i<yousosuu; i++){
		*mdr_xor = *xor_iroha ^ *xor_nihohe;
		mdr_xor++;
		xor_iroha++;
		xor_nihohe++;
	}
}
