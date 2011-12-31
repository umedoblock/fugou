/* dh_clear.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "DH.h"

/*2005/04/10*/

/*2005/04/10*/
void dh_priclear(DH_PRIVATEKEY *prikey)
{
	utbi_shokika(prikey->x);
}
/*2005/04/10*/
void dh_shrclear(DH_SHAREKEY *shrkey)
{
	utbi_shokika(shrkey->gxy);
	utbi_shokika(shrkey->y);
}

