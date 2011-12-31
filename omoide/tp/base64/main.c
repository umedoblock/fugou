/* main.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../utbi_h\\camellia\\camellia.h"
#include "../utbi_h\\base64\\base64.h"
#define _UNICODE

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
void ffff(wchar_t *ssp, size_t n, FILE *fin);

int main(void)
{
	/*係数は2.66666666666666666*/
	wchar_t *wssp, *wstrp, *bwssp;
	unt len, i, j, n, m;
	CAMELLIA_KEY enkey, dekey, tmpkey;
	SHA512 abc;
	int k, flg;

	_wsetlocale(LC_ALL, L"ja");

	tmpkey.keysize = 256;

	wssp = (wchar_t *)malloc(sizeof(wchar_t)*(unt)((1.0+1.0+2.7)*4096.0));
	if(wssp==NULL){
		exit(1);
	}
	wstrp = wssp + 4096;
	bwssp = wstrp + 4096;


	flg = 1;
	for(k=0;k<100&&flg;k++){
		i=0;
		j=0;
		len = 4094;
		fwprintf(stderr, L"終了したければ\"&&\"を入力\n");
		fwprintf(stderr, L"文字を入力 :\n");

		while(wssp[i-j]!='&'){
			/*文字列を受け取る*/
			wssp[i]=L'\0';
            fgetws(wssp+i, len, stdin);
			j = (unt)wcslen(wssp+i);
			i += j;
			len -= j;
			if(len<=1)break;
		}

		if(wssp[i-j+1]==L'&')break;
		len = (unt)wcslen(wssp);
		wssp[len-2] = L'\0';

        sha512_Data(&abc, (uchar *)wssp, 2*len);

		camellia_hashset(&tmpkey, &abc);
		camellia_keygen(&tmpkey);
		camellia_keycopy(&enkey, &tmpkey);
		camellia_keycopy(&dekey, &tmpkey);

		camellia_encrypt_cbc_WssBase64(bwssp, wssp, &enkey);

		fwprintf(stderr, L"--------------------------------------\n");
		fwprintf(stderr, L"base64_encode :\n");
		fwprintf(stderr, L"%s\n", bwssp);
		fwprintf(stderr, L"--------------------------------------\n");

		camellia_decrypt_cbc_Base64Wss(wstrp, bwssp, &dekey);
		fwprintf(stderr, L"base64_decode :\n");
		fwprintf(stderr, L"%s", wstrp);
		fwprintf(stderr, L"--------------------------------------\n");

		flg = base64_equal((uchar *)wstrp, (uchar *)wssp, 2*wcslen(wssp));
		if(flg){
				printf("good\n");
				printf("k=%d\n", k);
		}else{
			printf("bad\n");
		}
		fwprintf(stderr, L"\n\n");

	}


	if(k==100){
		printf("very good\n");
	}else{
		printf("shock\n");
	}
	free(wssp);

	return 0;
}


void ffff(wchar_t *ssp, size_t n, FILE *fin)
{
	wchar_t wc;
	size_t i=0;

	for(;;){
		wc=fgetwc(fin);
		if(wc!=WEOF&&wc!=L'\0'&&i<n){
			*ssp = wc;
			ssp++;
			i++;
		}else{
			break;
		}
	}

	*ssp = L'\0';
}
