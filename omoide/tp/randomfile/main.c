/* main.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../utbi_h\\sha\\sha.h"
#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main(void)
{
	long MBytes;
	uchar data[1024];
	wchar_t wss[80];
	unt i;
	SHA512 H, *pH = NULL;
	
	_wsetlocale(LC_ALL, L"ja");

	wprintf(L"乱数が何Mバイト必要?\n");
	wprintf(L"->");fgetws(wss, 75, stdin);
	MBytes = _wtol(wss);
	MBytes *= 1024*1024;

	pH = (SHA512 *)malloc(sizeof(SHA512));
	
	printf("出力先ファイル名は？\n");
	printf("->");fgetws(wss, 75, stdin);
	i = wcslen(wss);
	if(wss[i-1]==0x000a){
		wss[i-1] = L'\0';
	}
	if(wss[i-2] == 0x000d){
		wss[i-1] = L'\0';
		pH->hash[0][0] = L'\0';
	}

	for(i=0;i<7;i++){
		data[i] = (uchar)((i + 73) * 0x46);
	}
	for(i=0;i<1024;i++){
		data[i] = data[i-1] ^ data[i-6] ^ data[i-4];
	}

	H.hash[0][0] = MBytes;

	H.hash[0][1] = (wss[1000] << 16) | wss[203];

	H.hash[1][0] = (wss[367] << 16) | wss[203];
	H.hash[1][1] = (wss[879] << 16) | wss[751];

	sha512_randomData(data, 1024, H);
	sha512_Data(&H, data, 1024);

	printf("ただいま乱数作成中です。\n");

	sha512_randomFile(wss, MBytes, H);

	return 0;
}
