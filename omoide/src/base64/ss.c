/* ss.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "base64.h"


void strtodata(uchar *data, char *ss, unt n)
{
	unt i;
	uchar *bp = data;
	int ch;

	for(i=0;i<n;i++){
		ch = *ss;
		*bp++ = ch >> 8;
		*bp++ = ch;
		ss++;
	}
}

void datatostr(char *ss, uchar *data, unt n)
{
	uchar *bp = data;
	unt i;
	int ch;

	for(i=0;i<n/2;i++){
		ch = 0;
		//*ss <<= 8;
		ch |= *bp++;
		ch <<= 8;
		ch |= *bp++;
		*ss = ch;
		ss++;
	}
}
