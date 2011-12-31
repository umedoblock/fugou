/* sha_random.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "sha.h"

#include <stdio.h>
#include <stdlib.h>

void sha512_randomData(uchar *data, unt Byte, SHA512 seed)
{
	uchar *hp;
	int i;
	unt j;

	i=0;
	while(Byte>512/8){
		Byte -= 512/8;
		hp = (uchar *)&seed.hash[7][1] + 3;
		for(j=0;j<512/8;j++,i++){
			data[i] = *hp;
			hp--;
		}

		sha512_Data(&seed, &data[i-512/8], 512/8);
	}

	hp = (uchar *)&seed.hash[7][1] + 3;
	for(j=0;j<Byte;j++,i++){
		data[i] = *hp;
		hp--;
	}

	sha512_clear(&seed);
}

void sha512_randomFile(char *filename, unt Byte, SHA512 seed)
{
	FILE *fout;

	uchar buf[512/8], *hp;
	unt i, j;

	fout = fopen(filename, "wb");
	if(fout == NULL){
		exit(1);
	}

	i=0;
	while(Byte>512/8){
		Byte -= 512/8;
		hp = (uchar *)&seed.hash[7][1] + 3;
		for(j=0;j<512/8;j++){
			buf[j] = *hp;
			hp--;
		}
		fwrite(buf, sizeof(uchar), 512/8, fout);

		sha512_Data(&seed, buf, 512/8);
	}

	hp = (uchar *)&seed.hash[7][1] + 3;
	for(j=0;j<Byte;j++){
		buf[j] = *hp;
		hp--;
	}
	fwrite(buf, sizeof(uchar), Byte, fout);

	for(i=0;i<512/8;i++){
		buf[i] = 0;
	}

	sha512_clear(&seed);

	fclose(fout);
}
