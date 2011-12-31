/* utbi_and.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_and(unt *mdr_and, unt *and_iroha, unt *and_nihohe)
{
	int i;
	extern int yousosuu;

	for(i=0; i<yousosuu; i++){
		*mdr_and = *and_iroha & *and_nihohe;
		mdr_and++;
		and_iroha++;
		and_nihohe++;
	}
}
