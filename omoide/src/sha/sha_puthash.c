/* sha_puthash.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "sha.h"
#include <stdio.h>

void sha_puthash(unt *x, int hLen)
{
	int i, j;

	switch(hLen){
		case 160:
            for(i=(hLen-1)/32;i>=0;i--){
				fprintf(stdout, "%08x ", *(x+i));
			}
			break;
		case 256:
			for(i=(hLen-1)/32,j=0;i>=0;i--,j++){
				fprintf(stdout, "%08x ", *(x+i));
			}
			break;
 		case 384:
		case 512:
		default:
			for(i=(hLen-1)/32,j=0;i>=1;i-=2,j++){
				fprintf(stdout, "%08x%08x ", *(x+i), *(x+i-1));
			}
			break;
	}
}
