/* dh_enkey.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "DH.h"

/*2005/04/02*/
void dh_enkey(DH_SHAREKEY *shrkey, DH_PUBLICKEY *pubkey, unt mode)
{
	/*共通鍵用の乱数作り*/
	if(mode){
        utbi_ransuu(shrkey->y, pubkey->pLen);
	}

	/*受信者に送るための鍵を作る*/
	utbi_bekijouyo(shrkey->gy, pubkey->g, shrkey->y, pubkey->p);

	/*暗号化用の鍵を求める*/
	utbi_bekijouyo(shrkey->gxy, pubkey->gx, shrkey->y, pubkey->p);

}

