/* camellia_cBytes.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

unt camellia_cBytes(unt nByte)
{
	nByte &= 0xfffffff0;
	nByte += 2*CM_BLOCKSIZE;
	return nByte;
}
