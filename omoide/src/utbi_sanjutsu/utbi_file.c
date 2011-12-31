/* utbi_file.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_fwrite(FILE *fp, unt *x)
{
	unt i;
	uchar tmp, *p, *pp;
	extern int yousosuu;

	p = (uchar *)x;
	pp = p + sizeof(unt)*yousosuu - 1;

	for(i=0;i<(sizeof(unt)/2)*yousosuu;i++){
		tmp = *p;
		*p = *pp;
		*pp = tmp;
		p++;
		pp--;
	}

	p = (uchar *)x;
	pp = p + sizeof(unt)*yousosuu - 1;

	fwrite(p, sizeof(uchar), sizeof(unt)*yousosuu, fp);

	for(i=0;i<(sizeof(unt)/2)*yousosuu;i++){
		tmp = *p;
		*p = *pp;
		*pp = tmp;
		p++;
		pp--;
	}
	p = pp = NULL;
	tmp = 0;
	i = 0;
}
unt utbi_fread(unt *x, FILE *fp)
{
	extern int yousosuu;
	unt i;
	uchar tmp, *p, *pp;
	unt t;

	t = (unt)fread((uchar *)x, sizeof(uchar), sizeof(unt)*yousosuu, fp);

	if(t){
		p = (uchar *)x;
		pp = p + sizeof(unt)*yousosuu - 1;

		for(i=0;i<(sizeof(unt)/2)*yousosuu;i++){
			tmp = *p;
			*p = *pp;
			*pp = tmp;
			p++;
			pp--;
		}

		p = pp = NULL;
		tmp = 0;
		i = 0;
	}

	return t;

}

