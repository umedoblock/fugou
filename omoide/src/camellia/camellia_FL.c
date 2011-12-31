/* camellia_FL.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"
#ifdef UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
void camellia_FL(unt *x, unt *x, unt *y)
{
	cm_tmp = *(x+1) & *(y+1);
	*(z) = ((cm_tmp << 1) | (cm_tmp >> 31)) ^ *(x);
	*(z+1) = ( *(z) | *(y)) ^ *(x+1);



}
#endif

