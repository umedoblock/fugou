/* utbi_wa_ui.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

#define __WA_UI_NEW__
#ifdef __WA_UI_NEW__
/*2005/02/11*/
void utbi_wa_ui(unt *z_wa, unt *x_wa, unt y_wa)
{
	int i, c0;
	extern int yousosuu;
	int tmp = yousosuu;
//	static double wa_ui_time = 0.0;
//	clock_t s, e;

//	s = clock();

	*z_wa = *x_wa + y_wa;
	c0 = (*z_wa < y_wa);
	x_wa++;
	z_wa++;

	for(i=1; i<tmp;i++){
		*z_wa = *x_wa + c0;
		c0 = c0 && !(*z_wa);
		x_wa++;
		z_wa++;
	}
/*	e = clock();
	wa_ui_time += (double)(e-s)/CLOCKS_PER_SEC;
	printf("wa_ui_time = %lf\n", wa_ui_time);
*/
}
#endif/*__WA_UI_NEW__*/

#ifdef __WA_UI_OLD0__
void utbi_wa_ui(unt *z_wa, unt *x_wa, unt y_wa)
{
	int ketaagari = 0, i;
	unt *x_wa_kari, *x_wa_kari_adr;

	x_wa_kari = (unt *)calloc(yousosuu, sizeof(unt));
	if(x_wa_kari == NULL){
		printf("メモリ確保ならず by和_ui\n");
		return 0;
	}
	x_wa_kari_adr = x_wa_kari;

	utbi_fukusha(x_wa_kari, x_wa);
	utbi_shokika(z_wa);

	for(i=0; i<yousosuu; i++){

		*z_wa = *x_wa_kari + y_wa;

		if(*z_wa < *x_wa_kari && *z_wa < y_wa){
			/*桁上がりした*/
			*z_wa += ketaagari;
			ketaagari = 1;
		}else{
			*z_wa += ketaagari;
			if(*z_wa == 0 && ketaagari == 1){
				ketaagari = 1;
			}else{
				ketaagari = 0;
			}
		}

		z_wa++;
		x_wa_kari++;
		y_wa = 0;
	}

	x_wa_kari = x_wa_kari_adr;
	free(x_wa_kari);
}
#endif/*__WA_UI_OLD0__*/
