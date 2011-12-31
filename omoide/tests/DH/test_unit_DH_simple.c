/* test_unit_DH_simple.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <time.h>
#include "../src/DH.h"

int main(int argc, char *argv[])
{
	unt *ptr, *p, *g;
	unt *x, *gx, *gxy;
	unt *y, *gy, *gyx;
	unt *gcd, *x_1, *y_1, *x_, *y_;
	unt *aaa, *bbb, *p_1;
	unt *xx, *yy;
	unt **foo[] = {
		&p, &g, &x, &y, &gx, &gy,
		&gxy, &gyx, &gcd, &x_1, &y_1,
		&x_, &y_, &p_1, &aaa, &bbb,
		&xx, &yy
	};
	time_t s, e;
	int blen = 100, i;
	int num = 0, pass = 0, fail = 0, t_fail;
	extern int yousosuu;
	//gcd(x, p) = 1を満たす自然数xを見付け、
	//x * x_1 ≡  1 (mod p - 1)を満たすx_1を見付ける。

	if(argc < 3){
		fprintf(stdout, "please bit Length, times\n");
		return -1;
	}
	blen = atoi(argv[1]);
	num = atoi(argv[2]);

	utbi_bitset(blen);
	utbi_memory(&ptr, 18);
	*foo[0] = ptr;
	for(i=0;i<18-1;i++){
		*foo[i+1] = *foo[i] + yousosuu;
	}

	fprintf(stdout, "素数作成開始\n");
	time(&s);
	utbi_sosuu_seisei(p, blen, 0);
	time(&e);
	fprintf(stdout, "素数作成完了\n");
	fprintf(stdout, "素数作成時間:%lf\n", difftime(e, s));

	utbi_fukusha_ui(g, 71);
	t_fail = fail;
	time(&s);
	for(i=0;i<num;i++){
		if(t_fail != fail)fprintf(stdout, "i = %d\n", i);

		t_fail = fail;
		utbi_ransuu(x, blen);
		utbi_ransuu(y, blen);
		utbi_bekijouyo(gx,  g, x, p);
		utbi_bekijouyo(gy,  g, y, p);
		utbi_bekijouyo(gxy, gx, y, p);
		utbi_bekijouyo(gyx, gy, x, p);
		if(utbi_futougou(gxy, gyx) != 0){
			if(t_fail == fail)fprintf(stdout, "--\n");
			fprintf(stdout, "gxy != gyx\n");
			fprintf(stdout, "   g = 0x");utbi_putx(g);fprintf(stdout, "\n");
			fprintf(stdout, "   p = 0x");utbi_putx(p);fprintf(stdout, "\n");
			fprintf(stdout, " gxy = 0x");utbi_putx(gxy);fprintf(stdout, "\n");
			fprintf(stdout, " gyx = 0x");utbi_putx(gyx);fprintf(stdout, "\n");
			fprintf(stdout, "   x = 0x");utbi_putx(x);fprintf(stdout, "\n");
			fprintf(stdout, "   y = 0x");utbi_putx(y);fprintf(stdout, "\n");
			fail++;
		}

		utbi_sa_ui(p_1, p, 1);

		utbi_gcdext(gcd, x_1, aaa, x, p_1);
		utbi_bekijouyo(aaa, g, gcd, p);
		utbi_bekijouyo(x_, gx, x_1, p);
		if(utbi_futougou(x_, aaa) != 0){
			if(t_fail == fail)fprintf(stdout, "--\n");
			fprintf(stdout, "x_ != g ^ gcd\n");
			fail++;
		}

		utbi_gcdext(gcd, y_1, bbb, y, p_1);
		utbi_bekijouyo(bbb, g, gcd, p);
		utbi_bekijouyo(y_, gy, y_1, p);
		if(utbi_futougou(y_, bbb) != 0){
			if(t_fail == fail)fprintf(stdout, "--\n");
			fprintf(stdout, "y_ != g ^ gcd\n");
			fail++;
		}

		if(utbi_futougou(x_, y_) != 0){
			utbi_jouyo(aaa, x, x_1, p_1);
			utbi_jouyo(bbb, y, y_1, p_1);
			utbi_bekijouyo(xx, g, aaa, p);
			utbi_bekijouyo(yy, g, bbb, p);
			fprintf(stdout, "もしかしたら・・・ i = %d\n", i);
			if(utbi_futougou(xx, x_) != 0 || utbi_futougou(yy, y_) != 0){
				if(t_fail == fail)fprintf(stdout, "--\n");
				fprintf(stdout, "x_ != y_\n");
				fprintf(stdout, "   g = 0x");utbi_putx(g);fprintf(stdout, "\n");
				fprintf(stdout, "   p = 0x");utbi_putx(p);fprintf(stdout, "\n");
				fprintf(stdout, "   x = 0x");utbi_putx(x);fprintf(stdout, "\n");
				fprintf(stdout, "   y = 0x");utbi_putx(y);fprintf(stdout, "\n");
				fprintf(stdout, "  x_ = 0x");utbi_putx(x_);fprintf(stdout, "\n");
				fprintf(stdout, "  y_ = 0x");utbi_putx(y_);fprintf(stdout, "\n");
				fprintf(stdout, "g^(x*x^-1 mod p-1)= 0x");utbi_putx(aaa);fprintf(stdout, "\n");
				fprintf(stdout, "g^(y*y^-1 mod p-1)= 0x");utbi_putx(bbb);fprintf(stdout, "\n");
				fail++;
			}
		}
		if(t_fail == fail) pass++;
	}
	time(&e);

	fprintf(stdout, "(num, pass, fail) = (%d, %d, %d)\n", num, pass, fail);
	fprintf(stdout, "経過時間:%lf\n", difftime(e, s));

	free(ptr);
	return 0;
}

