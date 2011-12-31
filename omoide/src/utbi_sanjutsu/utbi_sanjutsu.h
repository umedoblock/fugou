/* utbi_sanjutsu.h
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#ifndef __UTBI_SANJUTSU_H__
#define __UTBI_SANJUTSU_H__

/*	static double t = 0.0;
	clock_t s, e;
	s = clock();
	e = clock();

	t += (double)(e-s)/CLOCKS_PER_SEC;
	printf("utbi_fukusha() = %lf\n", t);
*/

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __TYPEDEF_UNSIGNED__
#define __TYPEDEF_UNSIGNED__
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int unt;
#endif /*__TYPEDEF_UNSIGNED__*/

#ifndef __yousosuu__
#define __yousosuu__
//int yousosuu;
#endif

#define ERROR_NULL   0x01000000
#define ERROR_MALLOC 0x02000000
#define ERROR_FOPEN  0x04000000

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/*2005/02/08*/

unt ctoi(unt atai, uchar x);
void utbi_wa(unt *z, unt *x, unt *y);
void utbi_wa_ui(unt *z_wa, unt *x_wa, unt y_wa);
void utbi_sa(unt *z, unt *x, unt *y);
void utbi_sa_ui(unt *z_sa, unt *x_sa, unt y_sa);
void utbi_seki(unt *mdr_seki, unt *iroha_seki, unt *nihohe_seki);
/*2005/02/11*/
void utbi_seki_ui(unt *mdr_seki, unt *iroha_seki, unt nihohe_seki);
void utbi_shou(unt *mdr_shou, unt *wararerukazu_shou, unt *warukazu_shou);
void utbi_amari(unt *y, unt *a, unt *n);
unt utbi_amari_ui(unt *a, unt n);
/*2004/11/07*/
void utbi_shou_amari(unt *mdr_shou, unt *mdr_amari, /**/unt *a, unt *x);
/*2004/11/08*/
void utbi_jouyo(unt *z_jouyo, unt *x_jouyo, unt *y_jouyo, unt *n_jouyo);
/*2005/02/13*/
void utbi_jouyo_ui(unt *z_jouyo, unt *x_jouyo, unt y_jouyo, unt *n_jouyo);
void utbi_nijou(unt *mdr_nijou, unt *nijounomoto);
void utbi_bekijou(unt *mdr_bekijou, unt *kisuu_bekijou, unt *bekisuu_bekijou);
/*2004/11/07*/
void utbi_bekijouyo(unt *mdr_bekijouyo, unt *kisuu_bekijouyo, unt *bekisuu_bekijouyo, unt *houtosurukazu_bekijouyo);
/*2005/02/13*/
void utbi_bekijouyo_ui(unt *mdr_bekijouyo, unt *kisuu_bekijouyo, unt shisuu_bekijouyo, unt *houtosurukazu_bekijouyo);
/*2004/11/07*/
void utbi_gcd(unt *mdr_gcd, unt *iroha_gcd, unt *nihohe_gcd);
void utbi_gcdext(unt *mdr_gcd, unt *mdr_x, unt *mdr_y, unt *a_iroha, unt *b_nihohe);
int utbi_atamadase(unt x);
void utbi_hanten(unt *mdr_hanten, unt *iroha_hanten);
void utbi_shokika(unt *shokika);
void utbi_fukusha(unt *fukushasaki, unt *fukushamoto);
void utbi_fukusha_ui(unt *fukushasaki, unt fukushamoto);
void utbi_and(unt *mdr_and, unt *and_iroha, unt *and_nihohe);
void utbi_ior(unt *mdr_ior, unt *ior_iroha, unt *ior_nihohe);
void utbi_or(unt *mdr_or, unt *or_iroha, unt *or_nihohe);
void utbi_xor(unt *mdr_xor, unt *xor_iroha, unt *xor_nihohe);
void utbi_bitzurashi_h(unt *y, unt *x);
void utbi_bitzurashi_h4(unt *, unt *);
void utbi_bitzurashi_h8(unt *, unt *);
void utbi_bitzurashi_h_si(unt *y, unt *x, int j);
void utbi_bitzurashi_h32_si(unt *y, unt *x, int j);
void utbi_bitzurashi_m(unt *y, unt *x);
void utbi_bitzurashi_m_si(unt *y, unt *x, int j);
void utbi_bitzurashi_m32_si(unt *y, unt *x, int j);
unt bitkaiten_h_si(unt kaitenmoto, int j);
int utbi_futougou(unt *iroha_futougou, unt *nihohe_futougou);
int utbi_futougou_ui(unt *iroha_futougou, unt nihohe_futougou);
unt xtoi(unt atai, char x);
void utbi_xtoi(unt *x, char *ss);
void utbi_dtoi(unt *dtoi, wchar_t *wss);
/*2005/02/23*/
void utbi_itox(wchar_t *wss, unt *x);
void utbi_putx(unt *x_p);
void utbi_put0x(unt *x_p);
void utbi_putX(unt *x_p);
void utbi_put0X(unt *x_p);
void utbi_fputx(FILE *fp, unt *x_p);
void utbi_fputX(FILE *fp, unt *x_p);
void utbi_fput0x(FILE *fp, unt *x_p);
void utbi_fput0X(FILE *fp, unt *x_p);
/*2005/04/11*/
void utbi_fwrite(FILE *fp, unt *x);
unt utbi_fread(unt *x, FILE *fp);
/*2005/04/05*/
void utbi_datatoi(unt *, uchar *, unt);
void utbi_itodata(uchar *, unt *);
int utbi_ret(void);
unt *utbi_retaddr(unt *base, int num);

void utbi_putss(unt *ss_p);
void utbi_irekae(unt *irekae_iroha, unt *irekae_nihohe);
void utbi_narabekae(unt *qwp, int kaisuu, int op);
void utbi_narabekae2(unt *qwp, unt *juuzoku, int kaisuu, int op);
void mazekoze(unt *yama, unt *fuda, unt *ransuu, unt *n, int maisuu, int op);

/*乱数関係*/
void utbi_ransuu(unt *ransuu, int bLen);
unt utbi_ransuu32(void);

/*素数関係*/
/*2004/11/11*/
int utbi_sosuu_maeshori(unt *sosuukouho);
/*2004/11/10*/
int utbi_sosuu_lehmann(unt *hanteisurusosuu, int i);
/*2004/11/28*/
int utbi_sosuu_miller_rabin(unt *sosuukouho, int i);
/*2005/02/13*/
int utbi_sosuu_hantei(unt *p, int kaisuu);
/*2005/02/13*/
int utbi_sosuu_seisei(unt *p, int bLen, unsigned int op);
/*2005/04/07*/
int utbi_isg(unt g, unt *p);

/*2005/04/05*/
void utbi_bitset(int bLen);

/*2005/02/17*/
#ifndef __KAIGYOU__
#define __KAIGYOU__
void kaigyou(wchar_t *p);
#endif
/*2005/03/06*/
void sleep(double wtime);

/*2005/04/05*/
int utbi_memory(unt **x, int n);

#ifdef __cplusplus
}
#endif

#endif/*__UTBI_SANJUTSU_H__*/

/*
#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif
*/

