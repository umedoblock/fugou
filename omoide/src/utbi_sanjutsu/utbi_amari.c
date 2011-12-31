/* utbi_amari.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_amari(unt *y, unt *a, unt *n)
{
	int i, j=0, atama, atama0, atama1;
	extern int yousosuu;
	unt *aa, *nn, *mihari, *tmp;

	if(utbi_memory(&tmp, 3)==0){
		exit(1);
	}

	aa = tmp;
	nn = aa + yousosuu;
	mihari = nn + yousosuu;

	utbi_fukusha(aa, a);
	utbi_fukusha(nn, n);
	utbi_fukusha(mihari, nn);

	utbi_fukusha(y, aa);

	if(utbi_futougou(aa, nn) < 0){
		utbi_fukusha(y, aa);
		free(tmp);
		return;
	}

	aa += (yousosuu -1);
	for(i=yousosuu-1; (*aa == 0)&&(i>=0); i--){
		j--;
		aa--;
	}
	atama0 = utbi_atamadase(*aa);
	aa -= i;

	nn += (yousosuu -1);
	for(i=yousosuu-1; (*nn == 0)&&(i>=0); i--){
		j++;
		nn--;
	}
	atama1 = utbi_atamadase(*nn);
	nn -= i;

	atama = atama0 - atama1;

	if(atama > 0){
		;
	}else if(atama < 0){
		atama = 32 + atama;
		j--;
	}

	utbi_bitzurashi_h32_si(nn, nn, j);
	utbi_bitzurashi_h_si(nn, nn, atama);

	if(utbi_futougou(aa, nn) < 0){
		utbi_bitzurashi_m(nn, nn);
	}
	/*ここで最初に引く状態になった。*/
	while(utbi_futougou(y, mihari) >= 0){
		utbi_sa(y, y, nn);
		while(utbi_futougou(y, nn) < 0){
			utbi_bitzurashi_m_si(nn, nn, 1);
		}
	}

	free(tmp);

}
