/* sha512.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha.h"

#define SHA512_BLOCKSIZE (1024/8)
unt K[80][2]	= {
	{0x4a475817, 0x6c44198c}, {0x3ad6faec, 0x5fcb6fab}, {0xfc657e2a, 0x597f299c}, {0xcb3e42b6, 0x4cc5d4be},
	{0x9c100d4c, 0x431d67c4}, {0x15c9bebc, 0x3c9ebe0a}, {0x40c72493, 0x32caab7b}, {0x23047d84, 0x28db77f5},
	{0x131c471b, 0x1b710b35}, {0xbef90dae, 0x113f9804}, {0xa2c898a6, 0x0a637dc5}, {0x72176fba, 0x06f067aa},
	{0xee6ed178, 0xf57d4f7f}, {0xcde0eb1e, 0xeada7dd6}, {0x21c0c207, 0xd186b8c7}, {0xea26619c, 0xca273ece},
	{0xe372532b, 0xc67178f2}, {0xb2c67915, 0xbef9a3f7}, {0xde82bde9, 0xa4506ceb}, {0x23631e28, 0x90befffa},
	{0x1a6439ec, 0x8cc70208}, {0xa1f0ab72, 0x84c87814}, {0x43172f60, 0x78a5636f}, {0x5defb2fc, 0x748f82ee},
	{0xd6b2b8a3, 0x682e6ff3}, {0x7763e373, 0x5b9cca4f}, {0xe3418acb, 0x4ed8aa4a}, {0xc5c95a63, 0x391c0cb3},
	{0xe19b48a8, 0x34b0bcb5}, {0xdf8eeb99, 0x2748774c}, {0x5141ab53, 0x1e376c08}, {0xb8d2d0c8, 0x19a4c116},
	{0x32bbd1b8, 0x106aa070}, {0x5771202a, 0xf40e3585}, {0x5565a910, 0xd6990624}, {0xd6ef5218, 0xd192e819},
	{0x0654be30, 0xc76c51a3}, {0xd0f89791, 0xc24b8b70}, {0xbc423001, 0xa81a664b}, {0x4cf10364, 0xa2bfe8a1},
	{0x1482353b, 0x92722c85}, {0x47edaee6, 0x81c2c92e}, {0x3c77b2a8, 0x766a0abb}, {0x8baf63de, 0x650a7354},
	{0x9d95b3df, 0x53380d13}, {0x5ac42aed, 0x4d2c6dfc}, {0x5c26c926, 0x2e1b2138}, {0x46d22ffc, 0x27b70a85},
	{0x0a0e6e70, 0x14292967}, {0xe003826f, 0x06ca6351}, {0x930aa725, 0xd5a79147}, {0x3da88fc2, 0xc6e00bf3},
	{0xbeef0ee4, 0xbf597fc7}, {0x98fb213f, 0xb00327c8}, {0x2db43210, 0xa831c66d}, {0xee66dfab, 0x983e5152},
	{0x831153b5, 0x76f988da}, {0xbd41fbd4, 0x5cb0a9dc}, {0x6ea6e483, 0x4a7484aa}, {0x592b0275, 0x2de92c6f},
	{0x77ac9c65, 0x240ca1cc}, {0x8b8cd5b5, 0x0fc19dc6}, {0x384f25e3, 0xefbe4786}, {0x9ef14ad2, 0xe49b69c1},
	{0xcf692694, 0xc19bf174}, {0x25c71235, 0x9bdc06a7}, {0x3b1696b1, 0x80deb1fe}, {0xf27b896f, 0x72be5d74},
	{0xd5ffb4e2, 0x550c7dc3}, {0x4ee4b28c, 0x243185be}, {0x45706fbe, 0x12835b01}, {0xa3030242, 0xd807aa98},
	{0xda6d8118, 0xab1c5ed5}, {0xaf194f9b, 0x923f82a4}, {0xb605d019, 0x59f111f1}, {0xf348b538, 0x3956c25b},
	{0x8189dbbc, 0xe9b5dba5}, {0xec4d3b2f, 0xb5c0fbcf}, {0x23ef65cd, 0x71374491}, {0xd728ae22, 0x428a2f98}
};

/*2005/03/28*/
void sha512_init(SHA512 *H, unt *bits)
{
	int i;
	unt H_init[8][2] = {
		{0x137e2179, 0x5be0cd19},
		{0xfb41bd6b, 0x1f83d9ab},
		{0x2b3e6c1f, 0x9b05688c},
		{0xade682d1, 0x510e527f},
		{0x5f1d36f1, 0xa54ff53a},
		{0xfe94f82b, 0x3c6ef372},
		{0x84caa73b, 0xbb67ae85},
		{0xf3bcc908, 0x6a09e667}
	};
	
	for(i=7;i>=0;i--){
        sha_copy_64(H->hash[i], H_init[i]);
	}

	bits[0] = 0;
	bits[1] = 0;
	bits[2] = 0;
	bits[3] = 0;
}

