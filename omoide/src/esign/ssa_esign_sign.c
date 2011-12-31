/* ssa_esign_sign.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "esign.h"

int ssa_esign_sign(ESIGN_SIGN *sign, ESIGN_PRIVATEKEY *prikey)
{
	int flg;
	unt *f;

	if(utbi_memory(&f, 1)==0){
		flg = INVALID;
		goto FLAG;
	}

	if(emsa_esign_encode(f, sign) == INVALID){
		puts("error emsa_esign_encode");
		flg = INVALID;
		goto FLAG;
	}
	if(SP_esign(sign, prikey, f) == INVALID){
		puts("error SP_esign");
		flg = INVALID;
		goto FLAG;
	}
	flg = GOOD;

	FLAG:
	free(f);

	return flg;
}
