/* utbi_wa.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"
#define __WA_NEW__

#ifdef __WA_NEW__
//double wa_t = 0.0;
/*2005/02/09*/
void utbi_wa(unt *z, unt *x, unt *y)
{
/*
	clock_t s, e;
	s = clock();
*/
	extern int yousosuu;


#ifndef  ___ASSEMBLE_WIN32___
	int i, tmp;
	unt c0;
#endif
	if(x == y){
		utbi_bitzurashi_h(z, x);
	}else{
#ifdef  ___ASSEMBLE_WIN32___
		if(z != x){
			memcpy(z, x, sizeof(unt)*yousosuu);
		}






	_asm{
//			mov edx, dword ptr [x]
			mov esi, dword ptr [y]
			mov edi, dword ptr [z]
			mov ecx, [yousosuu]

			clc

		loop1:
			mov eax, dword ptr [esi]
			lea esi, [esi + 4]
			adc dword ptr [edi], eax
			lea edi, [edi + 4]
			loop short loop1
		}
	fprintf(stdout, "testes\n");


#else /*___ASSEMBLE_WIN32___*/

	tmp = yousosuu;

	*z = *x + *y;
	c0 = (*z < *y);
	x++;
	y++;
	z++;

	for(i=1;i<tmp;i++){
		*z = *x + *y + c0;
		/*ここが今流行のバグ*/
		/*c0 = (*z < *y) || (!(*z) && c0);*/
		c0 = (*z < *y) ||/* (!(*z) && c0) || */((*z == *y) && c0);
		/*c0 = (*z < *y) || !(*z) && c0 || ((!(~(*x))) && c0);*/
		/*c0 = (*z < *y) || c0 && (!(*z) || (!(~(*x))));*/
		x++;
		y++;
		z++;
	}
#endif /*___ASSEMBLE_WIN32___*/
	}
}
#endif /*__WA_NEW__*/
#ifdef __WA_OLD0__
void utbi_wa(unt *z, unt *x, unt *y)
{
	int ketaagari = 0, i, j;

	for(i=0; i<yousosuu; i++){

		*z = *x + *y;

		if(*z < *y){
			/*桁上がりした*/
			*z += ketaagari;
			ketaagari = 1;
		}else{
			*z += ketaagari;
			if(*z == 0 && ketaagari == 1){
				ketaagari = 1;
			}else{
				ketaagari = 0;
			}
		}

		x++;
		y++;
		z++;

	}
}
#endif/*__WA_OLD0__*/

