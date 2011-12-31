/* ctoi.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

unt ctoi(unt atai, uchar x)
{
	atai = (atai << 8);

	atai |= x;

	return atai;
}
