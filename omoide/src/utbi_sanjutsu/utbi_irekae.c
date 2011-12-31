/* utbi_irekae.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_irekae(unt *irekae_iroha, unt *irekae_nihohe)
{
	int i;
	extern int yousosuu;
	unt tmp;

	for(i=0;i<yousosuu;i++){
		tmp = *irekae_iroha;
		*irekae_iroha = *irekae_nihohe;
		*irekae_nihohe = tmp;
		irekae_iroha++;
		irekae_nihohe++;
	}
}
