/* camellia.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

// camellia.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
#include <stdio.h>
#include <time.h>
#include "../utbi_h\\camellia\\camellia.h"

void dbg_printf(unt *x, int keysize);


int main(void)
{
	int i, cout=0, flg=1;
	unt j, t;
	uchar *p;
	uchar m[CM_BLOCKSIZE], c[CM_BLOCKSIZE], buf[CM_BLOCKSIZE];
	clock_t start, end;
	double time0;
	CAMELLIA_KEY cmk, cmen, cmde;
	SHA512 SH;

	cmk.keysize = 256;
/*
	p = (uchar *)cmk.key[0];
	camellia_padding(p, cmk.keysize/8);
*/

	printf("m =\n");
	for(i=0;i<CM_BLOCKSIZE;i++){
		m[i] = 'T' + (uchar)i;
		printf("%02x ", m[i]);

	}puts("");
	sha512_Data(&SH, m, i);
	camellia_hashset(&cmk, &SH);
	printf("HASH =\n");
	for(i=0,j=0;i<8;i++,j++){
		printf("%08x%08x", SH.hash[8-i-1][1], SH.hash[8-1-i][0]);
		if(j%4==3){
			puts("");
		}else{
			printf(" ");
		}
	}
	printf("key =\n");
	p = (uchar *)cmk.key[0];
	for(i=0,j=0;i<cmk.keysize/8;i++,j++){
		printf("%02x", p[cmk.keysize/8-1-i]);
		if(j%16==15){
			puts("");
		}else{
			printf(":");
		}
	}
/***********************************************************/

	camellia_keygen(&cmk);
	camellia_keycopy(&cmen, &cmk);
	camellia_keycopy(&cmde, &cmk);
	start = clock();
	for(j=0;j<1024*1024*4*8/*&&flg*/;j++){
/*
		for(i=0;i<CM_BLOCKSIZE;i++){
			m[i] ^= c[i];
		}
		for(i=0;i<CM_BLOCKSIZE;i++){
			c[i] = m[i];
		}
*/
		camellia_encrypt(c, m, &cmen);
/*
		camellia_decrypt(buf, c, &cmde);
		for(i=CM_BLOCKSIZE;(m[i-1]==buf[i-1])&&i>0;i--){
		}
		cout += flg = !(i);
*/	
	}
	cout = 0;
	camellia_destroy(&cmk);
	camellia_destroy(&cmen);
	camellia_destroy(&cmde);

	end = clock();
/***********************************************************/

	time0 = (double)(end-start)/CLOCKS_PER_SEC;

	puts("");
	if(flg){
		printf("お(・∀・)め(・∀・)で(・∀・)と(・∀・)う！\n");
		printf("%d回連続で成功したよ。\n", cout);
		putchar('\a');putchar('\a');
	}else{
		putchar('\a');
	}

	printf("%dMByteの", t = ((j)/(1024*1024))*(/*2**/CM_BLOCKSIZE));
	printf("暗復号にかかった時間 :%lf秒\n", time0);
	printf("暗復号速度 = %lfMB/s\n", (double)t/(time0));
	puts("何かキーを入力すると終了します。");
	getchar();

	return 0;
}

void dbg_printf(unt *x, int keysize)
{
	int k, kw, kL;

	for(kw=1;kw<=2;kw++){
		printf("kw%d = ", kw);camellia_putx(x, 64);puts("");
		x+=2;
	}
	for(k=1;k<=6;k++){
		printf("k%d = ", k);camellia_putx(x, 64);puts("");
		x+=2;
	}
	for(kL=1;kL<=2;kL++){
		printf("kL%d = ", kL);camellia_putx(x, 64);puts("");
		x+=2;
	}
	for(;k<=12;k++){
		printf("k%d = ", k);camellia_putx(x, 64);puts("");
		x+=2;
	}
	for(;kL<=4;kL++){
		printf("kL%d = ", kL);camellia_putx(x, 64);puts("");
		x+=2;
	}
	for(;k<=18;k++){
		printf("k%d = ", k);camellia_putx(x, 64);puts("");
		x+=2;
	}

	if(keysize == 128){
		for(;kw<=4;kw++){
			printf("kw%d = ", kw);camellia_putx(x, 64);puts("");
			x+=2;
		}
		return;
	}
	for(;kL<=6;kL++){
		printf("kL%d = ", kL);camellia_putx(x, 64);puts("");
		x+=2;
	}

	for(;k<=24;k++){
		printf("k%d = ", k);camellia_putx(x, 64);puts("");
		x+=2;
	}
	for(;kw<=4;kw++){
		printf("kw%d = ", kw);camellia_putx(x, 64);puts("");
		x+=2;
	}
}



