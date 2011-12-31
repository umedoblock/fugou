/* main.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../utbi_h\camellia\camellia.h"
#include "dllsha512.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define INNAME "in.txt"
#define OUTNAME "out.txt"
#define TMPNAME "tmp.txt"

int main(void)
{
	uchar *m;
	uchar *c;
	uchar *b;
	uchar *p;
	CAMELLIA_KEY ckey;
	unt i, mBytes, j, tmp_mBytes;
	uchar data[256/8];
	clock_t s, e;
	double tm[2], v[2];
	int flg = 1;
	char ss[80];
	uchar oldiv[CM_BLOCKSIZE], newiv[CM_BLOCKSIZE];
	uchar *iv;
	FILE *fi, *fo, *ft;

	printf("MBytes? :");gets(ss);
	mBytes = (unt)(1024*1024*atoi(ss));
	mBytes /= 1024*1024;
	tmp_mBytes = mBytes;

	fi = fopen(INNAME, "rb");
	if(fi == NULL){
		printf("error fopen()\n");
		exit(1);
	}
	fo = fopen(OUTNAME, "wb");
	if(fo == NULL){
		printf("error fopen()\n");
		exit(1);
	}
	mBytes = 1024;
	m = (uchar *)malloc(sizeof(uchar)*mBytes*3+2*CM_BLOCKSIZE);
	if(m==NULL){
		puts("NULL");
		putchar('\a');
		exit(1);
	}
	c = m + mBytes;
	b = c + mBytes+2*CM_BLOCKSIZE;
	
	memset(oldiv, '&', sizeof(oldiv));
	memcpy(newiv, oldiv, sizeof(oldiv));
	DLLsha512_DataHash(m, (int)mBytes, m, 64);
	m[0]++;
	//memset(m, 'm', mBytes);

	ckey.keysize = 256;
	p = (uchar *)ckey.key[0];

	camellia_keyset(&ckey, data, 256);

	camellia_keygen(&ckey);

	/*camellia_decenc_cbc_block(b, newiv, &ckey,
							  m, oldiv, &ckey,
							  mBytes/CM_BLOCKSIZE);*/


	j = camellia_cBytes(mBytes);

	s = clock();
	iv = c;
	//c += CM_BLOCKSIZE;
	camellia_iv(iv);

	//fread(m, mBytes, sizeof(uchar), fi);
	fseek(fi, 0L, SEEK_END);
	mBytes = ftell(fi);
	fseek(fi, 0L, SEEK_SET);
	//camellia_encrypt_cbc_DataData(c+CM_BLOCKSIZE, m, iv, 1024, &ckey);
	fwrite(iv, CM_BLOCKSIZE, sizeof(uchar), fo);

	camellia_encrypt_cbc_FileFile(fo, fi, iv, 0, (ulong)mBytes, &ckey);
	e = clock();
	tm[0] = (double)(e-s)/CLOCKS_PER_SEC;
	v[0] = ((double)(8*mBytes)/(1024*1024))/tm[0];
	fclose(fi);fclose(fo);
	putchar('\a');

	s = clock();
	fi = fopen(OUTNAME, "rb");
	if(fi == NULL){
		printf("error fopen()\n");
		exit(1);
	}
	ft = fopen(TMPNAME, "wb");
	if(ft == NULL){
		printf("error fopen()\n");
		exit(1);
	}
	memset(iv, 0, CM_BLOCKSIZE);
	fread(iv, CM_BLOCKSIZE, sizeof(uchar), fi);
	camellia_decrypt_cbc_FileFile(ft, fi, iv, 0, mBytes+CM_BLOCKSIZE, &ckey);
	fclose(fi);fclose(ft);


	//mBytes = camellia_decrypt_cbc_DataData(b, c+CM_BLOCKSIZE, iv, j-2*CM_BLOCKSIZE, &ckey);
	e = clock();
	tm[1] = (double)(e-s)/CLOCKS_PER_SEC;
	v[1] = ((double)(8*mBytes)/(1024*1024))/tm[1];
	putchar('\a');

/*
	flg = 1;
	for(i=0;i<mBytes&&flg;i++){
		flg = (m[i] == b[i]);
	}
	if(flg&&(tmp_mBytes==mBytes)){
		puts("good camellia cbc mode");
	}else{
		puts("bad camelia cbc mode");
		printf("tmp = %u, mBytes = %u flg = %d\n", tmp_mBytes, mBytes, flg);

	}
	*/
	printf("ファイルサイズ :%uMBytes\n", mBytes/(1024*1024));
	printf("ファイルサイズ :%uBytes\n", mBytes);
	printf("暗復号平均速度 :%lfMbps\n", (v[0]+v[1])/2);
	printf("　　暗号化速度 :%lfMbps\n", v[0]);
	printf("　　　復号速度 :%lfMbps\n", v[1]);
	printf("暗復号平均時間 :%lf秒\n", (tm[0]+tm[1])/2);
	printf("　　暗号化時間 :%lf秒\n", tm[0]);
	printf("　　　復号時間 :%lf秒\n", tm[1]);
	printf("　　128bit換算 :%lfMbps\n", 1.33333*((v[0]+v[1])/2));

	free(m);
	return 0;
}


