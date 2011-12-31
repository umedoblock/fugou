/* utbi_sa_ui.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

#define __SA_UI_NEW__
#ifdef __SA_UI_NEW__
/*2005/02/09*/
void utbi_sa_ui(unt *z_sa, unt *x_sa, unt y_sa)
{
	extern int yousosuu;
	int i, c0;

	c0 = (*x_sa < y_sa);
	*z_sa = *x_sa - y_sa;
	x_sa++;
	z_sa++;

	for(i=1; i<yousosuu;i++){
		*z_sa = *x_sa - c0;
		c0 = c0 && !(~(*z_sa));

		x_sa++;
		z_sa++;
	}
}
#else/*__SA_UI_NEW__*/
void utbi_sa_ui(unt *z_sa_ui, unt *x_sa_ui, unt y_sa_ui)
{
	extern int yousosuu;
	unt *x_sa_ui_karimasu;
	unt *y_sa_ui_karimasu;

	x_sa_ui_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(x_sa_ui_karimasu == NULL){
		printf("メモリ確保ならず by差iroha\n");
		exit(1);
	}

	y_sa_ui_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(y_sa_ui_karimasu == NULL){
		printf("メモリ確保ならず by差nihohe\n");
		exit(1);
	}

	utbi_fukusha(x_sa_ui_karimasu, x_sa_ui);
	utbi_fukusha_ui(y_sa_ui_karimasu, y_sa_ui);

	utbi_sa(z_sa_ui, x_sa_ui_karimasu, y_sa_ui_karimasu);

	free(y_sa_ui_karimasu);
	free(x_sa_ui_karimasu);
}
#endif

