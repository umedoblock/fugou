/* camellia_hash.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"
/*
void camellia_hashset(CAMELLIA_KEY *ck, SHA512 *H)
{

	if(ck->keysize == 128){
		camellia_xor(ck->key[1], H->hash[7], H->hash[5]);
		camellia_xor(ck->key[1], ck->key[1], H->hash[3]);
		camellia_xor(ck->key[1], ck->key[1], H->hash[1]);

		camellia_xor(ck->key[0], H->hash[6], H->hash[4]);
		camellia_xor(ck->key[0], ck->key[0], H->hash[2]);
		camellia_xor(ck->key[0], ck->key[0], H->hash[0]);

	}else if(ck->keysize == 192){
		camellia_xor(ck->key[2], H->hash[7], H->hash[4]);
		camellia_xor(ck->key[2], ck->key[2], H->hash[1]);

		camellia_xor(ck->key[1], H->hash[6], H->hash[3]);
		camellia_xor(ck->key[1], ck->key[1], H->hash[0]);

		camellia_xor(ck->key[0], H->hash[5], H->hash[2]);

	}else{
		// ck->keysize == 256
		camellia_xor(ck->key[3], H->hash[7], H->hash[3]);

		camellia_xor(ck->key[2], H->hash[6], H->hash[2]);

		camellia_xor(ck->key[1], H->hash[5], H->hash[1]);

		camellia_xor(ck->key[0], H->hash[4], H->hash[0]);

	}

}
*/

