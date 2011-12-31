/* utbi_narabekae.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_narabekae(unt *qwp, int kaisuu, int op)
{

	extern int yousosuu;
	int i, j;

	if(op>0){
        for(i=kaisuu;i>0;i--){
			for(j=1;j<i;j++){
				if(utbi_futougou((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu))) > 0){
					utbi_irekae((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu)));
				}
			}
		}
	}else if(op<0){
        for(i=kaisuu;i>0;i--){
			for(j=1;j<i;j++){
				if(utbi_futougou((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu))) < 0){
					utbi_irekae((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu)));
				}
			}
		}
	}else{
		;
	}

}



void utbi_narabekae2(unt *qwp, unt *juuzoku, int kaisuu, int op)
{

	extern int yousosuu;
	int i, j;

	if(op>0){
        for(i=kaisuu;i>0;i--){
			for(j=1;j<i;j++){
				if(utbi_futougou((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu))) > 0){
					utbi_irekae((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu)));
					utbi_irekae((juuzoku+((j-1)*yousosuu)), (juuzoku+(j*yousosuu)));

				}
			}
		}
	}else if(op<0){
        for(i=kaisuu;i>0;i--){
			for(j=1;j<i;j++){
				if(utbi_futougou((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu))) < 0){
					utbi_irekae((qwp+((j-1)*yousosuu)), (qwp+(j*yousosuu)));
					utbi_irekae((juuzoku+((j-1)*yousosuu)), (juuzoku+(j*yousosuu)));
				}
			}
		}
	}else{
		;
	}

}
