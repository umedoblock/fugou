/* utbi_isg.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_isg(unt g, unt *p)
{
	unt *p_2, *g_, *gg;
	int ret;
	extern int yousosuu;

	if(utbi_memory(&p_2, 3)==0){
		exit(1);
	}

	g_ = p_2 + yousosuu;
	gg = g_ + yousosuu;

	utbi_fukusha_ui(gg, g);

	utbi_bitzurashi_m(p_2, p);

	utbi_bekijouyo(g_, gg, p_2, p);

	ret = utbi_futougou_ui(g_, 1);

	free(p_2);

	return ret;
}
