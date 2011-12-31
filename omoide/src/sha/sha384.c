/* sha384.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha.h"

#define SHA512_BLOCKSIZE (1024/8)

/*2005/03/28*/
void sha384_init(SHA512 *H, unt *bits)
{
	int i;
	unt H_init[8][2] = {
		{0xbefa4fa4, 0x47b5481d},
		{0x64f98fa7, 0xdb0c2e0d},
		{0x68581511, 0x8eb44a87},
		{0xffc00b31, 0x67332667},
		{0xf70e5939, 0x152fecd8},
		{0x3070dd17, 0x9159015a},
		{0x367cd507, 0x629a292a},
		{0xc1059ed8, 0xcbbb9d5d}
	};
	
	for(i=7;i>=0;i--){
        sha_copy_64(H->hash[i], H_init[i]);
	}
	for(i=0;i<4;i++){
		bits[i] = 0;
	}
}

/*2005/03/28*/
void sha384_file_name(SHA512 *H, char *filename)
{

	FILE *fin;
	int flg=1;
	unt n;
	uchar buf[SHA512_BLOCKSIZE];
	unt bits[4];
	int i;
	
	fin = fopen(filename, "rb");
	if(fin == NULL){
		puts("ファイル開けない sha512");
		exit(1);
	}

	sha384_init(H, bits);

	while(flg){
        n = fread(buf, sizeof(uchar), SHA512_BLOCKSIZE, fin);
		flg = sha512_padding(buf, flg, n, bits);
		sha512(H, buf, H);
	}

	fclose(fin);

	for(i=0;i<6;i++){
		sha_copy_64(H->hash[i] ,H->hash[i+2]);
	}
	for(;i<8;i++){
		H->hash[i][0] = 0;
		H->hash[i][1] = 0;
	}
}

/*2005/03/28*/
void sha384_file_Byte(SHA512 *H, FILE *fin, unt Byte)
{

	int flg=1;
	unt n, i;
	unt bits[4];
	uchar buf[SHA512_BLOCKSIZE];
	
	sha384_init(H, bits);

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

	fclose(fin);
	for(i=0;i<6;i++){
		sha_copy_64(H->hash[i] ,H->hash[i+2]);
	}
	for(;i<8;i++){
		H->hash[i][0] = 0;
		H->hash[i][1] = 0;
	}
}

/*2005/03/28*/
void sha384_Byte(SHA512 *H, uchar *data, unt Byte)
{

	int flg=1;
	unt n, m=0;
	uchar buf[SHA512_BLOCKSIZE];
	unt i = 0, bits[4];
	
	sha384_init(H, bits);

	while(flg){
		if(Byte > SHA512_BLOCKSIZE){
			n = SHA512_BLOCKSIZE;
			Byte -= SHA512_BLOCKSIZE;
		}else{
			n = Byte;
			Byte = 0;
		}
		memcpy(buf, data + m, n);
		m += n;
		flg = sha512_padding(buf, flg, n, bits);
		sha512(H, buf, H);
	}
	for(i=0;i<6;i++){
		sha_copy_64(H->hash[i] ,H->hash[i+2]);
	}
	for(;i<8;i++){
		H->hash[i][0] = 0;
		H->hash[i][1] = 0;
	}

}

