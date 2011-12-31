/* SP_esign.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "esign.h"

int SP_esign(ESIGN_SIGN *sign, ESIGN_PRIVATEKEY *prikey, unt *f)
{
	unt *r, *r_1, *r_e, *alpha;
	unt *z, *w0, *r_e_1, *t, *baa[3];
	unt *tmp;

	if(utbi_memory(&tmp, 11)==0){
		return INVALID;
	}
	r = tmp;
	r_1 = r + yousosuu;
	r_e = r_1 + yousosuu;
	r_e_1 = r_e + yousosuu;
	z = r_e_1 + yousosuu;
	alpha = z + yousosuu;
	w0 = alpha + yousosuu;
	t = w0 + yousosuu;
	baa[0] = t + yousosuu;
	baa[1] = baa[0] + yousosuu;
	baa[2] = baa[1] + yousosuu;

	utbi_shokika(sign->s);
	*sign->s = 0x00000001;
	utbi_bitzurashi_h32_si(sign->s, sign->s, (prikey->pLen)/32);
	utbi_bitzurashi_m(sign->s, sign->s);
	/*1. fが0<=f<2^(pLen-1)かどうか確認*/
	if(utbi_futougou_ui(f, 0)>=0&&utbi_futougou(f, sign->s)<0){
		/*署名値は正しい。*/
	}else{
		free(tmp);
		return INVALID;
	}

 	utbi_seki(prikey->pq, prikey->p, prikey->q);
	utbi_seki(prikey->n, prikey->p, prikey->pq);

	/*乱数生成*/
	/*fprintf(stderr, "署名生成時に必要な乱数を作っています。\n");*/
	utbi_ransuu(r, 2 * prikey->pLen + 32);

	/*2. gcd(r, n)=1 r∈{1, 2, 3, ・・・・, pq-1}*/
	utbi_amari(r, r, prikey->pq);
	utbi_gcdext(baa[0], r_1, baa[1], r, prikey->pq);
	while(utbi_futougou_ui(baa[0], 1)!=0){
		utbi_ransuu(r, 2 * prikey->pLen + 32);
		utbi_amari(r, r, prikey->pq);
		utbi_gcdext(baa[0], r_1, baa[1], r, prikey->pq);
	}


	/*3. z=f*2^(2pLen)*/
	utbi_bitzurashi_h32_si(z, f, (2*prikey->pLen)/32);

	/*4. alpha=(z-r^e) mod n*/
	utbi_bekijouyo_ui(r_e, r, prikey->e, prikey->n);
	if(utbi_futougou(z, r_e) < 0){
		utbi_wa(z, z, prikey->n);
		utbi_sa(alpha, z, r_e);
	}else{
		utbi_sa(alpha, z, r_e);
	}
	utbi_amari(alpha, alpha, prikey->n);


	/*5. w=|a/pq| となる最小の自然数　余があれば1加える*/
	utbi_shou_amari(w0, baa[1], alpha, prikey->pq);
	if(utbi_futougou_ui(baa[1], 0) != 0){
		utbi_wa_ui(w0, w0, 1);
	}

	/*6-1. t=(w/ {e*r^(e-1)} ) mod p*/
	/* r^(e-1) = r^e * r^(-1) */
	utbi_jouyo(r_e_1, r_e, r_1, prikey->p);

	/* e*r^(e-1) = e*r^(e-1) */
	utbi_jouyo_ui(baa[1], r_e_1, prikey->e, prikey->p);

	/* 1/{e*r^(e-1)} = {e*r^(e-1)}^(-1) */
	/* zの値はいらない。ただ数あわせで入れただけ*/
	utbi_gcdext(baa[0], baa[2], z, baa[1], prikey->p);

	/* t=(w/ {e*r^(e-1)} )*/
	utbi_jouyo(t, w0, baa[2], prikey->p);

	/*6-2. s = r + tpq*/
	utbi_seki(sign->s, t, prikey->pq);
	utbi_wa(sign->s, sign->s, r);

	free(tmp);

	return GOOD;
}
