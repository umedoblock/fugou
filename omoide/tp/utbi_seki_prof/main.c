/* main.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#define _UNICODE
#include "../utbi_h\\utbi_sanjutsu\\utbi_sanjutsu.h"
#include <stdio.h>
#include <wchar.h>
#include <time.h>

int main(void)
{
	unt *x, *y, *z, *w, *v, *f, *g, *tmp;
	int i, j, k;
	int n, m;
	int bit = 2048;
	char ss[80];
	clock_t s[8], e[8];
	double t[8];
	extern int yousosuu;
	int end;

	fprintf(stdout, "n->");fgets(ss, 80 -1, stdin);
	n = atoi(ss);

	utbi_bitset(bit);

	utbi_memory(&tmp, 7);
	x = tmp;
	y = x + yousosuu;
	z = y + yousosuu;
	w = z + yousosuu;
	v = w + yousosuu;
	f = v + yousosuu;
	g = f + yousosuu;

	utbi_ransuu(x, bit);
	utbi_ransuu(y, bit);

	s[0] = clock();
	for(i=0;i<n;i++){
		utbi_seki(x, x, y);
	}
	e[0] = clock();

	end = bit / 6 + (64 - 2);
	s[1] = clock();
	for(i=0;i<n;i++){
		for(j=0;j<end;j++){
			utbi_wa(x, x, y);
		}
	}
	e[1] = clock();

	for(i=0;i<2;i++){
		t[i] = (double )(e[i]-s[i]) / CLOCKS_PER_SEC;
		fprintf(stdout, "t[%d]=%f\n", i, t[i]);
	}

	free(tmp);
	return 0;
}