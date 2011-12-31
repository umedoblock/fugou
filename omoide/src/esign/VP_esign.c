/* VP_esign.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
/*#include "../\\utbi_h\\utbi_sanjutsu\\utbi_sanjutsu.h"*/
#include "esign.h"

int VP_esign(unt *f_, ESIGN_VERIFY *verify, ESIGN_PUBLICKEY *pubkey)
{
	unt *T, *_2_2pLen, *ff, *tmp;
	extern int yousosuu;

	if(utbi_memory(&tmp, 3)==0){
		exit(1);
	}
	T = tmp;
	_2_2pLen = tmp + yousosuu;
	ff = _2_2pLen + yousosuu;

	/*1. s が 0 <= s < n でなかったら、"INVALID"を出力して終了*/
	if((utbi_futougou_ui(verify->s, 0) >= 0) && (utbi_futougou(verify->s, pubkey->n) < 0)){
		;
	}else{
		/*puts("署名値がおかしいようです。");
		puts("終了します。\n");*/
		return INVALID;
	}

	/*2. T = s^e mod n */
	utbi_bekijouyo_ui(T, verify->s, pubkey->e, pubkey->n);

	/*3. f=T/(2^(2*pLen)) */
	utbi_bitzurashi_m32_si(f_, T, (2*pubkey->pLen)/32);

	*(_2_2pLen) = 0x00000001;
	utbi_bitzurashi_h_si(_2_2pLen, _2_2pLen, (2*pubkey->pLen)%32);
	utbi_bitzurashi_h32_si(_2_2pLen, _2_2pLen, (2*pubkey->pLen)/32);

	/*4. f_が 0 <= f_ < 2^(pLen-1) でなかったら、"INVALID"と出力して終了*/
	/*2^(pLen-1)を作る*/
	utbi_bitzurashi_m(_2_2pLen, _2_2pLen);

	if((utbi_futougou_ui(f_, 0) >= 0) && (utbi_futougou(f_, _2_2pLen) < 0)){
		;
	}else{
        /*puts("error VP_esign");
		puts("不正な署名のようです。\n");*/
		free(tmp);
		return INVALID;
	}

	/*5. f_を出力 */
	/*utbi_fukusha(f_);*/

	free(tmp);

	return GOOD;
}
