/* utbi_ior.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_ior(unt *mdr_ior, unt *ior_iroha, unt *ior_nihohe)
{
	int i;
	extern int yousosuu;

	for(i=0; i<yousosuu; i++){
		*mdr_ior = *ior_iroha | *ior_nihohe;
		mdr_ior++;
		ior_iroha++;
		ior_nihohe++;
	}
}
void utbi_or(unt *mdr_or, unt *or_iroha, unt *or_nihohe)
{
	utbi_ior(mdr_or, or_iroha, or_nihohe);
}

