/* utbi_sosuu_lehmann.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

extern unsigned short sosuu[1000];
int utbi_sosuu_lehmann(unt *hanteisurusosuu, int i)
{
	int j;
	unt *a_lehmann;
	unt *x_lehmann;
	unt *bekibeki_lehmann;
	unt *sosuu_1;
	extern int yousosuu;

	if(i>1000){
		printf("Lehmann素数判定回数を1000回に変更します。\n");
		i=1000;
	}

	a_lehmann = (unt *)calloc(yousosuu, sizeof(unt));
	if(a_lehmann == NULL){
		printf("lehmann判定でメモリ借りれませんでした。 a\n");
		exit(1);
	}

	x_lehmann = (unt *)calloc(yousosuu, sizeof(unt));
	if(x_lehmann == NULL){
		printf("lehmann判定でメモリ借りれませんでした。z\n");
		exit(1);
	}

	sosuu_1 = (unt *)calloc(yousosuu, sizeof(unt));
	if(sosuu_1 == NULL){
		printf("lehmann判定でメモリ借りれませんでした。sosuu_1\n");
		exit(1);
	}

	bekibeki_lehmann = (unt *)calloc(yousosuu, sizeof(unt));
	if(bekibeki_lehmann == NULL){
		printf("lehmann判定でメモリ借りれませんでした。 bekibeki\n");
		exit(1);
	}

	/*(p-1)/2*/
	utbi_sa_ui(sosuu_1, hanteisurusosuu, 1);
	utbi_bitzurashi_m(bekibeki_lehmann, sosuu_1);

	for(j = 0 ; j < i; j++){
		utbi_fukusha_ui(a_lehmann, sosuu[j+1]);
		utbi_bekijouyo(x_lehmann, a_lehmann, bekibeki_lehmann, hanteisurusosuu);
		if((utbi_futougou_ui(x_lehmann, 1) == 0) || (utbi_futougou(x_lehmann, sosuu_1) == 0)){
			continue;
		}else{
			free(a_lehmann);
			free(x_lehmann);
			free(sosuu_1);
			free(bekibeki_lehmann);
			exit(1);
		}
	}

	free(a_lehmann);
	free(x_lehmann);
	free(sosuu_1);
	free(bekibeki_lehmann);

	return 1;
}
