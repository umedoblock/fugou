/* sha256.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "sha.h"

void sha256_file(unt *Hash, char *filename)
{
	unt K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	unt H[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};

	unt a, b, c, d, e, f, g, h;
	unt tmp1, tmp2;

	FILE *fin;

	int flg = 0;
	unt i, j, k;
	unt f_bsize[2]={0x00000000, 0x00000000};

	uchar *p;

	int ch;

	unt M[16];
	unt W[64];

	fin = fopen(filename, "rb");
	if(fin == NULL){
		puts("ファイル開けない sha256");
		exit(1);
	}

	p = (uchar *)M;

	for(i=0;flg!=1;i=0){
		while((ch = fgetc(fin)) != EOF){
			f_bsize[0]+=8;
			if(f_bsize[0]){
				;
			}else{
				f_bsize[1]++;
			}
			*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)ch;
			i++;
			if(i==64)break;
		}

		if((i != 64) | (ch==EOF)){
			if(flg == 2){
				*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				i++;
				for(;i<56;i++){
					*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				}
				flg = 1;
				M[14] = f_bsize[1];
				M[15] = f_bsize[0];
			}else if(i < 56){
				*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x80;
				i++;
				for(;i<56;i++){
					*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				}
				flg = 1;
				M[14] = f_bsize[1];
				M[15] = f_bsize[0];
			}else{
				*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x80;
				i++;
				for(;i<64;i++){
					*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				}
				flg = 2;
				i = 0;
			}
		}

		for(j=0;j<16;j++){
			W[j] = M[j];
		}
		for(;j<64;j++){
			W[j] = sha_sigma1_32(W[j-2]) + W[j-7] + sha_sigma0_32(W[j-15]) + W[j-16];
		}

		a = H[0];
		b = H[1];
		c = H[2];
		d = H[3];
		e = H[4];
		f = H[5];
		g = H[6];
		h = H[7];

		for(k=0;k<64;k++){
			tmp1 = h + sha_SIGMA1_32(e) + sha_ch_32(e, f, g) + K[k] + W[k];
			tmp2 = sha_SIGMA0_32(a) + sha_maj_32(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + tmp1;
			d = c;
			c = b;
			b = a;
			a = tmp1 + tmp2;

		}

		H[0] += a;
		H[1] += b;
		H[2] += c;
		H[3] += d;
		H[4] += e;
		H[5] += f;
		H[6] += g;
		H[7] += h;

	}

	for(i = 0;i<8;i++){
		*(Hash+(7-i)) = H[i];
	}

	fclose(fin);
}

void sha256_str(unt *Hash, char *ss)
{
	unt K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	unt H[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};

	unt a, b, c, d, e, f, g, h;
	unt tmp1, tmp2;

	int flg = 0;
	unt i, j, k;
	unt f_bsize[2]={0x00000000, 0x00000000};

	uchar *p;

	int ch;

	unt M[16];
	unt W[64];

	p = (uchar *)M;

	for(i=0;flg!=1;i=0){
		while((ch = *ss) != '\0'){
			f_bsize[0]+=8;
			if(f_bsize[0]){
				;
			}else{
				f_bsize[1]++;
			}
			*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)ch;
			i++;
			ss++;
			if(i==64)break;
		}

		if((i != 64) | (ch=='\0')){
			if(flg == 2){
				*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				i++;
				for(;i<56;i++){
					*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				}
				flg = 1;
				M[14] = f_bsize[1];
				M[15] = f_bsize[0];
			}else if(i < 56){
				*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x80;
				i++;
				for(;i<56;i++){
					*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				}
				flg = 1;
				M[14] = f_bsize[1];
				M[15] = f_bsize[0];
			}else{
				*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x80;
				i++;
				for(;i<64;i++){
					*(p+(i&0xfc)+(3-(i&0x3))) = (uchar)0x00;
				}
				flg = 2;
				i = 0;
			}
		}

		for(j=0;j<16;j++){
			W[j] = M[j];
		}
		for(;j<64;j++){
			W[j] = sha_sigma1_32(W[j-2]) + W[j-7] + sha_sigma0_32(W[j-15]) + W[j-16];
		}

		a = H[0];
		b = H[1];
		c = H[2];
		d = H[3];
		e = H[4];
		f = H[5];
		g = H[6];
		h = H[7];

		for(k=0;k<64;k++){
			tmp1 = h + sha_SIGMA1_32(e) + sha_ch_32(e, f, g) + K[k] + W[k];
			tmp2 = sha_SIGMA0_32(a) + sha_maj_32(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + tmp1;
			d = c;
			c = b;
			b = a;
			a = tmp1 + tmp2;

		}

		H[0] += a;
		H[1] += b;
		H[2] += c;
		H[3] += d;
		H[4] += e;
		H[5] += f;
		H[6] += g;
		H[7] += h;

	}

	for(i = 0;i<8;i++){
		*(Hash+(7-i)) = H[i];
	}

}
