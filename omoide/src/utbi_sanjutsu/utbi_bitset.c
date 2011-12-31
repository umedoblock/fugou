/* utbi_bitset.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int yousosuu;

void utbi_bitset(int bLen)
{
	yousosuu = ((bLen-1)/32)+1;
}
