/* utbi_fputx.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "utbi_sanjutsu.h"

void utbi_fputX(FILE *fp, unt *x_p)
{
	int flg, i;
	extern int yousosuu;

	x_p = x_p + (yousosuu - 1);

	flg=0;
	for(i=(yousosuu - 1); i>=0; i--){
		if(flg == 0){
			if(*x_p==0){
				flg = 0;
			}else{
				flg = 1;
			}
		}

		if(flg==2){
			fprintf(fp, "%08X", *x_p);
		}

		if(flg==1){
			fprintf(fp, "%X", *x_p);
			flg = 2;
		}
		x_p--;
	}
	if(flg==0){
		fprintf(fp, "0");
	}
}

void utbi_fputx(FILE *fp, unt *x_p)
{
	int flg, i;
	extern int yousosuu;

	x_p = x_p + (yousosuu - 1);

	flg=0;
	for(i=(yousosuu - 1); i>=0; i--){
		if(flg == 0){
			if(*x_p==0){
				flg = 0;
			}else{
				flg = 1;
			}
		}

		if(flg==2){
			fprintf(fp, "%08x", *x_p);
		}

		if(flg==1){
			fprintf(fp, "%x", *x_p);
			flg = 2;
		}
		x_p--;
	}
	if(flg==0){
		fprintf(fp, "0");
	}
}


void utbi_fput0X(FILE *fp, unt *x_p)
{
	int i;
	extern int yousosuu;

	x_p = x_p + (yousosuu - 1);

	for(i=(yousosuu - 1); i>=0; i--){
		fprintf(fp, "%08X", *x_p);
		x_p--;
	}
}

void utbi_fput0x(FILE *fp, unt *x_p)
{
	int i;
	extern int yousosuu;

	x_p = x_p + (yousosuu - 1);

	for(i=(yousosuu - 1); i>=0; i--){
		fprintf(fp, "%08x", *x_p);
		x_p--;
	}
}
