/* emsa_esign_verify.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "esign.h"

int emsa_esign_verify(ESIGN_VERIFY *verify, unt *f_)
{
	unt *T, *PS, *F_, *F__, *M_, *hashM, *tmp;
	int i;
	extern int yousosuu;
	/* verify->lLen = 32*yousosuu */

	if(utbi_memory(&tmp, 6)==0){
		return INVALID;
	}
	T = tmp;
	PS = T + yousosuu;
	F_ = PS + yousosuu;
	F__ = F_ + yousosuu;
	M_ = F__ + yousosuu;
	hashM = M_ + yousosuu;

	utbi_shokika(T);utbi_shokika(PS);
	utbi_shokika(F_);utbi_shokika(F__);
	utbi_shokika(M_);utbi_shokika(hashM);
	/*1. */
	if(verify->lLen-16<verify->hLen){
		return INVALID;
	}

	/*2. T = I2BSP(f, l) */
	utbi_fukusha(T, f_);

	/*3. PS = 101010・・・・・101010*/
	/*PSの長さは l-16-verify->hLen*/
	for(i=0;(unt)i<(verify->lLen-verify->hLen)/32;i++){
		*(PS+i) = 0xaaaaaaaa;
	}
	utbi_bitzurashi_m_si(PS, PS, 16);
	/*
	for(i=0;(unt)i<yousosuu;i++){
		*(PS+i) = 0xaaaaaaaa;
	}
	utbi_bitzurashi_m_si(PS, PS, ((32*yousosuu) -((32*yousosuu/3-16-verify->hLen)))%32);
	utbi_bitzurashi_m32_si(PS, PS, ((32*yousosuu) -((32*yousosuu/3-16-verify->hLen)))/32);
	*/
	/*PSを作った*/

	/*4-1. Tの上位esign.pLen - verify->hLenビットを取り出してF__とする*/
	utbi_fukusha(F__, T);
	utbi_bitzurashi_m_si(F__, F__, verify->hLen%32);
	utbi_bitzurashi_m32_si(F__, F__, verify->hLen/32);
/*	utbi_bitzurashi_m_si(F__, F__, (32*yousosuu-(esign.pLen - verify->hLen))%32);
	utbi_bitzurashi_m32_si(F__, F__, (32*yousosuu-(esign.pLen - verify->hLen))/32);*/

	/*4-2. F_をF_= OS2BSP(0, 8) || PS || OS2BSP(255, 8) とする*/
	utbi_bitzurashi_h_si(PS, PS, 8);
	*(PS) |= 0x000000ff;
	utbi_ior(F_, F_, PS);

	/*4-3. F_ == F__ が成立するかどうか検証*/
	/*成立しない場合　INVALID　を返す。*/
	if(utbi_futougou(F_, F__) != 0){
		return INVALID;
	}

	/*5. Tの下位verify->hLenビットを取り出したものをM_とする。*/
	/*TとM_の値の並び順は一緒*/
	/*M_ = BS2OSP([T]_verify->hLen, verify->hLen) */
	for(i=(verify->hLen/32)-1;i>=0;i--){
		*(M_+i) = *(T+i);
	}

	/*6-1. Mのハッシュ値を取る*/
	if(*verify->fname!=L'\0'){
		sha512_FileName(&verify->H, verify->fname);       
	}else{
		/*もうすでにハッシュ値はHにセットされている。*/
	}
	utbi_shokika(hashM);
	/*6-2. Hash(M)にハッシュ値を複写*/
 	for(i=(verify->hLen/32)-1;i>=0;i--){
		*(hashM+i) = verify->H.hash[i/2][i%2];
	}

	/*6-3. Hash(M)とM_が同じならば"GOOD"*/
	/*そうでないならば INVALID を返す。*/
	if(utbi_futougou(hashM, M_) == 0){
		free(tmp);
		return GOOD;
	}else{
		free(tmp);
		return INVALID;
	}
}
