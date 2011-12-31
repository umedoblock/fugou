/* bitkaiten_h_si.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

unt bitkaiten_h_si(unt kaitenmoto, int j)
{
#ifdef ___ASSEMBLE_WIN32___
	_asm{
		mov cl, byte ptr [j]
		mov eax, [kaitenmoto]
		rol eax, cl
	}
#else
	unt w;

	w = kaitenmoto >> (32-j);
	return (((kaitenmoto) << j) | w);
#endif
}
