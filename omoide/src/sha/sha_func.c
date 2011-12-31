/* sha_func.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "sha.h"

unt sha_ch_32(unt x, unt y, unt z)
{
	return ((x & y) ^ ((~x) & z));
}

unt sha_parity_32(unt x, unt y, unt z)
{
	return x ^ y ^ z;
}

unt sha_maj_32(unt x, unt y, unt z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}

unt sha_ROTL_32(unt x, int bLen)
{
	return (x << bLen) | (x >> (32 - bLen));
}

unt sha_ROTR_32(unt x, int bLen)
{
	return (x >> bLen) | (x << (32 - bLen));
}

unt sha_SIGMA0_32(unt x)
{
	return sha_ROTR_32(x, 2) ^ sha_ROTR_32(x, 13) ^ sha_ROTL_32(x, 10);
}

unt sha_SIGMA1_32(unt x)
{
	return sha_ROTR_32(x, 6) ^ sha_ROTR_32(x, 11) ^ sha_ROTL_32(x, 7);
}

unt sha_sigma0_32(unt x)
{
	return sha_ROTR_32(x, 7) ^ sha_ROTL_32(x, 14) ^ (x >> 3);
}

unt sha_sigma1_32(unt x)
{
	return sha_ROTL_32(x, 15) ^ sha_ROTL_32(x, 13) ^ (x >> 10);
}

void sha_ch_64(unt *w, unt *x, unt *y, unt *z)
{
	*(w) = ((*x & *y) ^ ((~(*x)) & *z));
	w++;x++;y++;z++;
	*(w) = ((*x & *y) ^ ((~(*x)) & *z));
}

void sha_maj_64(unt *w, unt *x, unt *y, unt *z)
{
	*(w) = ((*x) & (*y)) ^ ((*x) & (*z)) ^ ((*y) & (*z));
	w++;x++;y++;z++;
	*(w) = ((*x) & (*y)) ^ ((*x) & (*z)) ^ ((*y) & (*z));
}

void sha_ROTL_64(unt *w, unt *x, int bLen)
{
	unt tmp = *(x+1);

	*(w+1) = (*(x+1) << bLen) | (*(x) >> (32 - bLen));
	*(w) = (*(x) << bLen) | (tmp >> (32 - bLen));
}

void sha_ROTR_64(unt *w, unt *x, int bLen)
{
	unt tmp = *x;

	*(w) = (*x) >> bLen | (*(x+1) << (32 - bLen));
	*(w+1) = (*(x+1)) >> bLen | (tmp << (32 - bLen));
}

void sha_SHR_64(unt *w, unt *x, int bLen)
{
	*w = (*x >> bLen) | (*(x+1) << (32 - bLen));
	w++;x++;
	*w = *x >> bLen;
}

void sha_SIGMA0_64(unt *w, unt *x)
{
	unt tmp1[2], tmp2[2], tmp3[2];

	sha_ROTR_64(tmp1, x, 28);
	sha_ROTL_64(tmp2, x, 30);
	sha_ROTL_64(tmp3, x, 25);
	sha_xor_64(w, tmp1, tmp2);
	sha_xor_64(w, w, tmp3);
}

void sha_SIGMA1_64(unt *w, unt *x)
{
	unt tmp1[2], tmp2[2], tmp3[2];

	sha_ROTR_64(tmp1, x, 14);
	sha_ROTR_64(tmp2, x, 18);
	sha_ROTL_64(tmp3, x, 23);
	sha_xor_64(w, tmp1, tmp2);
	sha_xor_64(w, w, tmp3);
}

void sha_sigma0_64(unt *w, unt *x)
{
	unt tmp1[2], tmp2[2], tmp3[2];

	sha_ROTR_64(tmp1, x, 1);
	sha_ROTR_64(tmp2, x, 8);
	sha_SHR_64(tmp3, x, 7);
	sha_xor_64(w, tmp1, tmp2);
	sha_xor_64(w, w, tmp3);
}

void sha_sigma1_64(unt *w, unt *x)
{
	unt tmp1[2], tmp2[2], tmp3[2];

	sha_ROTR_64(tmp1, x, 19);

	sha_ROTL_64(tmp2, x, 3);

	sha_SHR_64(tmp3, x, 6);

	sha_xor_64(w, tmp1, tmp2);

	sha_xor_64(w, w, tmp3);
}

void putb(unt *x, int bLen)
{
	int i;
	unt flg;

		for(i=(bLen-1)/32;i>=0;i--){
			for(flg=0x80000000;flg;flg>>=1){
				putchar('1'-!(flg&*(x+i)));
				/*
				if(flg & *(x+i)){
					putchar('1');
				}else{
					putchar('0');
				}
				*/
			}
			putchar(' ');
		}
}
