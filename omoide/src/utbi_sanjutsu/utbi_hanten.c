/* utbi_hanten.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_hanten(unt *mdr_hanten, unt *iroha_hanten)
{
	int i;
	extern int yousosuu;

	utbi_fukusha(mdr_hanten, iroha_hanten);

	for(i=0;i<yousosuu;i++){
		*mdr_hanten = ~(*mdr_hanten);
		mdr_hanten++;
	}

}
