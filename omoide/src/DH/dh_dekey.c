/* dh_dekey.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "DH.h"

/*2005/04/02*/
void dh_dekey(DH_SHAREKEY *shrkey, DH_PRIVATEKEY *prikey)
{
	/*送られてきた鍵を秘密鍵乗する。*/
	utbi_bekijouyo(shrkey->gxy, shrkey->gy, prikey->x, prikey->p);
}

