# esign.cpp
# Copyright (C) 2008 梅どぶろく umedoblock

#define _UNICODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "../\\utbi_h\\utbi_sanjutsu\\utbi_sanjutsu.h"
#include "../\\utbi_h\\esign\\esign.h"

int main(void)
{
	int sw;
	unt pLen, k, e, op;
	int i;
	wchar_t wss[80], wstr[1000];
	ESIGN_PUBLICKEY epubkey;
	ESIGN_PRIVATEKEY eprikey;
	ESIGN_SIGN sign;
	ESIGN_VERIFY verify;
	FILE *fpub, *fpri, *fs, *fv;

	_wsetlocale(LC_ALL, L"japanese");

	printf("どのモードを使いますか?\n");
	puts("1: 公開鍵・秘密鍵生成");
	puts("2: 署名生成");
	puts("3: 署名検証");
	printf("->");_getws(wss);

	sw = _wtoi(wss);

	switch(sw){
		case 1:
			wprintf(L"pLen?\n->");_getws(wss);
			pLen = _wtoi(wss);
			wprintf(L"op?\n");
			wprintf(L"y or n ->");_getws(wss);
			if(wss[0]==L'n'||wss[0]==L'N'){
				op = 0;
			}else{
				op = 1;
			}
			wprintf(L"pubkey and prikey name?\n");
			_getws(wss);
			wcscpy(wstr, wss);
			wcscat(wss, L".mkk");
			wcscat(wstr, L".mhk");

			fpub = _wfopen(wss, L"wb");
			if(fpub==NULL)exit(1);
			fpri = _wfopen(wstr, L"wb");
			if(fpri==NULL)exit(1);

			esign_kagiseisei(&epubkey, &eprikey, pLen, op);
			printf("p = \n");utbi_putx(eprikey.p);puts("");
			printf("q = \n");utbi_putx(eprikey.q);puts("");
			printf("n = \n");utbi_putx(eprikey.n);puts("");
			printf("pLen = %d\n", eprikey.pLen);
			printf("e = %d\n", eprikey.e);
			/*
			fwprintf(fpub, L"n =\n");utbi_fputx(fpub, epubkey.n);fwprintf(fpub, L"\n");
			fwprintf(fpub, L"pLen =\n");
			fwprintf(fpub, L"%d\n", epubkey.pLen);
			fwprintf(fpub, L"e =\n");
			fwprintf(fpub, L"%d\n", epubkey.e);
			*/
			esign_PubkeyFout(fpub, epubkey);
			/*
			fwprintf(fpri, L"p =\n");utbi_fputx(fpri, eprikey.p);fwprintf(fpri, L"\n");
			fwprintf(fpri, L"q =\n");utbi_fputx(fpri, eprikey.q);fwprintf(fpri, L"\n");
			fwprintf(fpri, L"n =\n");utbi_fputx(fpri, eprikey.n);fwprintf(fpri, L"\n");
			fwprintf(fpri, L"pLen =\n%d\n", eprikey.pLen);
			fwprintf(fpri, L"e =\n%d\n", eprikey.e);
			*/
			esign_PrikeyFout(fpri, eprikey);
			printf("case 1 end\n");
			getchar();

			free(eprikey.p);
			fclose(fpub);
			fclose(fpri);
			break;
		case 2:
			/*署名生成*/
			wprintf(L"秘密鍵ファイル名？\n");
			wprintf(L"->");_getws(wss);

			fpri = _wfopen(wss, L"rb");
			if(fpri==NULL)exit(1);
			esign_PrikeyFin(&eprikey, fpri);

			utbi_memory(&sign.s, 1);
			
			wprintf(L"署名対称ファイル名？\n");
			wprintf(L"->");_getws(sign.fname);
			wcscpy(wstr, sign.fname);
			wcscat(wstr, L".msm");
			fs = _wfopen(wstr, L"wb");
			if(fs ==NULL)
				exit(1);
			sign.hLen = 512;
			sign.lLen = eprikey.pLen;

			if(ssa_esign_sign(&sign, &eprikey)==GOOD){
                printf("s = \n");utbi_putx(sign.s);puts("");
				esign_SignFout(fs, sign);
			}else{
				printf("ssa_esign_sign error\n");
			}
			fclose(fs);fclose(fpri);
			free(sign.s);free(eprikey.p);
			printf("case 2 end\n");
			getchar();
			break;
		case 3:
			/*署名検証*/
			wprintf(L"公開鍵ファイル名？\n");
			wprintf(L"->");_getws(wss);
			wcscpy(wss, L"umedoblock.mkk");

			fpub = _wfopen(wss, L"rb");
			if(fpub==NULL)exit(1);
			esign_PubkeyFin(&epubkey, fpub);

			utbi_memory(&verify.s, 1);
			
			wprintf(L"署名検証ファイル名？\n");
			wprintf(L"->");_getws(verify.fname);
			wcscpy(verify.fname, L"a.txt");
			wcscpy(wstr, verify.fname);
			wcscat(wstr, L".msm");

			fv = _wfopen(wstr, L"rb");
			if(fv==NULL){
				exit(1);
			}

			esign_VerifyFin(&verify, fv);

			if(ssa_esign_verify(&verify, &epubkey)==GOOD){
				printf("good signature\n");
			}else{
				printf("bad signature\n");
			}
			printf("esign test finish\n");
			getchar();
			free(epubkey.n);
			free(verify.s);
	}
	return 0;
}
