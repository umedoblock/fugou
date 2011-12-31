/* utbi_bekijouyo.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

#define __NEW_BEKIJOUYO__

/*2005/02/20*/
#ifdef __NEW_BEKIJOUYO__
void utbi_bekijouyo(unt *mdr_bekijouyo, unt *kisuu_bekijouyo, unt *bekisuu_bekijouyo, unt *houtosurukazu_bekijouyo)
{
	int i;
	unt *work[15];
	int *foo;
	unt *a, *x, *n, *tmp;
	extern int yousosuu;

	if(utbi_futougou_ui(houtosurukazu_bekijouyo, 0) == 0){
		printf("法とする数が0ですのでエラ〜\n");
		printf("a ^ x mod nとする時\n");
		printf("a=");utbi_putx(kisuu_bekijouyo);
		printf("x=");utbi_putx(bekisuu_bekijouyo);
		printf("n=0\n");
		printf("n=");utbi_putx(houtosurukazu_bekijouyo);
		exit(1);
	}
	if(utbi_memory(&tmp, 15)==0){
		exit(1);
	}

	work[0] = tmp;

	foo = (int *)malloc(sizeof(int) * yousosuu * 8);
	if(foo == NULL){
		fprintf(stderr, "aho- bekijouyo by foo\n");
		exit(1);
	}

	/*utbi_fukusha(a, kisuu_bekijouyo);*/
	a = kisuu_bekijouyo;
	/*utbi_fukusha(x, bekisuu_bekijouyo);*/
	x = bekisuu_bekijouyo;
	/*utbi_fukusha(n, houtosurukazu_bekijouyo);*/
	n = houtosurukazu_bekijouyo;

	for(i=0;i < yousosuu; i++){
		*(foo+8*i+0) = (*(x + yousosuu - 1 - i) >> 28) & 0xf;
		*(foo+8*i+1) = (*(x + yousosuu - 1 - i) >> 24) & 0xf;
		*(foo+8*i+2) = (*(x + yousosuu - 1 - i) >> 20) & 0xf;
		*(foo+8*i+3) = (*(x + yousosuu - 1 - i) >> 16) & 0xf;
		*(foo+8*i+4) = (*(x + yousosuu - 1 - i) >> 12) & 0xf;
		*(foo+8*i+5) = (*(x + yousosuu - 1 - i) >> 8) & 0xf;
		*(foo+8*i+6) = (*(x + yousosuu - 1 - i) >> 4) & 0xf;
		*(foo+8*i+7) = (*(x + yousosuu - 1 - i) >> 0) & 0xf;
	}


	for(i=0; i< 14; i++){
        work[i+1] = work[i] + yousosuu;
	}
	utbi_fukusha(work[0], a);
	for(i=0;i<14;i++){
		utbi_jouyo(work[i+1], work[i], work[0], n);
	}

	if(*(foo+0)){
		utbi_fukusha(mdr_bekijouyo, work[(*(foo+0)-1)]);
	}else{
		utbi_fukusha_ui(mdr_bekijouyo, 1);
	}

	for(i=1; i< 8*yousosuu; i++){
		/*戻り値のbekijouyoをとりあえず16乗する。*/
		utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, mdr_bekijouyo, n);
		utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, mdr_bekijouyo, n);
		utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, mdr_bekijouyo, n);
		utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, mdr_bekijouyo, n);

		if(*(foo+i)){
			/*もし、4連ビットの指数が0でなければ。*/
			utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, work[(*(foo+i)-1)], n);
		}
	}

	free(tmp);
	free(foo);

}

#endif/*__NEW_BEKIJOUYO__*/

