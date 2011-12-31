/* utbi_bitzurashi_m.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_bitzurashi_m(unt *y, unt *x)
{
	extern int yousosuu;
#ifdef ___ASSEMBLE_WIN32___
	_asm{
			mov edx, dword ptr [x]
			mov esi, dword ptr [y]
			mov ecx, [yousosuu]
			shl ecx, 2
			lea edx, [edx+ecx-4]
			lea esi, [esi+ecx-4]
			shr ecx, 2
			clc
		loop0:
			mov eax, dword ptr [edx]
			rcr eax, 1
			mov dword ptr [esi], eax
			lea edx, [edx-4]
			lea esi, [esi-4]
			loop short loop0
	}
#else
	int i;
	utbi_fukusha(y, x);

	for(i=0; i<yousosuu - 1; i++){
		*y = (*(x+1) << 31) | (*x >> 1);

		x++;
		y++;
	}
	*y = *x >> 1;
#endif
}
