/* dh_keygen.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

/*2005/04/02*/
#include "DH.h"

void dh_keygen(DH_PUBLICKEY *pubkey, DH_PRIVATEKEY *prikey, unt mode)
{
	unt *gcd, *p_1;
	extern int yousosuu;

	mode = 0;
	if(utbi_memory(&gcd, 2)==0){
		return;
	}
	p_1 = gcd + yousosuu;

	utbi_sa_ui(p_1, prikey->p, 1);

	/*色々と先に複写しておく*/
	utbi_fukusha(pubkey->p, prikey->p);
	utbi_fukusha(pubkey->g, prikey->g);
	pubkey->pLen = prikey->pLen;

	/*乱数生成&秘密鍵生成*/
	utbi_ransuu(prikey->x, prikey->pLen);
	*prikey->x |= 0x00000001;
	utbi_gcd(gcd, prikey->x, p_1);

	/*xとpが互いに素になるまで乱数を作り続ける*/
	while(utbi_futougou_ui(gcd, 1) != 0){
		utbi_ransuu(prikey->x, prikey->pLen);
		*prikey->x |= 0x00000001;
		utbi_gcd(gcd, prikey->x, p_1);
	}

	/*公開鍵生成*/
	utbi_bekijouyo(pubkey->gx, prikey->g, prikey->x, prikey->p);


	free(gcd);
}
