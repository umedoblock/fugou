/* utbi_sosuu_miller_rabin.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"
extern unsigned short sosuu[1000];

int utbi_sosuu_miller_rabin(unt *p, int i)
{
	unt *p_1, *p_1_adr;
	unt *a, *z, *m, *tmp;
	int j, b;
	int h, flg;
	extern int yousosuu;


	if(i>1000){
		printf("miller_rabib素数判定回数を1000回に縮めます。\n");
		i=1000;
	}

	if(utbi_memory(&tmp, 4)==0){
		exit(1);
	}
	p_1_adr = p_1 = tmp;
	a = p_1 + yousosuu;
	z = a + yousosuu;
	m = z + yousosuu;

	utbi_sa_ui(p_1, p, 1);

	b=0;
	for(;;){
		if((*(p_1)) == 0){
			b += 32;
			p_1++;
		}else{
			break;
		}
	}

	for(flg = 0x00000001;((*(p_1) & flg) == 0);flg<<=1){
		b++;
	}
	p_1 = p_1_adr;

	utbi_bitzurashi_m32_si(m, p_1, (b/32));
	utbi_bitzurashi_m_si(m, m, (b%32));

	for(h=0;h<i;h++){
/*No1*/	utbi_fukusha_ui(a, sosuu[h]);

/*No2*/	j=0;
		utbi_bekijouyo(z, a, m, p);

/*No3*/	if((utbi_futougou_ui(z, 1) == 0) || (utbi_futougou(z, p_1) == 0)){
			/*return 1;*/
			continue;
		}

		for(;;){
/*No4*/		if((j>0) && (utbi_futougou_ui(z, 1) == 0)){
				free(tmp);
				return 0;
			}
/*No5*/		j++;
			if(j<b){
				;
			}else{
				break;
			}
			if((utbi_futougou(z, p_1) != 0)){
				utbi_jouyo(z, z, z, p);
			}else{
				continue;
			}
		}
/*No6  j==bでここまで来る*/
		if((utbi_futougou(z, p_1) != 0)){
			free(tmp);
			return 0;
		}else{
			continue;/*return 1;*/
		}
	}

	free(tmp);
	return 1;


/********************************************************************
	utbi_fukusha_ui(a, sosuu[h]);
	j=0;
	utbi_bekijouyo(z, a, m, p);
	if((utbi_futougou_ui(z, 1) == 0) || (utbi_futougou(z, p_1) == 0)){
		return 1;
	}
	label4:
	if((j>0) && (utbi_futougou_ui(z, 1) == 0)){
		return 0;
	}
	j++;
	if((j<b) && (utbi_futougou(z, p_1) != 0)){
		utbi_jouyo(z, z, z, p);
		goto lebel4;
	}
	if((utbi_futougou(z, p_1) == 0)){
		return 1;
	}
	if((j==b) && (utbi_futougou(z, p_1) != 0)){
		return 0;
	}
*********************************************************************/

}
