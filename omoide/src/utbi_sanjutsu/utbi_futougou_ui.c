/* utbi_futougou_ui.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_futougou_ui(unt *iroha_futougou, unt nihohe_futougou)
{
	extern int yousosuu;
#ifdef ___ASSEMBLE_WIN32___
	_asm{
			mov edx, dword ptr [iroha_futougou]

			mov ecx, [yousosuu]
			shl ecx, 2
			add edx, ecx
			shr ecx, 2

			lea edx, [edx - 4]
			dec ecx

		loop0:
			cmp dword ptr [edx], 0
			jne loop_cmp
			lea edx, [edx - 4]
			loop short loop0

			mov eax, dword ptr [edx]
            cmp eax, [nihohe_futougou]
		loop_cmp:
			jb loop_mi
			jz loop_equal
			mov eax, 1
			jmp loop0000
		loop_equal:
			xor eax, eax
			jmp loop0000
		loop_mi:
			or eax, 0ffffffffh

		loop0000:
	}
#else
			
	int i;

	for(i=yousosuu-1; *(iroha_futougou+i)==0 && i>0;i--){
	}

	if(i != 0){
		return 1;
	}else{
		if(*iroha_futougou > nihohe_futougou){
			return 1;
		}else if(*iroha_futougou < nihohe_futougou){
			return -1;
		}else{
			return 0;
		}
	}
#endif
}