/*2005/03/28*/
void sha512(SHA512 *H, uchar *data, SHA512 *HH)
{

	unt a[2], b[2], c[2], d[2], e[2], f[2], g[2], h[2];
	unt tmp1[2], tmp2[2], tmp3[2], tmp4[2];

	int i, k;

	uchar *p;

	unt W[80][2];

	p = (uchar *)&W[0][0];
	p += 160*4-1;

	for(i=0;i<1024/8;i++){
		*p = *data;
		p--;
		data++;
	}

	for(i=16;i<80;i++){
		sha_sigma1_64(tmp1, W[79-(i-2)]);
		sha_sigma0_64(tmp2, W[79-(i-15)]);

		sha_add_64(W[79-i], W[79-(i-7)], W[79-(i-16)]);
		sha_add_64(W[79-i], W[79-i], tmp1);
		sha_add_64(W[79-i], W[79-i], tmp2);

	}

	sha_copy_64(a, HH->hash[7]);
	sha_copy_64(b, HH->hash[6]);
	sha_copy_64(c, HH->hash[5]);
	sha_copy_64(d, HH->hash[4]);
	sha_copy_64(e, HH->hash[3]);
	sha_copy_64(f, HH->hash[2]);
	sha_copy_64(g, HH->hash[1]);
	sha_copy_64(h, HH->hash[0]);

	for(k=0;k<80;k++){
		sha_SIGMA1_64(tmp3, e);
		sha_ch_64(tmp4, e, f, g);

		sha_add_64(tmp1, h, tmp3);
		sha_add_64(tmp1, tmp1, tmp4);

		sha_add_64(tmp1, tmp1, K[79-k]);
		sha_add_64(tmp1, tmp1, W[79-k]);

		sha_SIGMA0_64(tmp3, a);
		sha_maj_64(tmp4, a, b, c);
		sha_add_64(tmp2, tmp3, tmp4);

		sha_copy_64(h, g);
		sha_copy_64(g, f);
		sha_copy_64(f, e);
		sha_add_64(e, d, tmp1);
		sha_copy_64(d, c);
		sha_copy_64(c, b);
		sha_copy_64(b, a);
		sha_add_64(a, tmp1, tmp2);
	}

	/*a = a + b; は正常に動く*/
	sha_add_64(H->hash[7], HH->hash[7], a);
	sha_add_64(H->hash[6], HH->hash[6], b);
	sha_add_64(H->hash[5], HH->hash[5], c);
	sha_add_64(H->hash[4], HH->hash[4], d);
	sha_add_64(H->hash[3], HH->hash[3], e);
	sha_add_64(H->hash[2], HH->hash[2], f);
	sha_add_64(H->hash[1], HH->hash[1], g);
	sha_add_64(H->hash[0], HH->hash[0], h);
}

/*2005/03/28*/
int sha512_padding(uchar *pad, int flg, unt n, unt *bits)
{
	int i;
	uchar *p;
	
	bits[0] += 8*n;
	if(bits[0] < 8*n){
		bits[1]++;
		if(!bits[1]){
			bits[2]++;
			if(!bits[2]){
				bits[3]++;
			}
		}
	}
	if(n != SHA512_BLOCKSIZE){
		if(flg == 1){
			pad[n] = 0x80;
			n++;
			if(n<(1024-128)/8){
				flg = 2;
			}else{
				for(i=n;i<(1024)/8;i++){
					pad[i] = 0x00;
				}
				flg = 3;
			}
		}
		if(flg == 2){
			for(i=n;i<(1024-128)/8;i++){
				pad[i] = 0x00;
			}
			p = (uchar *)&bits[0];
			p += 4*4-1;
			for(i=(1024-128)/8;i<1024/8;i++){
				pad[i] = *p;
				p--;
			}
			flg = 0;
		}
		if(flg == 3){
			flg = 2;
		}
	}

	return flg;
}

