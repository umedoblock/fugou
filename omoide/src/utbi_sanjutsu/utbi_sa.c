/* utbi_sa.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

/*2005/02/09*/
void utbi_sa(unt *z, unt *x, unt *y)
{
	extern int yousosuu;
#ifdef  ___ASSEMBLE_WIN32___
	_asm{
			mov edx, dword ptr [x]
			mov esi, dword ptr [y]
			mov edi, dword ptr [z]
			mov ecx, [yousosuu]
			clc

loop0:
			mov eax, dword ptr [edx]
			sbb eax, dword ptr [esi]
			mov dword ptr [edi], eax
			lea edx, [edx + 4]
			lea esi, [esi + 4]
			lea edi, [edi + 4]
			loop short loop0

	}


#else /*___ASSEMBLE_WIN32___*/
	int i, c0, c1;

	c0 = (*x < *y);
	*z = *x - *y;
	x++;
	y++;
	z++;

	i = 0;

	for(i=1;i<yousosuu;i++){
		c1 = (*x < *y);
		*z = *x - *y - c0;
		c0 = c1 || (!(~(*z)) && c0);

		/*#ifdef UNCHI
		*z = *x - *y;
		c1 = c1 || (!(*z) && c0);
		*z -= c0;
		c0 = c1;
		#endif*/

		x++;
		y++;
		z++;
	}
#endif
}
