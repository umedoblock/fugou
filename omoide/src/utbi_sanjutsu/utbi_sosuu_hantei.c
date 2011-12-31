/* utbi_sosuu_hantei.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_sosuu_hantei(unt *p, int kaisuu)
{

	if(utbi_sosuu_maeshori(p)){
		if(utbi_sosuu_miller_rabin(p, kaisuu)){
			return 1;
		}
	}


	return 0;
}
