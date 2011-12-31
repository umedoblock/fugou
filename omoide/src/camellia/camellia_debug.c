/* camellia_debug.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"
#include <string.h>
#include <ctype.h>

#define CAMELIADEBUGFILE "z:\visual_studio.net\utbi_\camellia\t_camellia.txt"
#define chtox(ch) (ch-((ch>'9') ? 'a' - 10: '0'))
int camellia_keystring(CAMELLIA_KEY *dbgkey, uchar *keyString, unt keysize)
{
	unt i;
	uchar *p = (uchar *)dbgkey->key;
	i=0;
	while(keyString[i] != '\0'){
		if(isupper((int)keyString[i])){
			keyString[i] = (uchar)tolower(keyString[i]);
		}
	}
	
	dbgkey->keysize = keysize;
	for(i=0;i<dbgkey->keysize/8;i++){
		p[i] = (chtox(keyString[2*i]) << 4) | (chtox(keyString[2*i+1]) & 0xf);
	}
	return 1;
}
/*
 * 特に必要ない。
int camellia_debug(void)
{
	char ss[130];
	FILE *fi;
	int i, flg;
	CAMELLIA_KEY cmkey;
	
	
	if((fi = fopen(CAMELLIADEBUGFILE, "rb")) == NULL){
		fprintf(stderr, "ERROR NOT FOUND :%s\n", CAMELLIADEBUGFILE);
		return 0;
	}
	return 1;
}	
*/	
	
