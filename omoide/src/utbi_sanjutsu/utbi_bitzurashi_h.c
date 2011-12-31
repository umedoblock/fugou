/* utbi_bitzurashi_h.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_bitzurashi_h(unt *y, unt *x)
{
	extern int yousosuu;
#ifdef ___ASSEMBLE_WIN32___
	_asm{
			mov edx, dword ptr [x]
			mov esi, dword ptr [y]
			mov ecx, [yousosuu]

			dec ecx
			shl ecx, 2
			add edx, ecx
			add esi, ecx
			shr ecx, 2
			
			cmp edx, esi
			je loop1

		loop0:
			mov eax, dword ptr [edx]
			mov ebx, dword ptr [edx-4]
			shl eax, 1
			shr ebx, 31
			lea edx, [edx-4]
			or eax, ebx
			mov dword ptr [esi], eax
			lea esi, [esi-4]
			loop short loop0

			mov eax, dword ptr [edx]
			shl eax, 1
			mov dword ptr [esi], eax
		jmp loop_end

		loop1:
			mov ebx, dword ptr [edx-4]
			shr ebx, 31
			shl dword ptr [edx], 1
			or dword ptr [edx], ebx
			lea edx, [edx-4]
			loop short loop1

			shl dword ptr [edx], 1

		loop_end:
	}
#else
	int i;

	if(y != x){
		utbi_fukusha(y, x);
	}

	y += (yousosuu - 1);

	for(i=yousosuu-1; i>0; i--){
		*y = (*y << 1) | (*(y -1) >> 31);
		y--;
	}
	*y = *y << 1;
#endif
}

void utbi_bitzurashi_h4(unt *y, unt *x)
{
	extern int yousosuu;
#ifdef ___ASSEMBLE_WIN32___
	_asm{
			mov edx, dword ptr [x]
			mov esi, dword ptr [y]
			mov ecx, [yousosuu]

			dec ecx
			shl ecx, 2
			add edx, ecx
			add esi, ecx
			shr ecx, 2
			
			cmp edx, esi
			je loop1

		loop0:
			mov eax, dword ptr [edx]
			mov ebx, dword ptr [edx-4]
			shl eax, 4
			shr ebx, 28
			lea edx, [edx-4]
			or eax, ebx
			mov dword ptr [esi], eax
			lea esi, [esi-4]
			loop short loop0

			mov eax, dword ptr [edx]
			shl eax, 4
			mov dword ptr [esi], eax
		jmp loop_end

		loop1:
			mov ebx, dword ptr [edx-4]
			shr ebx, 28
			shl dword ptr [edx], 4
			or dword ptr [edx], ebx
			lea edx, [edx-4]
			loop short loop1

			shl dword ptr [edx], 4

		loop_end:
	}
#else
	utbi_bitzurashi_h(y, x);
	utbi_bitzurashi_h(y, y);
	utbi_bitzurashi_h(y, y);
	utbi_bitzurashi_h(y, y);
#endif
}

void utbi_bitzurashi_h8(unt *y, unt *x)
{
	extern int yousosuu;
	if(x != y)
		memcpy(y, x, sizeof(unt)*yousosuu);
#ifdef ___ASSEMBLE_WIN32___
	_asm{
			mov ecx, [yousosuu]
			mov edx, dword ptr [x]
			mov esi, dword ptr [y]

			lea edx, [edx+4*ecx-4]
			lea esi, [esi+4*ecx-4]

			dec ecx
			

		loop1:
			mov ebx, dword ptr [edx-4]
			shr ebx, 32-8

			shl dword ptr [edx], 8
			or dword ptr [edx], ebx

			lea edx, [edx-4]
			loop short loop1

			shl dword ptr [edx], 8

	}

#endif
}
