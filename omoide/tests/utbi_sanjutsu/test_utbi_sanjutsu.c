/* test_utbi_sanjutsu.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../src/utbi_sanjutsu.h"

int main(int argc, char *argv[])
{
	unt *x, *y, *z;
	unt *w, *v, *f, *g, *tmp;
	int i, j, k;
	int n, m;
	int bit = 2048;
	char ss[80];
	clock_t s[8], e[8];
	double t[8];
	extern int yousosuu;
	int end;

	if(argc < 3){
		fprintf(stdout, "n, bit please\n");
		return -1;
	}

	n   = atoi(argv[1]);
	if(n <= 0) n = 10;

	bit = atoi(argv[2]);
	if(bit <= 0) bit = 128;

	fprintf(stdout, "n = %d, bit = %d\n", n, bit);

	utbi_bitset(bit);

	utbi_memory(&tmp, 7);
	x = tmp;
	y = x + yousosuu;
	z = y + yousosuu;
	w = z + yousosuu;
	v = w + yousosuu;
	f = v + yousosuu;
	g = f + yousosuu;

	s[0] = clock();
	for(i=0;i<n;i++){
		utbi_ransuu(x, bit);
		utbi_ransuu(y, bit);

		fprintf(stdout, "x = 0x");utbi_put0x(x);fprintf(stdout, "\n");
		fprintf(stdout, "y = 0x");utbi_put0x(y);fprintf(stdout, "\n");
		utbi_wa(z, x, y);
		fprintf(stdout, "x + y = 0x");utbi_put0x(z);fprintf(stdout, "\n");
		utbi_sa(z, x, y);
		fprintf(stdout, "x - y = 0x");utbi_put0x(z);fprintf(stdout, "\n");
		utbi_seki(z, x, y);
		fprintf(stdout, "x * y = 0x");utbi_put0x(z+yousosuu);utbi_put0x(z);fprintf(stdout, "\n");
	}
	e[0] = clock();

	s[1] = e[1] = clock();

	for(i=0;i<2;i++){
		t[i] = (double )(e[i]-s[i]) / CLOCKS_PER_SEC;
		fprintf(stdout, "t[%d]=%f\n", i, t[i]);
	}

	free(tmp);
	return 0;
}

