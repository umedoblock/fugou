/* emsa_esign_encode.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "esign.h"
int emsa_esign_encode();

int emsa_esign_encode(unt *f, ESIGN_SIGN *sign)
{
	unt *PS, *F, *tmp;
	int i;
	extern int yousosuu;

	if(utbi_memory(&tmp, 2)==0){
		exit(1);
	}
	PS = tmp;
	F = PS + yousosuu;

	/*1. もし、Ｍのビット長がハッシュ関数の入力ビット幅を超えていたならば、*/
	/*   invalid を返す*/
	/*if(hLen > 512)==error*/
	/*だって、どうでもesignだから～*/

	/*2. l-16 < sign->hLen ならINVALID を返してエラー処理*/
	if(sign->lLen-16 < sign->hLen){
		puts("sign->hLen-16<sign->hLen");
		return INVALID;
	}

	/*3. Mをハッシュ値に変換*/
	if(*sign->fname != L'\0'){
        sha512_FileName(&sign->H, sign->fname);
	}else{
		/*もうすでにsign->Hにハッシュ値はセットされている*/
	}
	/*4. l-16-sign->hLen はここでは常に偶数*/
	/*l_esignyousosuu * 32*/
	/*PSは l-16-sign->hLen ビット*/
	/*4. PS = 101010・・・・・101010*/
	/*PSの長さは l-16-sign->hLen*/
	for(i=0;(unt)i<yousosuu;i++){
		*(PS+i) = 0xaaaaaaaa;
	}
	utbi_bitzurashi_m_si(PS, PS, ((32*yousosuu) -((32*yousosuu/3-16-(sign->hLen))))%32);
	utbi_bitzurashi_m32_si(PS, PS, ((32*yousosuu) -((32*yousosuu/3-16-(sign->hLen))))/32);
	/*PSを作った*/

	/*5. F = S2BSP(0,8) || PS || OS2BSP(255,8) || H */
	/*Fを0で初期化*/
	utbi_shokika(F);
	utbi_bitzurashi_h_si(PS, PS, 8);
	*(PS) |= 0x000000ff;
	utbi_bitzurashi_h32_si(PS, PS, (sign->hLen/32));
	utbi_ior(F, F, PS);

	for(i=0;(unt)i<(sign->hLen)/32;i++){
		*(F+i) = sign->H.hash[i/2][i%2];
	}

	/*6. f=BS2IP(F) */
	utbi_fukusha(f, F);

	free(tmp);

	return GOOD;
}
