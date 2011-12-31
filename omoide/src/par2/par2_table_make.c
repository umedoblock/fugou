/* par2_table_make.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "par2.h"

int gf[PAR2_COUNT], gfi[PAR2_COUNT];

int par2_init(void)
{
	int i, j;
	for (i = 0, j = 1; i < PAR2_MAX; i++) {
		gf[j] = i; gfi[i] = j;
		if (PAR2_MAX < (j <<= 1)) j ^= PAR2_PRIME_POLY;
	}
	return 0;
}

int par2_add(int a, int b)
{
	return a ^ b;
}

// PAR2用掛け算
int par2_mult__(int a, int b)
{
	int c;
	if (a == 0 || b == 0) return 0;
	if ((c = gf[a] + gf[b]) < PAR2_MAX) return gfi[c];
	return gfi[c - PAR2_MAX];
}

// PAR2用割り算(b=0は本来あってはならない)
int par2_div(int a, int b)
{
	int c;
	if (a == 0) return 0;
	//都合上変更。
	if (b == 0) return 0xff;
	if (0 <= (c = gf[a] - gf[b])) return gfi[c];
	return gfi[c + PAR2_MAX];
}

//a の x 乗
//分割するときに必要になる。
int par2_f(int a, int x)
{
	int i, n = 1;
	if(a==0)return 0;
	if(a==256){
		a = par2_mult__(16, 16);
	}
	for(i=0;i<x;i++){
		n = par2_mult__(a, n);
	}
	return n;
}

int add_func(int j, int i)
{
	return par2_add(j, i);
}

int mul_func(int j, int i)
{
	return par2_mult__(j, i);
}

int div_func(int j, int i)
{
	return par2_div(j, i);
}

int f_func(int j, int i)
{
	if(i+1>256)return -1;
	return par2_f(i+1, j);
}

int foo(int (*func)(int, int))
{
	int i, j, tmp;

	fprintf(stdout, "[%d][%d] = {\n", PAR2_COUNT, PAR2_COUNT);
	for(j=0;j<PAR2_COUNT;j++){
		fprintf(stdout, "\t{");
		for(i=0;i<PAR2_COUNT;i++){
			tmp = func(j, i);
			fprintf(stdout, "0x%02x", tmp);
			if(i!=PAR2_COUNT-1){
				fprintf(stdout, ", ");
			}
		}
		fprintf(stdout, "},\n");
	}
	fprintf(stdout, "};\n");
}

int main(void)
{
	par2_init();
	//fprintf(stdout, "unsigned char par2_add_%d", PAR2_BIT);
	//foo(add_func);
	fprintf(stdout, "unsigned char par2_mul_%d", PAR2_BIT);
	foo(mul_func);
	//fprintf(stdout, "unsigned char par2_div_%d", PAR2_BIT);
	//foo(div_func);
	//fprintf(stdout, "unsigned char par2_f_%d", PAR2_BIT);
	//foo(f_func);
	fprintf(stdout, "\n");
	fprintf(stderr, "%d\n", par2_f(3, 0));
	fprintf(stderr, "%d\n", par2_f(3, 1));
	fprintf(stderr, "%d\n", par2_f(3, 2));
	fprintf(stderr, "%d\n", par2_f(3, 3));
	return 0;
}
/*
	for(j=0;j<PAR2_COUNT;j++){
		fprintf(stdout, "{");
		for(i=0;i<PAR2_COUNT;i++){
			if(i!=PAR2_COUNT-1){
				fprintf(stdout, "0x%02x, ", par2_add(gf[j], gf[i]));
			}
		}
		fprintf(stdout, "},\n");
	}
	for(j=0;j<PAR2_COUNT;j++){
		fprintf(stdout, "{");
		for(i=0;i<PAR2_COUNT;i++){
			if(i!=PAR2_COUNT-1){
				fprintf(stdout, "0x%02x, ", par2_div(gfi[j], gfi[i]));
			}
		}
		fprintf(stdout, "},\n");
	}
 */
