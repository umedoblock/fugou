/* utbi_putx.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "utbi_sanjutsu.h"

void utbi_putX(unt *x_p)
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
			printf("%08X", *x_p);
		}

		if(flg==1){
			printf("%X", *x_p);
			flg = 2;
		}
		x_p--;
	}
	if(flg==0){
		printf("0");
	}
}

void utbi_putx(unt *x_p)
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
			printf("%08x", *x_p);
		}

		if(flg==1){
			printf("%x", *x_p);
			flg = 2;
		}
		x_p--;
	}
	if(flg==0){
		printf("0");
	}
}
