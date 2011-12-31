/* mazekoze.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"
void mazekoze_bekijouyo(void);
void mazekoze_jouyo(unt *y, unt *a, unt *x);
#define BITNUM (8)
#define _N_NUM ((1<<BITNUM)-1)
struct{
	unt *a;
	unt *b;
	unt *x;
	unt *y;
	unt *p;
	unt *p_adr;
	unt *_p[_N_NUM];
	unt *a1;
	unt *b1;
	unt *c1;
	unt *c1_adr;
	int maisuu;
	unt *tmp;
	unt *adr;
	int memsize;
	unt *shisuu;
	unt *work;
}cut;

void mazekoze(unt *yama, unt *fuda, unt *ransuu, unt *n, int maisuu, int op)
{
	unt i, j;
	unt *tmp;
	unt *cut_a_adr;
	extern int yousosuu;

	cut.y = yama;
	cut.a = fuda;
	cut_a_adr = cut.a;
	cut.x = ransuu;
	cut.p = n;
	cut.maisuu = maisuu;

	if((utbi_futougou_ui(cut.p, 0))==0){
		fprintf(stderr, "aho- 0を法にできるかよ。\n");
		fprintf(stderr, "%s\n", __FILE__);
		exit(1);
	}

	cut.memsize = _N_NUM+(15*cut.maisuu)+1+8;
	if(utbi_memory(&cut.tmp, cut.memsize)){
		;
	}else{
		fprintf(stderr, "aho- cut.tmp\n");
		fprintf(stderr, "%s\n", __FILE__);
		exit(1);
	}
	cut.adr = cut.tmp;

	cut._p[0] = cut.tmp;
	cut.tmp += _N_NUM*yousosuu;

	cut.work = cut.tmp;
	cut.tmp += (15*yousosuu)*cut.maisuu;

	cut.c1 = cut.tmp;
	cut.c1_adr = cut.c1;
	cut.tmp += 1*yousosuu;

	cut.shisuu = cut.tmp;
	cut.tmp += 8*yousosuu;

	cut.tmp = cut.adr;

	for(i=0; i<_N_NUM-1; i++){
        cut._p[i+1] = cut._p[i] + yousosuu;
	}

	utbi_hanten(cut._p[0], cut.p);
	utbi_wa_ui(cut._p[0], cut._p[0], 1);
	utbi_amari(cut._p[0], cut._p[0], cut.p);

	for(i=0;i<_N_NUM-1;i++){
		utbi_wa(cut._p[i+1], cut._p[i], cut._p[0]);

		if(utbi_futougou(cut._p[i+1], cut._p[0]) < 0){
			do{
                utbi_wa(cut._p[i+1], cut._p[i+1], cut._p[0]);
			}while(utbi_futougou(cut._p[i+1], cut._p[0]) < 0);
		}
		if(utbi_futougou(cut._p[i+1], cut.p) >= 0){
			utbi_sa(cut._p[i+1], cut._p[i+1], cut.p);
		}
	}

	/*べき乗剰余処理で必要となるa^1〜a^15を作る。*/
	/*cut.workのアドレスは変化させない*/
	tmp = cut.work;
	for(i=0;i<(unt)cut.maisuu;i++){
		/*a^1を求める。*/
 		utbi_fukusha(tmp, cut.a);
		for(j=1;j<(unt)15;j++){
			tmp += yousosuu;
			utbi_jouyo(tmp, tmp-(yousosuu), cut.a, cut.p);
		}
		/*次の札に移る*/
		tmp += yousosuu;
		cut.a += yousosuu;
	}
	/*次のべき乗剰余さんのためにも*/
	/*cut.aのアドレスを元に戻しておいてやる*/
	cut.a = cut_a_adr;

	mazekoze_bekijouyo();

	/*すべてのメモリを0クリア*/
	for(i=0;i<(unt)cut.memsize;i++){
		*(cut.adr+i) = 0;
	}
	free(cut.adr);

	utbi_narabekae2(yama, fuda, maisuu, op);

}

