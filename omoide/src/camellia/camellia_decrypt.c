/* camellia_decrypt.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

void camellia_decrypt(uchar *m, uchar *c, CAMELLIA_KEY *cmDEkey)
{

#ifdef ___ASSEMBLE32___
	extern unsigned int SP32[1024];
	unsigned int *p = SP32;

	_asm{
			/*暗号分のアドレス記憶*/
			mov edi, [c]
			mov eax, dword ptr [edi]
			mov ebx, dword ptr [edi+4]
			mov ecx, dword ptr [edi+8]
			mov edx, dword ptr [edi+12]
#ifdef ___BIGENDIAN___
			bswap eax
			bswap ebx
			bswap ecx
			bswap edx
#endif
			/*副鍵のアドレスを取得。*/
			mov esi, cmDEkey
			add esi, 60h
			/*平文のアドレス取得。*/
			mov edi, [m]


/********************************************************************************/
			/*kw3, kw4*/
			xor eax, dword ptr [esi+64*4]
			xor ebx, dword ptr [esi+65*4]
			xor ecx, dword ptr [esi+66*4]
			xor edx, dword ptr [esi+67*4]
/********************************************************************************/
			/*L23, R23*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k24(64)*/
			xor eax, dword ptr [esi+62*4]
			xor ebx, dword ptr [esi+63*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax

			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi
/********************************************************************************/
			/*L22, R22*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k23(64)*/
			xor eax, dword ptr [esi+60*4]
			xor ebx, dword ptr [esi+61*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L21, R21*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k22(64)*/
			xor eax, dword ptr [esi+58*4]
			xor ebx, dword ptr [esi+59*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi
/********************************************************************************/
			/*L20, R20*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k21(64)*/
			xor eax, dword ptr [esi+56*4]
			xor ebx, dword ptr [esi+57*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi
/********************************************************************************/
			/*L19, R19*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k20(64)*/
			xor eax, dword ptr [esi+54*4]
			xor ebx, dword ptr [esi+55*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi
/********************************************************************************/
			/*L18, R18*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k19(64)*/
			xor eax, dword ptr [esi+52*4]
			xor ebx, dword ptr [esi+53*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi
/********************************************************************************/
			push edi

			/*FL, FL^(-1)*/
			/*kL5, kL6(64)*/

			/*FL*/
			mov edi, eax
			and edi, dword ptr [esi+50*4]
			rol edi, 1
			xor ebx, edi
			mov edi, dword ptr [esi+51*4]
			or  edi, ebx
			xor eax, edi

			/*FL^(-1)*/
			mov edi, dword ptr [esi+49*4]
			or  edi, edx
			xor ecx, edi
			mov edi, ecx
			and edi, dword ptr [esi+48*4]
			rol edi, 1
			xor edx, edi

			pop edi

/********************************************************************************/
			/*L17, R17*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k18(64)*/
			xor eax, dword ptr [esi+46*4]
			xor ebx, dword ptr [esi+47*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L16, R16*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k17(64)*/
			xor eax, dword ptr [esi+44*4]
			xor ebx, dword ptr [esi+45*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L15, R15*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k16(64)*/
			xor eax, dword ptr [esi+42*4]
			xor ebx, dword ptr [esi+43*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L14, R14*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k15(64)*/
			xor eax, dword ptr [esi+40*4]
			xor ebx, dword ptr [esi+41*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L13, R13*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k14(64)*/
			xor eax, dword ptr [esi+38*4]
			xor ebx, dword ptr [esi+39*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L12, R12*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k13(64)*/
			xor eax, dword ptr [esi+36*4]
			xor ebx, dword ptr [esi+37*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			push edi

			/*FL, FL^(-1)*/
			/*kL3, kL4(64)*/

			/*FL*/
			mov edi, eax
			and edi, dword ptr [esi+34*4]
			rol edi, 1
			xor ebx, edi
			mov edi, dword ptr [esi+35*4]
			or  edi, ebx
			xor eax, edi

			/*FL^(-1)*/
			mov edi, dword ptr [esi+33*4]
			or  edi, edx
			xor ecx, edi
			mov edi, ecx
			and edi, dword ptr [esi+32*4]
			rol edi, 1
			xor edx, edi

			pop edi

/********************************************************************************/
			/*L11, R11*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k12(64)*/
			xor eax, dword ptr [esi+30*4]
			xor ebx, dword ptr [esi+31*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L10, R10*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k11(64)*/
			xor eax, dword ptr [esi+28*4]
			xor ebx, dword ptr [esi+29*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L9, R9*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k10(64)*/
			xor eax, dword ptr [esi+26*4]
			xor ebx, dword ptr [esi+27*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L8, R8*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k9(64)*/
			xor eax, dword ptr [esi+24*4]
			xor ebx, dword ptr [esi+25*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L7, R7*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k8(64)*/
			xor eax, dword ptr [esi+22*4]
			xor ebx, dword ptr [esi+23*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L6, R6*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k7(64)*/
			xor eax, dword ptr [esi+20*4]
			xor ebx, dword ptr [esi+21*4]

			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			push edi

			/*FL, FL^(-1)*/
			/*kL1(64)*/

			/*FL*/
			mov edi, eax
			and edi, dword ptr [esi+18*4]
			rol edi, 1
			xor ebx, edi
			mov edi, dword ptr [esi+19*4]
			or  edi, ebx
			xor eax, edi

			/*FL^(-1)*/
			mov edi, dword ptr [esi+17*4]
			or  edi, edx
			xor ecx, edi
			mov edi, ecx
			and edi, dword ptr [esi+16*4]
			rol edi, 1
			xor edx, edi

			pop edi

/********************************************************************************/
			/*L5, R5*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k6(64)*/
			xor eax, dword ptr [esi+14*4]
			xor ebx, dword ptr [esi+15*4]
			
			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L4, R4*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k5(64)*/
			xor eax, dword ptr [esi+12*4]
			xor ebx, dword ptr [esi+13*4]
			
			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L3, R3*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k4(64)*/
			xor eax, dword ptr [esi+10*4]
			xor ebx, dword ptr [esi+11*4]
			
			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L2, R2*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k2(64)*/
			xor eax, dword ptr [esi+8*4]
			xor ebx, dword ptr [esi+9*4]
			
			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L1, R1*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k2(64)*/
			xor eax, dword ptr [esi+6*4]
			xor ebx, dword ptr [esi+7*4]
			
			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor eax, ecx
			xor ebx, edx

			pop edx
			pop ecx
			pop esi

/********************************************************************************/
			/*L0, R0*/
			push esi
			push eax
			push ebx
			push ecx
			push edx

			/*k1(64)*/
			xor eax, dword ptr [esi+4*4]
			xor ebx, dword ptr [esi+5*4]
			
			/*SP32のアドレス*/
			/*esi = &SP32[0]*/
			mov esi, p

			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx

			xor ecx, ecx
			xor edx, edx

#ifdef ___BIGENDIAN___
			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]


#else
			mov cl, byte ptr [edi+0]
			mov dl, byte ptr [edi+4+0]
			mov eax, dword ptr [edx*4+esi+256*1*4]
			mov ebx, dword ptr [ecx*4+esi+256*0*4]

			mov cl, byte ptr [edi+1]
			mov dl, byte ptr [edi+4+1]
			xor eax, dword ptr [edx*4+esi+256*2*4]
			xor ebx, dword ptr [ecx*4+esi+256*1*4]

			mov cl, byte ptr [edi+2]
			mov dl, byte ptr [edi+4+2]
			xor eax, dword ptr [edx*4+esi+256*3*4]
			xor ebx, dword ptr [ecx*4+esi+256*2*4]

			mov cl, byte ptr [edi+3]
			mov dl, byte ptr [edi+4+3]
			xor eax, dword ptr [edx*4+esi+256*0*4]
			xor ebx, dword ptr [ecx*4+esi+256*3*4]

#endif
			xor eax, ebx
			pop edx
			ror ebx, 8
			pop ecx
			xor ebx, eax


			xor ecx, eax
			xor edx, ebx

			pop ebx
			pop eax
			pop esi

/********************************************************************************/
			/*kw1, kw2*/
			xor eax, dword ptr [esi+0*4]
			xor ebx, dword ptr [esi+1*4]
			xor ecx, dword ptr [esi+2*4]
			xor edx, dword ptr [esi+3*4]
/********************************************************************************/
#ifdef ___BIGENDIAN___
			bswap eax
			bswap ebx
			bswap ecx
			bswap edx
#endif
			mov edi, [m]
			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx
			mov dword ptr [edi+8], ecx
			mov dword ptr [edi+12], edx
	}
#else
	unt w[2], tmpL[2], tmpR[2];
	unt C[2][2];
	uchar *mp, *cp;

	cp = mp = (uchar *)C[0] + 15;

	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;
	*cp=*c;cp--;c++;*cp=*c;cp--;c++;

	/*復号では逆から副鍵を使う*/

	if(cmDEkey->keysize == 128){
		camellia_xor(tmpL, C[0], cmDEkey->subkey_all[25]);
		camellia_xor(tmpR, C[1], cmDEkey->subkey_all[24]);
	}else{
		/*********(keysize==192||keysize==256)*********/
		camellia_xor(tmpL, C[0], cmDEkey->subkey_all[33]);
		camellia_xor(tmpR, C[1], cmDEkey->subkey_all[32]);

		camellia_copy(w, tmpR);
		camellia_F(tmpR, tmpR, cmDEkey->subkey_all[31]);
		camellia_xor(tmpR, tmpR, tmpL);
		camellia_copy(tmpL, w);

		camellia_copy(w, tmpR);
		camellia_F(tmpR, tmpR, cmDEkey->subkey_all[30]);
		camellia_xor(tmpR, tmpR, tmpL);
		camellia_copy(tmpL, w);

		camellia_copy(w, tmpR);
		camellia_F(tmpR, tmpR, cmDEkey->subkey_all[29]);
		camellia_xor(tmpR, tmpR, tmpL);
		camellia_copy(tmpL, w);

		camellia_copy(w, tmpR);
		camellia_F(tmpR, tmpR, cmDEkey->subkey_all[28]);
		camellia_xor(tmpR, tmpR, tmpL);
		camellia_copy(tmpL, w);

		camellia_copy(w, tmpR);
		camellia_F(tmpR, tmpR, cmDEkey->subkey_all[27]);
		camellia_xor(tmpR, tmpR, tmpL);
		camellia_copy(tmpL, w);

		camellia_copy(w, tmpR);
		camellia_F(tmpR, tmpR, cmDEkey->subkey_all[26]);
		camellia_xor(tmpR, tmpR, tmpL);
		camellia_copy(tmpL, w);


		camellia_FL(tmpR, cmDEkey->subkey_all[25]);
		
		camellia_FL_1(tmpL, cmDEkey->subkey_all[24]);
		
	}

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[23]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[22]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[21]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[20]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[19]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[18]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);


	camellia_FL(tmpR, cmDEkey->subkey_all[17]);
	
	camellia_FL_1(tmpL, cmDEkey->subkey_all[16]);
	


	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[15]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[14]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[13]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[12]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[11]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[10]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_FL(tmpR, cmDEkey->subkey_all[9]);
	
	camellia_FL_1(tmpL, cmDEkey->subkey_all[8]);
	

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[7]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[6]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[5]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[4]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[3]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_copy(w, tmpR);
	camellia_F(tmpR, tmpR, cmDEkey->subkey_all[2]);
	camellia_xor(tmpR, tmpR, tmpL);
	camellia_copy(tmpL, w);

	camellia_xor(C[0], tmpR, cmDEkey->subkey_all[1]);
	camellia_xor(C[1], tmpL, cmDEkey->subkey_all[0]);

	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;
	*m=*mp;mp--;m++;*m=*mp;mp--;m++;

	/*ローカル変数の値を消去*/
	camellia_zero(tmpL);
	camellia_zero(tmpR);
	camellia_zero(w);
	camellia_zero(C[0]);
	camellia_zero(C[1]);
#endif

}
