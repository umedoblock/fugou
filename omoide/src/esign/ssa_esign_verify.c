/* ssa_esign_verify.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "esign.h"

int ssa_esign_verify(ESIGN_VERIFY *verify, ESIGN_PUBLICKEY *pubkey)
{
	int flg;
	unt *f_=NULL;

	if(utbi_memory(&f_, 1)==0){
		flg = INVALID;
		goto FLAG;
	}

	if(VP_esign(f_, verify, pubkey)==INVALID){
		flg = INVALID;
		goto FLAG;
	}

	if(emsa_esign_verify(verify, f_) == GOOD){
		flg = GOOD;
		goto FLAG;
	}else{
		flg = INVALID;
		goto FLAG;
	}

	FLAG:

	free(f_);

	return flg;
}
