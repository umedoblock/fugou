/* utbi_seki.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_seki(unt *mdr_seki, unt *iroha_seki, unt *nihohe_seki)
{
	unt *w_seki = NULL, *w_seki_adr;
	extern int yousosuu;
	unt *chuukansou = NULL, *chuukansou_adr;
	unt *mdr_seki_adr;
	unt *iroha_seki_karimasu, *iroha_seki_karimasu_adr;
	unt *nihohe_seki_karimasu, *nihohe_seki_karimasu_adr;
	int i, j;
	unt w;
	unsigned short *sahen, *uhen;
	int ketaagari;

	iroha_seki_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(iroha_seki_karimasu == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	iroha_seki_karimasu_adr = iroha_seki_karimasu;

	nihohe_seki_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(nihohe_seki_karimasu == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	nihohe_seki_karimasu_adr = nihohe_seki_karimasu;

	chuukansou = (unt *)calloc(2*yousosuu, sizeof(unt));
	if(chuukansou == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	chuukansou_adr = chuukansou;

	w_seki = (unt *)calloc(yousosuu, sizeof(unt));
	if(w_seki == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	w_seki_adr = w_seki;

	utbi_fukusha(iroha_seki_karimasu, iroha_seki);
	utbi_fukusha(nihohe_seki_karimasu, nihohe_seki);
	sahen = (unsigned short*)iroha_seki_karimasu;
	uhen = (unsigned short*)nihohe_seki_karimasu;

	utbi_shokika(mdr_seki);
	utbi_shokika(chuukansou);
	utbi_shokika(chuukansou+yousosuu);
	utbi_shokika(w_seki);

	mdr_seki_adr = mdr_seki;

/*	printf("---------右側---------------------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	ketaagari = 0;
	for(j=0;j<(2*yousosuu);j+=2){
		*mdr_seki += ketaagari;
		ketaagari = 0;
		for(i=0;(i-1)!=j;i++){
/*			printf("(%d,%d) ", i, j-i);*/
			w=(*(sahen+i))*(*(uhen+(j-i)));
			*mdr_seki += w;
			if(*mdr_seki < w){
				ketaagari++;
			}
		}
		mdr_seki++;
	}
	mdr_seki = mdr_seki_adr;

/*	printf("---------左側---------------------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	for(j=1;j<2*(yousosuu);j+=2){
		*w_seki += ketaagari;
		ketaagari = 0;
		for(i=j;i<2*yousosuu;i++){
/*			printf("(%d,%d) ", i, 2*yousosuu-i+j-1);*/
			w = *(sahen + i) * (*(uhen+(2*yousosuu-i+j-1)));
			*w_seki += w;
			if(*w_seki < w){
				ketaagari++;
			}
		}
		w_seki++;
	}
	w_seki = w_seki_adr;

/*	printf("--------中間層----右側側----------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	ketaagari = 0;
	for(i=1;i<=2*yousosuu-1;i+=2){
		*chuukansou += ketaagari;
		ketaagari = 0;
		for(j=0;j<=i;j++){
/*			printf("(%d,%d),", j, i-j);*/
			w=(*(sahen+j))*(*(uhen+(i-j)));
			*chuukansou += w;
			if(*chuukansou < w){
				ketaagari++;
			}
		}
		chuukansou++;
	}

/*	printf("--------中間層----左側側----------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	for(i=2;i<(2*yousosuu-1);i+=2){
		*chuukansou += ketaagari;
		ketaagari = 0;
		for(j=i;j<2*yousosuu;j++){
			/*printf("(%d,%d),", j, 2*yousosuu+i-j-1);*/
			w=(*(sahen+j))*(*(uhen+(2*yousosuu+i-j-1)));
			*chuukansou += w;
			if(*chuukansou < w){
				ketaagari++;
			}
		}
		chuukansou++;
	}
	chuukansou = chuukansou_adr;

	/*中間層の左側側が桁上がりしたときに受ける必要がある。*/
	*(w_seki+(yousosuu-1)) += (0x00010000 * ketaagari);

	/*中間層は16ビットずれていて扱いにくいので*/
	/*左に16ビットシフトさせて戻値や左側側のビットとあわせる*/
	/*これで扱いやすくなる*/
	utbi_bitzurashi_h_si((chuukansou+yousosuu), (chuukansou+yousosuu), 16);

	*(chuukansou + yousosuu) |= (*(chuukansou + (yousosuu-1)) >> 16);
	utbi_bitzurashi_h_si(chuukansou, chuukansou, 16);

	/*ずらした中間層をそれぞれ左側と右側に加える*/
	/*右側同士を加える*/
	ketaagari = 0;
	utbi_wa(mdr_seki, mdr_seki, chuukansou);
	if(utbi_futougou(mdr_seki, (chuukansou)) < 0){
		ketaagari++;
	}

	/*今度は左側同士を加える*/
	utbi_wa_ui(w_seki, w_seki, (unt)ketaagari);
	utbi_wa(w_seki, w_seki, (chuukansou+yousosuu));

	/*計算結果を左側に戻してやる*/
	utbi_fukusha(mdr_seki+yousosuu, w_seki);

	nihohe_seki_karimasu = nihohe_seki_karimasu_adr;
	iroha_seki_karimasu = iroha_seki_karimasu_adr;
	chuukansou = chuukansou_adr;
	w_seki = w_seki_adr;

	free(chuukansou);
	free(w_seki);
	free(iroha_seki_karimasu);
	free(nihohe_seki_karimasu);
}

