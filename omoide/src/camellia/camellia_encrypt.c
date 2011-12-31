/* camellia_encrypt.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

extern unsigned int SP32[1024];
void camellia_encrypt(uchar *c, uchar *m, CAMELLIA_KEY *cmENkey)
{
#ifdef ___ASSEMBLE32___
{
	unsigned int *p = SP32;
	_asm{
/*
			xor eax, dword ptr [esi]
			xor ebx, dword ptr [esi+*4]
			xor ecx, dword ptr [esi+*4]
			xor edx, dword ptr [esi+*4]
*/
			/*平分のアドレス記憶*/
			mov edi, [m]
			/*平分をレジスタに複写*/
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
			/*
			eax = LL
			ebx = LR
			ecx = RL
			edx = RR
			esi = &subkey_all
			edi = sbox
			*/

			/*副鍵のアドレスを取得。*/
			mov esi, cmENkey
			add esi, 60h
			/*暗号文のアドレス取得。*/
			mov edi, [c]

/********************************************************************************/
			/*kw1, kw2*/
			xor eax, dword ptr [esi+0*4]
			xor ebx, dword ptr [esi+1*4]
			xor ecx, dword ptr [esi+2*4]
			xor edx, dword ptr [esi+3*4]

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
			push edi

			/*FL, FL^(-1)*/
			/*kL1(64)*/

			/*FL*/
			mov edi, eax
			and edi, dword ptr [esi+16*4]
			rol edi, 1
			xor ebx, edi
			mov edi, dword ptr [esi+17*4]
			or  edi, ebx
			xor eax, edi

			/*FL^(-1)*/
			mov edi, dword ptr [esi+19*4]
			or  edi, edx
			xor ecx, edi
			mov edi, ecx
			and edi, dword ptr [esi+18*4]
			rol edi, 1
			xor edx, edi

			pop edi

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
			push edi

			/*FL, FL^(-1)*/
			/*kL3, kL4(64)*/

			/*FL*/
			mov edi, eax
			and edi, dword ptr [esi+32*4]
			rol edi, 1
			xor ebx, edi
			mov edi, dword ptr [esi+33*4]
			or  edi, ebx
			xor eax, edi

			/*FL^(-1)*/
			mov edi, dword ptr [esi+35*4]
			or  edi, edx
			xor ecx, edi
			mov edi, ecx
			and edi, dword ptr [esi+34*4]
			rol edi, 1
			xor edx, edi

			pop edi

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
			push edi

			/*FL, FL^(-1)*/
			/*kL5, kL6(64)*/

			/*FL*/
			mov edi, eax
			and edi, dword ptr [esi+48*4]
			rol edi, 1
			xor ebx, edi
			mov edi, dword ptr [esi+49*4]
			or  edi, ebx
			xor eax, edi

			/*FL^(-1)*/
			mov edi, dword ptr [esi+51*4]
			or  edi, edx
			xor ecx, edi
			mov edi, ecx
			and edi, dword ptr [esi+50*4]
			rol edi, 1
			xor edx, edi

			pop edi


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


			xor ecx, eax
			xor edx, ebx

			pop ebx
			pop eax
			pop esi

/********************************************************************************/
			/*kw3, kw4*/
			xor eax, dword ptr [esi+64*4]
			xor ebx, dword ptr [esi+65*4]
			xor ecx, dword ptr [esi+66*4]
			xor edx, dword ptr [esi+67*4]

/********************************************************************************/

#ifdef ___BIGENDIAN___
			bswap eax
			bswap ebx
			bswap ecx
			bswap edx
#endif
			mov edi, [c]
			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx
			mov dword ptr [edi+8], ecx
			mov dword ptr [edi+12], edx

	}
}
#else
{
	unt w[2], tmpL[2], tmpR[2];
	unt M[2][2];
	uchar *mp, *cp;

	cp = mp = (uchar *)M[0] + 15;

	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;
	*mp=*m;mp--;m++;*mp=*m;mp--;m++;

	/*L0 || R0 = M(128) xor (kw1 || kw2)*/
	camellia_xor(tmpL, M[1], cmENkey->subkey_all[0]);
	camellia_xor(tmpR, M[0], cmENkey->subkey_all[1]);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[2]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[3]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[4]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[5]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[6]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[7]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	/*Lr = FL(Lr, kL[(2r/6)-1])*/
	camellia_FL(tmpL, cmENkey->subkey_all[8]);
	camellia_FL_1(tmpR, cmENkey->subkey_all[9]);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[10]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[11]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[12]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[13]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[14]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[15]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	/*Lr = FL(Lr, kL[(2r/6)-1])*/
	camellia_FL(tmpL, cmENkey->subkey_all[16]);
	camellia_FL_1(tmpR, cmENkey->subkey_all[17]);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[18]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[19]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[20]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[21]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[22]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[23]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	if(cmENkey->keysize == 128){
		camellia_xor(M[1], tmpR, cmENkey->subkey_all[24]);
		camellia_xor(M[0], tmpL, cmENkey->subkey_all[25]);
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		*c=*cp;cp--;c++;*c=*cp;cp--;c++;
		/*ローカル変数の値を消去*/
		camellia_zero(tmpL);
		camellia_zero(tmpR);
		camellia_zero(w);
		camellia_zero(M[0]);
		camellia_zero(M[1]);
		return;
	}

	/*Lr = FL(Lr, kL[(2r/6)-1])*/
	camellia_FL(tmpL, cmENkey->subkey_all[24]);
	camellia_FL_1(tmpR, cmENkey->subkey_all[25]);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[26]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[27]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[28]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[29]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[30]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	/* Lr = R(r-1) xor F(L(r-1), kr) */
	/*F(L(r-1), kr)*/
	camellia_F(tmpL, tmpL, cmENkey->subkey_all[31]);
	/*R(r-1) xor F(L(r-1), kr)*/
	camellia_xor(tmpL, tmpL, tmpR);
	/* Rr = L(r-1) */
	camellia_copy(tmpR, w);

	camellia_xor(M[1], tmpR, cmENkey->subkey_all[32]);
	camellia_xor(M[0], tmpL, cmENkey->subkey_all[33]);

	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;
	*c=*cp;cp--;c++;*c=*cp;cp--;c++;

	/*ローカル変数の値を消去*/
	camellia_zero(tmpL);
	camellia_zero(tmpR);
	camellia_zero(w);
	camellia_zero(M[0]);
	camellia_zero(M[1]);
}
#endif
}
