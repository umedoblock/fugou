/* sha1.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "sha.h"


void sha1_file(unt *Hash, char *filename)
{
	FILE *fin;

	unt K[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
	unt H[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
	unt a = 0x67452301;
	unt b = 0xefcdab89;
	unt c = 0x98badcfe;
	unt d = 0x10325476;
	unt e = 0xc3d2e1f0;


	int flg = 0;
	unt i, j, k;
	unt f_bsize[2]={0x00000000, 0x00000000};
	unt temp = 0;

	uchar *p;

	int ch;

	unt M[16];
	unt W[80];

	for(i=0;i<16;i++){
		M[i] = 0xffffffff;
	}

	fin = fopen(filename, "rb");
	if(fin == NULL){
		puts("ファイル開けない sha1");
		getchar();
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
		for(;j<80;j++){
			W[j] = sha_ROTL_32((W[j-3] ^ W[j-8] ^ W[j-14] ^ W[j-16]), 1);
		}

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

	*(Hash+4) = H[0];
	*(Hash+3) = H[1];
	*(Hash+2) = H[2];
	*(Hash+1) = H[3];
    *(Hash+0) = H[4];
	
	fclose(fin);
}



void sha1_str(unt *Hash, char *ss)
{
	unt K[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
	unt H[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
	unt a = 0x67452301;
	unt b = 0xefcdab89;
	unt c = 0x98badcfe;
	unt d = 0x10325476;
	unt e = 0xc3d2e1f0;


	int flg = 0;
	unt i, j, k;
	unt f_bsize[2]={0x00000000, 0x00000000};
	unt temp = 0;

	uchar *p;

	int ch;

	unt M[16];
	unt W[80];

	for(i=0;i<16;i++){
		M[i] = 0xffffffff;
	}

	p = (uchar *)M;

	for(i=0;flg!=1;i=0){
		while((ch = (int)*ss) != '\0'){
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
		for(;j<80;j++){
			W[j] = sha_ROTL_32((W[j-3] ^ W[j-8] ^ W[j-14] ^ W[j-16]), 1);
		}

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

	*(Hash+4) = H[0];
	*(Hash+3) = H[1];
	*(Hash+2) = H[2];
	*(Hash+1) = H[3];
    *(Hash+0) = H[4];
	
}
