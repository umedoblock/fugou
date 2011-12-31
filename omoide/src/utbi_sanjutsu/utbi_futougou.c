/* utbi_futougou.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_futougou(unt *iroha_futougou, unt *nihohe_futougou)
{
extern int yousosuu;
#ifdef ___ASSEMBLE_WIN32___
	_asm{
			mov edx, dword ptr [iroha_futougou]
			mov esi, dword ptr [nihohe_futougou]

			mov ecx, [yousosuu]
			shl ecx, 2
			add edx, ecx
			add esi, ecx
			shr ecx, 2

		loop0:
			lea edx, [edx - 4]
			lea esi, [esi - 4]
			mov eax, dword ptr [edx]
			cmp eax, dword ptr [esi]
			jne loop_end
			loop short loop0
		loop_end:
			jb loop_mi
			jz loop_equal
			mov eax, 1
			jmp loop0000
		loop_mi:
			or eax, 0ffffffffh
			jmp loop0000
		loop_equal:
			xor eax, eax
		loop0000:
	}
#else
	int i;

	iroha_futougou += (yousosuu - 1);
	nihohe_futougou += (yousosuu - 1);

	for(i=0; (*iroha_futougou
			== *nihohe_futougou) && (i < (yousosuu-1)); i++){
		iroha_futougou--;
		nihohe_futougou--;
	}

	if(*iroha_futougou > *nihohe_futougou){
		return 1;
	}else if(*iroha_futougou < *nihohe_futougou){
		return -1;
	}else{
		return 0;
	}
#endif
}
