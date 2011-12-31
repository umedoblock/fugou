/* utbi_ransuu.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>
#include "utbi_sanjutsu.h"

void utbi_ransuu_seikika(unt *ransuu, int bLen);

void utbi_ransuu(unt *ransuu, int bLen)
{
	//nには乱数を作成するべき要素数が入る。
	int i, n = (bLen-1)/(8*(int)sizeof(unt)) + 1;
	struct timeb tb;

	ftime(&tb);

	utbi_shokika(ransuu);
	srand(time(&tb.time)+tb.millitm);
	for(i=0;i<n;i++){
		ransuu[i] = (rand() << 16) + rand();
	}

	//32bit単位でbit数を指定されない場合も考えら
	//れるので、正規化を行う。
	utbi_ransuu_seikika(ransuu, bLen);
}

unt utbi_ransuu32(void)
{
	int i, k, t;
	clock_t start[2], end[2];
	long timer[2];
	unt r;

	r = 0;

	for(i=0;i<32;){
		for(t=0; t<2; t++){
			start[t] = clock();
			for(k=0; k<1000000; k++){
				;
			}
			end[t] = clock();
			timer[t] = end[t] - start[t];
		}

		if(timer[0] != timer[1]){
			r <<= 1;
			r |= (timer[0] < timer[1]);
			i++;
		}

	}

	return r;	
}

void utbi_ransuu_seikika(unt *ransuu, int bLen)
{
	*(ransuu + ((bLen-1)/32)) &= (~(unt)0) >> (32-(bLen%32) % 32);
}
