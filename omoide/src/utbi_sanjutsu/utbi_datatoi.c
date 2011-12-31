/* utbi_datatoi.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_datatoi(unt *x, uchar *data, unt nBytes)
{
	int i;

	utbi_shokika(x);

	for(i=0;i<(int)nBytes;i++){
		utbi_bitzurashi_h_si(x, x, 8);
		*x |= *data;
		data++;
	}

}

void utbi_itodata(uchar *data, unt *x)
{
	int i;
	uchar *p;
	extern int yousosuu;

	p = (uchar *)x;
	p += (4*yousosuu-1);

	for(i=0;i<4*yousosuu;i++){
		*data = *p;
		data++;
		p--;
	}
}