void mazekoze_bekijouyo(void)
{
	int i, j;
	unt baa[2];
	int foo;
	unt *tmp_y, *tmp_work;
	extern int yousosuu;

	tmp_y = cut.y;
	tmp_work = cut.work;

	/*指数の値により乗剰余の値が変わるので*/
	/*指数を全部取得しておく*/
	for(i=0;i < yousosuu; i++){
		baa[0] = 8*i;
		baa[1] = *(cut.x + yousosuu - 1 - i);
		*(cut.shisuu+baa[0]+7) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+6) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+5) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+4) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+3) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+2) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+1) = baa[1] & 0xf;
		baa[1] >>= 4;
		*(cut.shisuu+baa[0]+0) = baa[1] & 0xf;
	}

	/*一回分ここで省略*/
	/*戻り値の札にまず初期値を設定*/
	/*もし、4連ビットの指数が0でなければ。*/
	foo = *(cut.shisuu);
	if(foo){
		/*アドレスの初期化*/
		cut.y = tmp_y;
		cut.work = tmp_work;
		/*指数の値は同じなのだから*/
		/*一気に縦に走らせる*/
		cut.work += ((foo-1)*yousosuu);
		for(j=0;j<cut.maisuu;j++){
			utbi_fukusha(cut.y, cut.work);
			/*次の札へと移っていく*/
			cut.y += yousosuu;
			/*次の掛けるべき値へ*/
			cut.work += 15*yousosuu;
		}
	}else{
		for(j=0;j<cut.maisuu;j++){
			utbi_fukusha_ui(cut.y, 1);
			cut.y += yousosuu;
		}
	}
	/*次の指数へ移動します。*/
	cut.shisuu++;

	for(i=1; i<8*yousosuu; i++){
		/*cut.yアドレスの初期化*/
		cut.y = tmp_y;
		/*戻り値のbekijouyoをとりあえず16乗する。*/
		for(j=0;j<cut.maisuu;j++){
			mazekoze_jouyo(cut.y, cut.y, cut.y);
			mazekoze_jouyo(cut.y, cut.y, cut.y);
			mazekoze_jouyo(cut.y, cut.y, cut.y);
			mazekoze_jouyo(cut.y, cut.y, cut.y);
			/*次の札へと移っていく*/
			cut.y += yousosuu;
		}

		/*もし、4連ビットの指数が0でなければ。*/
		foo = *(cut.shisuu);
		if(foo){
			/*アドレスの初期化*/
			cut.y = tmp_y;
			cut.work = tmp_work;
			/*指数の値は同じなのだから*/
			/*一気に縦に走らせる*/
			cut.work += ((foo-1)*yousosuu);
			for(j=0;j<cut.maisuu;j++){
				mazekoze_jouyo(cut.y, cut.y, cut.work);
				/*次の札へと移っていく*/
				cut.y += yousosuu;
				/*次の掛けるべき値へ*/
				cut.work += 15*yousosuu;
			}
		}
		/*次の指数へ移動します。*/
		cut.shisuu++;
	}
}



void mazekoze_jouyo(unt *y, unt *a, unt *x)
{
	extern int yousosuu;
	unt *Rtmp;
	unt *Ltmp;
	int i;
	uchar *www;
	unt foo;
	if(!utbi_memory(&Rtmp, 2)){
		exit(1);
	}
	Ltmp = Rtmp + yousosuu;
	utbi_shokika(Ltmp);
	utbi_shokika(Rtmp);
	/*
	                    	aaaaaaaaaaaaaaaaaaaaaaa
	                      ×xxxxxxxxxxxxxxxxxxxxxxx
	  ---------------------------------------------
	  tmptmptmptmptmptmptmptmptmptmptmptmptmptmptmp
	 */
#ifdef __ASSEMBLE_WIN32__
	_asm{
			xor ebx, ebx
			mov esi, [Rtmp]

			xor edi, edi
		loop_di:
			xor ecx, ecx
			mov edx, [x]
			mov edx, dword ptr [edx+edi*4]
			push edx
		loop_c:
			mov eax, [a]
			pop edx
			mov eax, dword ptr [eax+ecx*4]
			push edx
			/**************************/
			mul edx
			add edx, ebx
			/**/
			xor ebx, ebx
			push ecx
			add ecx, edi
			add [esi+ecx*4], eax
			adc [esi+ecx*4+4], edx
			adc ebx, 0/*桁上がりの吸収*/
			pop ecx

			inc ecx
			cmp ecx, [yousosuu]
			jne loop_c

			inc edi
			pop edx
			cmp edi, [yousosuu]
			jne loop_di

	}
#else
	//本当はここに信頼のできる積を求める処理を置く。
	//今は、utbi_seki()を置いてみる。
	//過去の私が信用できるか・・・
	utbi_seki(Rtmp, a, x);
#endif
	/*
	s=clock();
	e=clock();
	+= (double)(e-s)/CLOCKS_PER_SEC;

	*/
/*
	if(utbi_futougou(Ltmp, cut.p) >= 0){
		utbi_sa(Ltmp, Ltmp, cut.p);
	}
	if(utbi_futougou(Rtmp, cut.p) >= 0){
		utbi_sa(Rtmp, Rtmp, cut.p);
	}
*/
	/*左側は右側に比べて32*yousosuuビットほど左にずれなくてはいけない*/
	/*2乗剰余処理を32*yousosuu回ほど行う必要がある*/
	/*2005/02/19*/
	/*2乗剰余処理を4ビット単位で行うことで高速化を図る*/
	www = (uchar *)Ltmp;
	www += (sizeof(unt)*yousosuu - 1);
	//一番重要なループ、ここが高速化の鍵を握る
	//高速化対象関数：utbi_bitzurashi_hX(), utbi_wa(), utbi_futougou()
	for(i=0; i<(32/BITNUM)*yousosuu; i++){
		foo = (*(www) >> (8-BITNUM));
		utbi_bitzurashi_h8(Ltmp, Ltmp);
		if(foo){
			utbi_wa(Ltmp, Ltmp, cut._p[foo-1]);

			if(utbi_futougou(Ltmp, cut._p[foo-1]) < 0){
				utbi_wa(Ltmp, Ltmp, cut._p[0]);
			}
		}
	}




	/*これで右側と左側のビットの位置が一致した*/

	if(utbi_futougou(Ltmp, cut.p) >= 0){
		utbi_sa(Ltmp, Ltmp, cut.p);
	}

	/*最終的な処理*/
	/*左側と右側の和をとってやる*/

	utbi_wa(y, Ltmp, Rtmp);

	if(utbi_futougou(y, Rtmp) < 0){
		utbi_wa(y, y, cut._p[0]);
	}
	if(utbi_futougou(y, cut.p) >= 0){
		utbi_sa(y, y, cut.p);
	}
	/*これで乗剰余処理終了*/

	utbi_shokika(Rtmp);
	utbi_shokika(Ltmp);
	free(Rtmp);

}

