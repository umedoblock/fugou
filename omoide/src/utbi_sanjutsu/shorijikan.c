/* shorijikan.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utbi_sanjutsu.h"

void shorijikan(int i, char ss[], clock_t sstart)
{
	FILE *ffout;
	clock_t end;
	double time;
	ffout = fopen(ss, "a+");
	if(ffout == NULL){
		puts("処理時間でファイル開けず。");
		exit(1);
	}

	end = clock();
	time = (double)(end - sstart) / CLOCKS_PER_SEC;

	switch(i){
/*		case 100:
		case 200:
		case 400:
		case 800:
		case 1000:*/
		case 2000:
		case 5000:
			printf("現在%d回前処理を行いました。\n", i);
			printf("ただいまの経過時間 :%6f秒\n", time);
			fprintf(ffout, "現在%d回前処理を行いました。\n", i);
			fprintf(ffout, "ただいまの経過時間 :%6f秒\n", time);
			break;
		default :
			break;
	}
	if((i%100) == 0){
		printf("現在%d回前処理を行いました。\n", i);
		printf("ただいまの経過時間 :%6f秒\n", time);
		fprintf(ffout, "現在%d回前処理を行いました。\n", i);
		fprintf(ffout, "ただいまの経過時間 :%6f秒\n", time);
	}
	fclose(ffout);
}