#ifdef __OLD_BEKIJOUYO0__
void utbi_bekijouyo(unt *mdr_bekijouyo, unt *kisuu_bekijouyo, unt *bekisuu_bekijouyo, unt *houtosurukazu_bekijouyo)
{
	int i;
	unt flg;

	utbi_tmp_adr(bekijouyo_a);
	bekijouyo_x.tmp = bekijouyo_x.adr;
	bekijouyo_n.tmp = bekijouyo_n.adr;

	if(utbi_futougou_ui(houtosurukazu_bekijouyo, 0) == 0){
		printf("法とする数が0ですのでエラ〜\n");
		printf("a ^ x mod nとする時\n");
		printf("a=");utbi_putx(kisuu_bekijouyo);
		printf("x=");utbi_putx(bekisuu_bekijouyo);
		printf("n=0\n");
		printf("n=");utbi_putx(houtosurukazu_bekijouyo);
		exit(1);
	}

	for(i=0;(unt)i<yousosuu;i++){
		*bekijouyo_a.tmp = *kisuu_bekijouyo;
		*bekijouyo_x.tmp = *bekisuu_bekijouyo;
		*bekijouyo_n.tmp = *houtosurukazu_bekijouyo;
		*mdr_bekijouyo = 0;

		bekijouyo_a.tmp++;kisuu_bekijouyo++;
		bekijouyo_x.tmp++;bekisuu_bekijouyo++;
		bekijouyo_n.tmp++;houtosurukazu_bekijouyo++;
		mdr_bekijouyo++;
	}
	bekijouyo_a.tmp -= (yousosuu);
	kisuu_bekijouyo -= (yousosuu);
	bekisuu_bekijouyo -= (yousosuu);
	bekijouyo_n.tmp -= (yousosuu);
	houtosurukazu_bekijouyo -= (yousosuu);
	mdr_bekijouyo -= (yousosuu);
	*mdr_bekijouyo = 0x00000001;
	/*utbi_fukusha_ui(mdr_bekijouyo, 1);*/

	/*utbi_fukusha(bekijouyo_a.tmp, kisuu_bekijouyo);
	utbi_fukusha(bekijouyo_x.tmp, bekisuu_bekijouyo);
	utbi_fukusha(bekijouyo_n.tmp, houtosurukazu_bekijouyo);
	utbi_fukusha_ui(mdr_bekijouyo, 1);*/

/*	bekijouyo_x.tmp -= (yousosuu-1);*/
/*	bekijouyo_x.tmp += (yousosuu - 1);*/
/*  bekijouyo_x.tmpはforを回る最中に頭まで来ている。*/
	bekijouyo_x.tmp--;
	for(i=(yousosuu-1); i>=0; i--){
		if(*bekijouyo_x.tmp){
			break;
		}
		bekijouyo_x.tmp--;
	}
	flg = 0x00000001 << utbi_atamadase(*bekijouyo_x.tmp);

	while(i>=0){
		for( ; flg; flg >>= 1){
			/*戻り値のbekijouyoをとりあえず2乗する。*/
			utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, mdr_bekijouyo, bekijouyo_n.tmp);

			if(flg & *bekijouyo_x.tmp){
				/*もし、ビットが1であれば基数をかけてやる。*/
				utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, bekijouyo_a.tmp, bekijouyo_n.tmp);
			}
		}
		flg = 0x80000000;
		bekijouyo_x.tmp--;
		i--;
	}

	bekijouyo_a.tmp = bekijouyo_a.adr;
	bekijouyo_x.tmp = bekijouyo_x.adr;
	bekijouyo_n.tmp = bekijouyo_n.adr;

}
#endif/*__OLD_BEKIJOUYO0__*/

/*2005/02/13*/
void utbi_bekijouyo_ui(unt *mdr_bekijouyo, unt *kisuu_bekijouyo, unt shisuu_bekijouyo, unt *houtosurukazu_bekijouyo)
{
	unt flg;
	unt *a, *n, *tmp;
	extern int yousosuu;


	if(utbi_futougou_ui(houtosurukazu_bekijouyo, 0) == 0){
		printf("法とする数が0ですのでエラ〜\n");
		printf("a ^ x mod nとする時\n");
		printf("a=");utbi_putx(kisuu_bekijouyo);
		printf("x=%u", shisuu_bekijouyo);
		printf("n=0\n");
		printf("n=");utbi_putx(houtosurukazu_bekijouyo);
		exit(1);
	}

	if(utbi_memory(&tmp, 2)==0){
		exit(1);
	}

	a = tmp;
	n = tmp + yousosuu;

	utbi_fukusha(a, kisuu_bekijouyo);
	utbi_fukusha(n, houtosurukazu_bekijouyo);
	utbi_fukusha_ui(mdr_bekijouyo, 1);

	for(flg = 0x80000000; flg; flg >>= 1){
		/*戻り値のbekijouyoをとりあえず2乗する。*/
		utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, mdr_bekijouyo, n);

		if(flg & shisuu_bekijouyo){
			/*もし、ビットが1であれば基数をかけてやる。*/
			utbi_jouyo(mdr_bekijouyo, mdr_bekijouyo, a, n);
		}
	}

	free(tmp);
}
