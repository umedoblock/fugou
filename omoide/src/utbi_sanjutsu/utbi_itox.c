/* utbi_itox.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_itox(wchar_t *wss, unt *x)
{
	int i, j;
	wchar_t tmp;
	unt *work;

	if(!utbi_memory(&work, 1)){
		exit(1);
	}
	utbi_fukusha(work, x);

	i=0;
	while(utbi_futougou_ui(work, 0) > 0){
		wss[i] = (wchar_t)(*work&0x000f);
		utbi_bitzurashi_m_si(work, work, 4);
		i++;
	}
	if(i==0){
		wss[0] = 0;
		i++;
	}

	for(j=0;j<i;j++){
		wss[j] += ((wss[j]<10) ? L'0' : (L'a'-10));
	}
	wss[i] = L'\0';

	for(j=0;j<i/2;j++){
		tmp = wss[j];
		wss[j] = wss[i-j-1];
		wss[i-j-1] = tmp;
	}
	utbi_shokika(work);

	free(work);
}
