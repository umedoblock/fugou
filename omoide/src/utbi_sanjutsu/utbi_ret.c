/* utbi_ret.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_ret(void)
{
	extern int yousosuu;

	return yousosuu;
}
unt *utbi_retaddr(unt *base, int num)
{
	extern int yousosuu;

	return base + (num * yousosuu);
}