/*2005/03/28*/
void sha512_FileName(SHA512 *H, char *filename)
{

	FILE *fin;
	int flg=1;
	unt n;
	uchar buf[SHA512_BLOCKSIZE];
	unt bits[4];
	
	fin = fopen(filename, "rb");
	if(fin == NULL){
		exit(1);
	}

	sha512_init(H, bits);

	while(flg){
        n = (unt)fread(buf, sizeof(uchar), SHA512_BLOCKSIZE, fin);
		flg = sha512_padding(buf, flg, n, bits);
		sha512(H, buf, H);
	}

	fclose(fin);
}

/*2005/03/28*/
void sha512_FileByte(SHA512 *H, FILE *fin, unt Byte)
{

	int flg=1;
	unt n;
	uchar buf[SHA512_BLOCKSIZE];
	unt bits[4];
	
	sha512_init(H, bits);

	while(flg){
		if(Byte > SHA512_BLOCKSIZE){
			n = SHA512_BLOCKSIZE;
			Byte -= SHA512_BLOCKSIZE;
		}else{
			n = Byte;
			Byte = 0;
		}
        n = fread(buf, sizeof(uchar), n, fin);
		flg = sha512_padding(buf, flg, n, bits);
		sha512(H, buf, H);
	}

}

/*2005/03/28*/
void sha512_Data(SHA512 *H, uchar *data, unt Byte)
{

	int flg=1;
	unt n, m;
	unt bits[4];
	uchar buf[SHA512_BLOCKSIZE];
	unt i = 0;
	
	sha512_init(H, bits);

	while(flg){
		if(Byte >= SHA512_BLOCKSIZE){
			n = SHA512_BLOCKSIZE;
			Byte -= SHA512_BLOCKSIZE;
		}else{
			n = Byte;
			Byte = 0;
		}
		for(i=0,m=0;i<n;i++){
			buf[m] = data[i];
			m++;
		}
		flg = sha512_padding(buf, flg, n, bits);
		sha512(H, buf, H);
	}

}

void sha512_hashtodata(uchar *data, SHA512 *x)
{
	int i, j;
	unt t;

	for(i=0;i<16;i++){
		t = x->hash[7-(i>>1)][1-(i&1)];
		for(j=0;j<(int )sizeof(unt);j++){
			*data++ = (uchar )((t>>(8*(sizeof(unt)-1-j)))&0xff);
		}
	}

}

void sha512_hashhash(SHA512 *x, SHA512 *y)
{
	uchar buf[512/8];

	sha512_hashtodata(buf, y);

	sha512_Data(x, buf, 512/8);
	memset(buf, 0, 512/8);
	
}
void sha512_RandomData(uchar *out, unt outsize, uchar *seed, unt seedsize)
{
	SHA512 h, hh;
	uchar buf[512/8];
	int copybytes;

	sha512_Data(&h, seed, (int)seedsize);
	
	for(;;){//(outsize > 0)
		sha512_hashtodata(buf, &h);

		if(outsize >= 512/8){
			copybytes = 512/8;
		}else{
			copybytes = outsize;
		}
		memcpy(out, buf, copybytes);
		out += copybytes;
		outsize -= copybytes;

		if(outsize==0){
			break;
		}
		sha512_hashhash(&hh, &h);
		memcpy(&h, &hh, sizeof(SHA512));
	}
}


void sha512_clear(SHA512 *H)
{
	memset(H, 0, sizeof(SHA512));
}

int sha512_equal(SHA512 a, SHA512 b)
{
	int i;

	for(i=512/32-1;i>=0&&a.hash[i/2][i%2]==b.hash[i/2][i%2];i--){
		;
	}

	return !(i+1);
}


void sha512_puthash(SHA512 a)
{
	int i;

	for(i=7;i>=0;i--){
		printf("%08x%08x ", a.hash[i][1], a.hash[i][0]);
	}
}

