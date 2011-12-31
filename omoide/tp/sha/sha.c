/* sha.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

// sha.cpp : コンソール アプリケーションのエントリ ポイントを定義します。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utbi_h\\sha\\sha.h"

int main(void)
{
	unt H[16];

	char ss[80];
	FILE *fin;
	uchar buf[150] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

	puts("Hash値を取るファイル名を指定してください。");
	printf("file name ->");gets(ss);

	/********************sha512************************/

	sha512_file_name(H, ss);
	printf("sha512_file_name = \n");
	sha_puthash(H, 512);puts("");

	fin = fopen(ss, "rb");
	sha512_FileByte(H, fin, 112);
	printf("sha512_FileByte = \n");
	sha_puthash(H, 512);puts("");
	fclose(fin);

	sha512_Byte(H, buf, 112);
	printf("sha512_Byte = \n");
	sha_puthash(H, 512);puts("");

	/********************sha384************************/

	sha384_file_name(H, ss);
	printf("sha384_file_name = \n");
	sha_puthash(H, 384);puts("");

	fin = fopen(ss, "rb");
	sha384_file_Byte(H, fin, 112);
	printf("sha384_file_Byte = \n");
	sha_puthash(H, 384);puts("");
	fclose(fin);

	sha384_Byte(H, buf, 112);
	printf("sha384_Byte = \n");
	sha_puthash(H, 384);puts("");

	/**************************************************/



#ifdef IOSOHISUOIER
	puts("SHA-1, SHA-256, SHA-384, SHA-512の");
	puts("ハッシュ値を一気に取ります。");
	puts("Hash値を取るファイル名を指定してください。");
	printf("file name ->");gets(ss);

	sha1_str(H, abc);
	puts("SHA-1_str = ");
	sha_puthash(H, 160);puts("");
	sha1_file(H, ss);
	puts("SHA-1 = ");
	sha_puthash(H, 160);puts("");

	puts("SHA-256_str = ");
	sha256_str(H, abc);
	sha_puthash(H, 256);puts("");

	puts("SHA-256 = ");
	sha256_file(H, ss);
	sha_puthash(H, 256);puts("");

	puts("SHA-384_str = ");
	sha384_str(H, abc);
	sha_puthash(H, 384);puts("");

	puts("SHA-384 = ");
	sha384_file(H, ss);
	sha_puthash(H, 384);puts("");

	puts("SHA-512_str = ");
	sha512_str(H, abc);
	sha_puthash(H, 512);puts("");

	puts("SHA-512 = ");
	sha_puthash(H, 512);puts("");
#endif

	getchar();
	return 0;
}
