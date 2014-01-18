/* sha1.c
 * Copyright (C) 2008-2012 梅濁酒(umedoblock)
 */

#include <stdio.h>
#include <string.h>

typedef unsigned int unt;
typedef unsigned char uchar;

unt K[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};

unt sha_ch_32(unt x, unt y, unt z)
{
	return ((x & y) ^ ((~x) & z));
}

unt sha_ROTL_32(unt x, int bLen)
{
	return (x << bLen) | (x >> (32 - bLen));
}

void hash2data(uchar *data, unt *hash, int hashlen)
{
	unt t;
	int i, j, k;

	for(i=hashlen/(8*sizeof(unt))-1,j=0;i>=0;i--,j++){
		t = hash[i];
		for(k=0;k<(int)sizeof(unt);k++){
			data[sizeof(unt)*j+k] =
				(t >> (8 * (sizeof(unt)-1-k))) & 0xff;
		}
	}
}

void view_hash(uchar *hash, int len_hash)
{
	int i;

	for (i=0;i<len_hash/8;i++){
		fprintf(stdout, "%02x", hash[i]);
	}
}

inline void sha1_load(unt *f_bsize, uchar *p, int i, int ch)
{
	f_bsize[0]+=8;
	if(f_bsize[0]){
		;
	}else{
		f_bsize[1]++;
	}
	/* 何をやっとるんだね。。。 */
	/* 理解できんじゃないか！ */
	*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)ch;
}

int sha1_load_data(
	unt *f_bsize, uchar *p, uchar *data, int i, int index, int len_data)
{
	int ch;

	while(index < len_data){
		ch = data[index];
		sha1_load(f_bsize, p, i, ch);
		i++;
		index++;
		if(i==64)break;
	}
	return i;
}

int sha1_load_file(
	unt *f_bsize, uchar *p, FILE *fp, int i, int index)
{
	int ch;

	ch = 'a';
	while ((ch = fgetc(fp)) != EOF) {
		sha1_load(f_bsize, p, i, ch);
		i++;
		index++;
		if(i==64)break;
	}

	return i;
}

int sha1_tail(int *flg, uchar *p, int i, unt *M, unt *f_bsize)
{
	if(*flg == 2){
		*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
		i++;
		for(;i<56;i++){
			*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
		}
		*flg = 1;
		M[14] = f_bsize[1];
		M[15] = f_bsize[0];
	}else if(i < 56){
		*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x80;
		i++;
		for(;i<56;i++){
			*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
		}
		*flg = 1;
		M[14] = f_bsize[1];
		M[15] = f_bsize[0];
	}else{
		*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x80;
		i++;
		for(;i<64;i++){
			*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
		}
		*flg = 2;
		i = 0;
	}

	return i;
}

void sha1_main(unt *H, unt *W, unt *K)
{
	int k;
	unt temp = 0;
	unt a , b, c, d , e;

	a = H[0];
	b = H[1];
	c = H[2];
	d = H[3];
	e = H[4];

	for(k=0;k<20;k++){
		temp = sha_ROTL_32(a, 5) + sha_ch_32(b, c, d) + e + W[k] + K[0];
		e = d;
		d = c;
		c = sha_ROTL_32(b, 30);
		b = a;
		a = temp;
	}
	for(;k<40;k++){
		temp = sha_ROTL_32(a, 5) + (b^c^d) + e + W[k] + K[1];
		e = d;
		d = c;
		c = sha_ROTL_32(b, 30);
		b = a;
		a = temp;
	}
	for(;k<60;k++){
		temp = sha_ROTL_32(a, 5) + ((b&c)|(b&d)|(c&d)) + e + W[k] + K[2];
		e = d;
		d = c;
		c = sha_ROTL_32(b, 30);
		b = a;
		a = temp;
	}
	for(;k<80;k++){
		temp = sha_ROTL_32(a, 5) + (b^c^d) + e + W[k] + K[3];
		e = d;
		d = c;
		c = sha_ROTL_32(b, 30);
		b = a;
		a = temp;
	}

	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;
}

void sha1(uchar *hash, uchar *data, FILE *fp, int len_data)
{
	unt H[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};

	int flg = 0;
	unt i, j;
	unt f_bsize[2]={0x00000000, 0x00000000};
	uchar *p;

	unt M[16];
	unt W[80];
	unt temp;

	int index;

	p = (uchar *)M;

	index = 0;
	for(i=0;flg!=1;i=0){
		if (data)
			i = sha1_load_data(f_bsize, p, data, i, index, len_data);
		else
			i = sha1_load_file(f_bsize, p, fp, i, index);
		index += i;
		if((i != 64)){
			i = sha1_tail(&flg, p, i, M, f_bsize);
		}

		j = 16;
		memcpy(W, M, sizeof(unt) * j);
		for(;j<80;j++){
			W[j] = sha_ROTL_32((W[j-3] ^ W[j-8] ^ W[j-14] ^ W[j-16]), 1);
		}

		sha1_main(H, W, K);
	}

	for (i=0;i<5/2;i++){
		temp = H[i];
		H[i] = H[4-i];
		H[4-i] = temp;
	}
	hash2data(hash, H, 160);
}
