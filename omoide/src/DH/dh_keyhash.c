/* dh_keyhash.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "DH.h"

/*2005/04/11*/
int dh_keyhash(SHA512 *ks, DH_SHAREKEY *shrkey)
{
	uchar *buf;
	unt Bytes;
	extern int yousosuu;

	Bytes = (sizeof(uchar)*4*yousosuu);
	buf = (uchar *)malloc(Bytes);
	if(buf==NULL){
		return 0;
	}

	utbi_itodata(buf, shrkey->gxy);

	ks = NULL;
	//sha512_Data(ks, buf, Bytes);

	for(;Bytes>0;Bytes--){
		buf[Bytes-1] = 0;
	}

	free(buf);
	return 1;
}




