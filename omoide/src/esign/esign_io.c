/* esign_io.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "esign.h"
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

void esign_PubkeyFout(FILE *fpub, ESIGN_PUBLICKEY pubkey)
{
	fwprintf(fpub, L"pLen =\r\n");
	fwprintf(fpub, L"%d\r\n", pubkey.pLen);

	fwprintf(fpub, L"e =\r\n");
	fwprintf(fpub, L"%d\r\n", pubkey.e);

	fwprintf(fpub, L"n =\r\n");
	utbi_fputx(fpub, pubkey.n);fwprintf(fpub, L"\r\n");

}
int esign_PubkeyFin(ESIGN_PUBLICKEY *pubkey, FILE *fpub)
{
	extern int yousosuu;
	wchar_t *wss, wstr[20];
/*	fwprintf(fpub, L"pLen =\r\n");*/
	fgetws(wstr, 15, fpub);
/*	fwprintf(fpub, L"%d\r\n", pubkey.pLen);*/
	fgetws(wstr, 15, fpub);
	pubkey->pLen = (unt)_wtol(wstr);

/*	fwprintf(fpub, L"e =\r\n");*/
	fgetws(wstr, 15, fpub);
/*	fwprintf(fpub, L"%d\r\n", pubkey.e);*/
	fgetws(wstr, 15, fpub);
	pubkey->e = (unt)_wtoi(wstr);

	utbi_bitset(3*pubkey->pLen);
	if(utbi_memory(&pubkey->n, 1)==0){
		return 0;
	}
	wss = (wchar_t *)malloc(sizeof(wchar_t)*8*yousosuu+10);
	if(wss==NULL){
		free(pubkey->n);
		return 0;
	}

/*	fwprintf(fpub, L"n =\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpub);
/*	utbi_fputx(fpub, pubkey.n);fwprintf(fpub, L"\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpub);
	utbi_xtoi(pubkey->n, wss);

	free(wss);
	return 1;
}

void esign_PrikeyFout(FILE *fpri, ESIGN_PRIVATEKEY prikey)
{
	fwprintf(fpri, L"pLen =\r\n");
	fwprintf(fpri, L"%d\r\n", prikey.pLen);

	fwprintf(fpri, L"e =\r\n");
	fwprintf(fpri, L"%d\r\n", prikey.e);

	fwprintf(fpri, L"p =\r\n");
	utbi_fputx(fpri, prikey.p);fwprintf(fpri, L"\r\n");

	fwprintf(fpri, L"q =\r\n");
	utbi_fputx(fpri, prikey.q);fwprintf(fpri, L"\r\n");

	fwprintf(fpri, L"n =\r\n");
	utbi_fputx(fpri, prikey.n);fwprintf(fpri, L"\r\n");

}
int esign_PrikeyFin(ESIGN_PRIVATEKEY *prikey, FILE *fpri)
{
	wchar_t *wss, wstr[20], *p;
	unt i;

	
	/*fwprintf(fpri, L"pLen =\r\n");*/
	fgetws(wstr, 15, fpri);
	/*fwprintf(fpri, L"%d\r\n", prikey.pLen);*/
	fgetws(wstr, 15, fpri);
	prikey->pLen=(unt)_wtoi(wstr);

	/*fwprintf(fpri, L"e =\r\n");*/
	fgetws(wstr, 15, fpri);
	/*fwprintf(fpri, L"%d\r\n", prikey.e);*/
	fgetws(wstr, 15, fpri);
	prikey->e=(unt)_wtoi(wstr);

	utbi_bitset(3*prikey->pLen);
	if(utbi_memory(&prikey->p, 4)==0){
		return 0;
	}
	prikey->q = prikey->p + yousosuu;
	prikey->pq = prikey->q + yousosuu;
	prikey->n = prikey->pq + yousosuu;

	i=sizeof(wchar_t)*8*yousosuu+10;
	wss = (wchar_t *)malloc((size_t)i);
	if(wss==NULL){
		free(prikey->p);
		return 0;
	}
	p = wss;
	/*fwprintf(fpri, L"p =\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpri);
	/*utbi_fputx(fpri, prikey.p);fwprintf(fpri, L"\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpri);
	utbi_xtoi(prikey->p, wss);

	/*fwprintf(fpri, L"q =\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpri);
	/*utbi_fputx(fpri, prikey.q);fwprintf(fpri, L"\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpri);
	utbi_xtoi(prikey->q, wss);

	/*fwprintf(fpri, L"n =\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpri);
	/*utbi_fputx(fpri, prikey.n);fwprintf(fpri, L"\r\n");*/
	fgetws(wss, 8*yousosuu+5, fpri);
	utbi_xtoi(prikey->n, wss);

	while(*wss!=L'\0'){
		*wss = L'\0';
		wss++;
	}

	for(i=0;i<20;i++){
		wstr[i] = L'\0';
	}

	free(p);
	return 1;
}

void esign_SignFout(FILE *fout, ESIGN_SIGN sign)
{
	fwprintf(fout, L"s =\r\n");
	utbi_fputx(fout, sign.s);fwprintf(fout, L"\r\n");

	fwprintf(fout, L"hLen =\r\n");
	fwprintf(fout, L"%d\r\n", sign.hLen);

	fwprintf(fout, L"lLen =\r\n");
	fwprintf(fout, L"%d\r\n", sign.lLen);
}

int esign_VerifyFin(ESIGN_VERIFY *verify, FILE *fin)
{
	wchar_t *wss;

	wss = (wchar_t *)malloc(sizeof(wchar_t)*8*yousosuu+10);
	if(wss == NULL){
		return 0;
	}

	/*fwprintf(fout, L"s =\r\n");*/
	fgetws(wss, 8*yousosuu+10, fin);
	/*utbi_fputx(fout, sign.s);fwprintf(fout, L"\r\n");*/
	fgetws(wss, 8*yousosuu+10, fin);
	utbi_xtoi(verify->s, wss);

	/*fwprintf(fout, L"hLen =\r\n");*/
	fgetws(wss, 8*yousosuu+10, fin);
	/*fwprintf(fout, L"%d\r\n", sign.hLen);*/
	fgetws(wss, 8*yousosuu+10, fin);
	verify->hLen = (unt)_wtoi(wss);

	/*fwprintf(fout, L"lLen =\r\n");*/
	fgetws(wss, 8*yousosuu+10, fin);
	/*fwprintf(fout, L"%d\r\n", sign.lLen);*/
	fgetws(wss, 8*yousosuu+10, fin);
	verify->lLen = (unt)_wtoi(wss);

	free(wss);
	return 1;
}

