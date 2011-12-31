/* camellia_padding.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"
#include <time.h>

void camellia_iv(uchar *iv)
{
	camellia_padding(iv, CM_BLOCKSIZE);
}

void camellia_padding(uchar *pad, int nByte)
{
	int i;

	for(i=0;i<nByte;i++){
		pad[i] = 0x77;
	}
}

