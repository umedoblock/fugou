/* utbi_jouyo.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"
#define __NEW_JOUYO__
/*#define __OLD_JOUYO0__*/

/*2005/02/19*/
#ifdef __NEW_JOUYO__
void utbi_jouyo(unt *mdr_jouyo, unt *iroha_jouyo, unt *nihohe_jouyo, unt *hou_jouyo)
{
	extern int yousosuu;
	unt *w_jouyo, *w_jouyo_adr;
	unt *www;
	unt *chuukansou, *chuukansou_adr;
	unt *iroha_jouyo_karimasu, *iroha_jouyo_karimasu_adr;
	unt *nihohe_jouyo_karimasu, *nihohe_jouyo_karimasu_adr;
	unt *mdr_jouyo_adr;
	unt *_n[15];
	unt *tmp = NULL;
	int foo;
	int i, j;
	int ketaagari;
	unt w;
	unsigned short *sahen, *uhen;

	if(utbi_memory(&tmp, 22)==0){
		exit(1);
	}

	_n[0] = tmp;
	iroha_jouyo_karimasu = _n[0] + 15*yousosuu;
	nihohe_jouyo_karimasu = iroha_jouyo_karimasu + 2*yousosuu;
	chuukansou = nihohe_jouyo_karimasu + 2*yousosuu;
	w_jouyo = chuukansou + 2*yousosuu;

	iroha_jouyo_karimasu_adr = iroha_jouyo_karimasu;
	nihohe_jouyo_karimasu_adr = nihohe_jouyo_karimasu;
	chuukansou_adr = chuukansou;
	w_jouyo_adr = w_jouyo;
	mdr_jouyo_adr = mdr_jouyo;

	utbi_shokika(chuukansou);
	utbi_shokika(chuukansou+yousosuu);
	utbi_shokika(w_jouyo);

	utbi_fukusha(iroha_jouyo_karimasu, iroha_jouyo);
	utbi_fukusha(nihohe_jouyo_karimasu, nihohe_jouyo);
	utbi_shokika(mdr_jouyo);

	sahen = (unsigned short*)iroha_jouyo_karimasu;
	uhen = (unsigned short*)nihohe_jouyo_karimasu;

/*	utbi_shokika(mdr_jouyo);*/

	/*-nを求める。*/
	utbi_hanten(_n[0], hou_jouyo);
	utbi_wa_ui(_n[0], _n[0], 1);
	utbi_amari(_n[0], _n[0], hou_jouyo);

/*  マイナス法剰余が全て0xffffffffだとメモリ領域破壊が起こる*/

	utbi_amari(_n[0], _n[0], hou_jouyo);

	for(i=0; (unt)i< 14; i++){
        _n[i+1] = _n[i] + yousosuu;
	}
	for(i=0;(unt)i<14;i++){
		utbi_wa(_n[i+1], _n[i], _n[0]);
		
		if(utbi_futougou(_n[i+1], _n[0]) < 0){
			do{
                utbi_wa(_n[i+1], _n[i+1], _n[0]);
			}while(utbi_futougou(_n[i+1], _n[0]) < 0);
		}
		if(utbi_futougou(_n[i+1], hou_jouyo) >= 0){
			utbi_sa(_n[i+1], _n[i+1], hou_jouyo);
		}
	}

/*	printf("---------右側---------------------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	ketaagari = 0;
	for(j=0;j<(2*yousosuu);j+=2){
		*mdr_jouyo += ketaagari;
		ketaagari = 0;
		for(i=0;(i-1)!=j;i++){
/*			printf("(%d,%d) ", i, j-i);*/
			w=(*(sahen+i))*(*(uhen+(j-i)));
			*mdr_jouyo += w;
			if(*mdr_jouyo < w){
				ketaagari++;
			}
		}
		mdr_jouyo++;
	}
	mdr_jouyo = mdr_jouyo_adr;

/*	printf("---------左側---------------------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	for(j=1;j<2*(yousosuu);j+=2){
		*w_jouyo += ketaagari;
		ketaagari = 0;
		for(i=j;i<2*yousosuu;i++){
/*			printf("(%d,%d) ", i, 2*yousosuu-i+j-1);*/
			w = *(sahen + i) * (*(uhen+(2*yousosuu-i+j-1)));
			*w_jouyo += w;
			if(*w_jouyo < w){
				ketaagari++;
			}
		}
		w_jouyo++;
	}
	w_jouyo = w_jouyo_adr;

/*	printf("--------中間層----右側----------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	ketaagari = 0;
	for(i=1;i<=2*yousosuu-1;i+=2){
		*chuukansou += ketaagari;
		ketaagari = 0;
		for(j=0;(unt)j<=(unt)i;j++){
/*			printf("(%d,%d),", j, i-j);*/
			w=(*(sahen+j))*(*(uhen+(i-j)));
			*chuukansou += w;
			if(*chuukansou < w){
				ketaagari++;
			}
		}
		chuukansou++;
	}

/*	printf("--------中間層----左側----------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	for(i=2;i<(2*yousosuu-1);i+=2){
		*chuukansou += ketaagari;
		ketaagari = 0;
		for(j=i;j<2*yousosuu;j++){
/*			printf("(%d,%d),", j, 2*yousosuu+i-j-1);*/
			w=(*(sahen+j))*(*(uhen+(2*yousosuu+i-j-1)));
			*chuukansou += w;
			if(*chuukansou < w){
				ketaagari++;
			}
		}
		chuukansou++;
	}
	chuukansou = chuukansou_adr;

	/*中間層の左側が桁上がりしたときに受ける必要がある。*/
	*(w_jouyo+(yousosuu-1)) += (0x00010000 * ketaagari);

	/*中間層は16ビットずれていて扱いにくいので*/
	/*左に16ビットシフトさせて戻値や左側のビットとあわせる*/
	/*これで扱いやすくなる*/