void utbi_seki_ui(unt *mdr_seki, unt *iroha_seki, unt nihohe_seki)
{
/*	unt *w_seki = NULL, *w_seki_adr;*/
	unt *chuukansou = NULL, *chuukansou_adr;
	unt *mdr_seki_adr;
	unt *iroha_seki_karimasu, *iroha_seki_karimasu_adr;
	unt *nihohe_seki_karimasu, *nihohe_seki_karimasu_adr;
	int i, j, k=0;
	unt w;
	unsigned short *sahen, *uhen;
	int ketaagari;
	extern int yousosuu;

	iroha_seki_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(iroha_seki_karimasu == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	iroha_seki_karimasu_adr = iroha_seki_karimasu;

	nihohe_seki_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(nihohe_seki_karimasu == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	nihohe_seki_karimasu_adr = nihohe_seki_karimasu;

	chuukansou = (unt *)calloc(yousosuu, sizeof(unt));
	if(chuukansou == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	chuukansou_adr = chuukansou;

/*	w_seki = (unt *)calloc(yousosuu, sizeof(unt));
	if(w_seki == NULL){
		printf("メモリ貸してくれません。\n");
		exit(1);
	}
	w_seki_adr = w_seki;*/

	utbi_fukusha(iroha_seki_karimasu, iroha_seki);
	utbi_fukusha_ui(nihohe_seki_karimasu, nihohe_seki);
	sahen = (unsigned short*)iroha_seki_karimasu;
	uhen = (unsigned short*)nihohe_seki_karimasu;

	utbi_shokika(mdr_seki);
	utbi_shokika(chuukansou);
/*	utbi_shokika(chuukansou+yousosuu);
	utbi_shokika(w_seki);*/

	mdr_seki_adr = mdr_seki;

/*	printf("---------右側---------------------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	ketaagari = 0;
	for(j=0;j<(2*yousosuu);j+=2){
		*mdr_seki += ketaagari;
		ketaagari = 0;
		for(i=0;(i-1)!=j;i++){
/*			printf("(%d,%d) ", i, j-i);*/
			w=(*(sahen+i))*(*(uhen+(j-i)));
			*mdr_seki += w;
			if(*mdr_seki < w){
				ketaagari++;
			}
		}
		mdr_seki++;

	}
	mdr_seki = mdr_seki_adr;

/*	printf("---------左側---------------------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
/*	for(j=1;j<2*(yousosuu);j+=2){
		*w_seki += ketaagari;
		ketaagari = 0;
		for(i=j;i<2*yousosuu;i++){
			printf("(%d,%d) ", i, 2*yousosuu-i+j-1);
			w = *(sahen + i) * (*(uhen+(2*yousosuu-i+j-1)));
			*w_seki += w;
			if(*w_seki < w){
				ketaagari++;
			}
		}
		w_seki++;
	}
	w_seki = w_seki_adr;*/

/*	printf("--------中間層----右側側----------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
	ketaagari = 0;
	for(i=1;i<=2*yousosuu-1;i+=2){
		*chuukansou += ketaagari;
		ketaagari = 0;
		for(j=0;j<=i;j++){
/*			printf("(%d,%d),", j, i-j);*/
			w=(*(sahen+j))*(*(uhen+(i-j)));
			*chuukansou += w;
			if(*chuukansou < w){
				ketaagari++;
			}
		}
		chuukansou++;
		k++;
	}
	chuukansou = chuukansou_adr;

/*	printf("--------中間層----左側側----------------------------\n");*/
/*	printf("----------------------------------------------------\n");*/
/*	for(i=2;i<(2*yousosuu-1);i+=2){
		*chuukansou += ketaagari;
		ketaagari = 0;
		for(j=i;j<2*yousosuu;j++){
			printf("(%d,%d),", j, 2*yousosuu+i-j-1);
			w=(*(sahen+j))*(*(uhen+(2*yousosuu+i-j-1)));
			*chuukansou += w;
			if(*chuukansou < w){
				ketaagari++;
			}
		}
		chuukansou++;
	}*/

	/*中間層の左側側が桁上がりしたときに受ける必要がある。*/
/*	*(w_seki+(yousosuu-1)) += (0x00010000 * ketaagari);*/

	/*中間層は16ビットずれていて扱いにくいので*/
	/*左に16ビットシフトさせて戻値や左側側のビットとあわせる*/
	/*これで扱いやすくなる*/
/*	utbi_bitzurashi_h_si((chuukansou+yousosuu), (chuukansou+yousosuu), 16);*/

/*	*(chuukansou + yousosuu) |= (*(chuukansou + (yousosuu-1)) >> 16);*/
	utbi_bitzurashi_h_si(chuukansou, chuukansou, 16);

	/*ずらした中間層をそれぞれ左側と右側に加える*/
	/*右側同士を加える*/
	ketaagari = 0;
	utbi_wa(mdr_seki, mdr_seki, chuukansou);
/*	if(utbi_futougou(mdr_seki, (chuukansou)) < 0){
		ketaagari++;
	}*/

	/*今度は左側同士を加える*/
/*	utbi_wa_ui(w_seki, w_seki, (unt)ketaagari);
	utbi_wa(w_seki, w_seki, (chuukansou+yousosuu));*/


	nihohe_seki_karimasu = nihohe_seki_karimasu_adr;
	iroha_seki_karimasu = iroha_seki_karimasu_adr;
	chuukansou = chuukansou_adr;
/*	w_seki = w_seki_adr;*/

	free(chuukansou);
/*	free(w_seki);*/
	free(iroha_seki_karimasu);
	free(nihohe_seki_karimasu);
}
