/* camellia_keygen.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

#ifndef ___BIGENDIAN___
#define bbswap(x); x=(x>>24)|(x<<24)|((x&0xff0000)>>8)|((x&0xff00)<<8);
#endif

void camellia_keygen(CAMELLIA_KEY *ck)
{
#ifdef ___ASSEMBSE32___
	int i;
#endif
#ifdef ___BIGENDIAN___
	int i;
#endif
#ifdef ___ASSEMBLE32___
	#define ssswap(x, y); x^=y^=x^=y;
	static int i_asm32 = 1;

	if(i_asm32){
        camellia_SP32();
		i_asm32 = 0;
	}
#endif
	camellia_subkey(ck->subkey, ck->key, ck->keysize);
	camellia_subkey_all(ck->subkey_all, ck->subkey, ck->keysize);

#ifdef ___ASSEMBLE32___
	if(ck->keysize==128){
		for(i=0;i<26;i++){
			ssswap(ck->subkey_all[i][0], ck->subkey_all[i][1]);
		}
	}else{
		/*192, 256*/
		for(i=0;i<34;i++){
			ssswap(ck->subkey_all[i][0], ck->subkey_all[i][1]);
		}
	}
#ifndef ___BIGENDIAN___
	if(ck->keysize==128){
		for(i=0;i<26;i++){
			bbswap(ck->subkey_all[i][0]);
			bbswap(ck->subkey_all[i][1]);
		}
	}else{
		/*192, 256*/
		for(i=0;i<34;i++){
			bbswap(ck->subkey_all[i][0]);
			bbswap(ck->subkey_all[i][1]);
		}
	}
#endif

#endif


}

void camellia_keyset(CAMELLIA_KEY *ck, uchar *data, int kLen)
{
	unt i;
	uchar *p = (uchar *)ck->key;

	ck->keysize = kLen;
	for(i=0;i<ck->keysize/8;i++){
		p[ck->keysize/8-1-i] = data[i];
	}
}