/*	utbi_bitzurashi_h_si((chuukansou+yousosuu), (chuukansou+yousosuu), 16);
	*(chuukansou + yousosuu) |= (*(chuukansou + (yousosuu-1)) >> 16);
	utbi_bitzurashi_h_si(chuukansou, chuukansou, 16);*/

	chuukansou += (2*yousosuu-1);
	for(i=0;i<2*yousosuu-1;i++){
		*chuukansou = (*chuukansou << 16) | (*(chuukansou-1) >> 16);
		chuukansou--;
	}
	*chuukansou <<= 16;
	chuukansou = chuukansou_adr;



	/*ずらした中間層をそれぞれ左側と右側に加える*/
	/*右側同士を加える*/
/*	utbi_amari(mdr_jouyo, mdr_jouyo, hou_jouyo);
	utbi_amari(chuukansou, chuukansou, hou_jouyo);
	ketaagari = 0;
	utbi_wa(mdr_jouyo, mdr_jouyo, chuukansou);
	if(utbi_futougou(mdr_jouyo, (chuukansou)) < 0){
		ketaagari++;
	}
	if(utbi_futougou(mdr_jouyo, hou_jouyo) >= 0){
		utbi_sa(mdr_jouyo, mdr_jouyo, hou_jouyo);
	}*/
	if(utbi_futougou(mdr_jouyo, hou_jouyo) >= 0){
		utbi_sa(mdr_jouyo, mdr_jouyo, hou_jouyo);
	}
	if(utbi_futougou(chuukansou, hou_jouyo) >= 0){
		utbi_sa(chuukansou, chuukansou, hou_jouyo);
	}
	ketaagari = 0;
	utbi_wa(mdr_jouyo, mdr_jouyo, chuukansou);
	if(utbi_futougou(mdr_jouyo, (chuukansou)) < 0){
		ketaagari++;
	}
	if(utbi_futougou(mdr_jouyo, hou_jouyo) >= 0){
		utbi_sa(mdr_jouyo, mdr_jouyo, hou_jouyo);
	}


	/*今度は左側同士を加える*/
	utbi_amari(w_jouyo, w_jouyo, hou_jouyo);
	utbi_amari((chuukansou+yousosuu), (chuukansou+yousosuu), hou_jouyo);
	utbi_wa_ui(w_jouyo, w_jouyo, (unt)ketaagari);
	utbi_wa(w_jouyo, w_jouyo, (chuukansou+yousosuu));
/*	if(utbi_futougou(w_jouyo, (chuukansou+yousosuu)) < 0){
		if(utbi_futougou(w_jouyo, hou_jouyo) >= 0){
			utbi_sa(w_jouyo, w_jouyo, hou_jouyo);
		}
		utbi_wa(w_jouyo, w_jouyo, _n[0]);
	}*/
	/*↑について、桁上がりするわけないので↓の処理のみ*/
	if(utbi_futougou(w_jouyo, hou_jouyo) >= 0){
		utbi_sa(w_jouyo, w_jouyo, hou_jouyo);
	}

	/*左側は右側に比べて32*yousosuuビットほど左にずれなくてはいけない*/
	/*2乗剰余処理を32*yousosuu回ほど行う必要がある*/
	/*2005/02/19*/
	/*2乗剰余処理を4ビット単位で行うことで高速化を図る*/
	www = w_jouyo + yousosuu - 1;
	for(i=0; i<(32/4)*yousosuu; i++){
		foo = *(www) >> 28;
		if(foo){
			utbi_bitzurashi_h4(w_jouyo, w_jouyo);
			utbi_wa(w_jouyo, w_jouyo, _n[foo-1]);
			if(utbi_futougou(w_jouyo, _n[foo-1]) < 0){
				utbi_wa(w_jouyo, w_jouyo, _n[0]);
			}
		}else{
			utbi_bitzurashi_h4(w_jouyo, w_jouyo);
		}
		/*if(utbi_futougou(w_jouyo, hou_jouyo) >= 0){
			utbi_sa(w_jouyo, w_jouyo, hou_jouyo);
		}*/
	}
	/*これで右側と左側のビットの位置が一致した*/

	/*最終的な処理*/
	/*左側と右側の和をとってやる*/

	utbi_wa(mdr_jouyo, mdr_jouyo, w_jouyo);
	if(utbi_futougou(mdr_jouyo, w_jouyo) < 0){
		utbi_wa(mdr_jouyo, mdr_jouyo, _n[0]);
	}
	utbi_amari(mdr_jouyo, mdr_jouyo, hou_jouyo);
/*	if(utbi_futougou(mdr_jouyo, hou_jouyo) >= 0){
		utbi_sa(mdr_jouyo, mdr_jouyo, hou_jouyo);
	}*/
	/*これで乗剰余処理終了*/
	free(tmp);
}
#endif/*__NEW_JOUYO__*/

void utbi_jouyo_ui(unt *z_jouyo, unt *x_jouyo, unt y_jouyo, unt *n_jouyo)
{
	extern int yousosuu;
	unt *yyy;

	yyy = (unt *)malloc(yousosuu * sizeof(unt));
	if(yyy == NULL){
		printf("aho- jouyo_ui\n");
		exit(1);
	}

	utbi_fukusha_ui(yyy, y_jouyo);

	utbi_jouyo(z_jouyo, x_jouyo, yyy, n_jouyo);

	free(yyy);
}

