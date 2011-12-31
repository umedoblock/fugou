/* utbi_sosuu_seisei.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_sosuu_seisei(unt *p, int bLen, unsigned int op)
{
	int i;
	int flg=1;

	if(op){
		for(;;){
			utbi_shokika(p);
			utbi_ransuu(p, bLen-1);
			*p |= 1;
			*(p+((bLen-1)/32)) |= (1 << (((bLen-1)%32)-1));
			for(i=0;i<1000&&flg==1;i++){
				if(utbi_sosuu_hantei(p, 10)){
					utbi_bitzurashi_h(p, p);
					utbi_wa_ui(p, p, 1);
					if(utbi_sosuu_hantei(p, 10)){
						return 1;
					}else{
						utbi_bitzurashi_m(p, p);
					}
				}
				utbi_wa_ui(p, p, 2);
			}
		}
	}else{
		for(;;){
			utbi_shokika(p);
			utbi_ransuu(p, bLen);
			*p |= 3;
			*(p+((bLen-1)/32)) |= (1 << ((bLen-1)%32));
			for(i=0;i<1000&&flg;i++){
				if(utbi_sosuu_hantei(p, 10)){
					return 1;
				}
				utbi_wa_ui(p, p, 4);
			}
		}
	}

}
