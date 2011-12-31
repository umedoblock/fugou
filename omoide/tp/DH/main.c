/* main.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../../utbi_sanjutsu/src/utbi_sanjutsu.h"
#include "../../DH/src/DH.h"
#include <stdio.h>

int main(void)
{
	unt *gcd, *foo, *baa, *x_1;
	unt i, j;
	int sw;
	uchar data[512/8];
	DH_PRIVATEKEY dpri, pri[2];
	DH_PUBLICKEY dpub, pub[2];
	DH_SHAREKEY shr[2];
	SHA512 h[2];

	char ss[80];

	printf("mode\n");
	printf("1 :原始根探し\n");
	printf("2 :公開鍵・秘密鍵作り\n");
	printf("3 :鍵共有\n");
	gets(ss);
	sw = atoi(ss);
	switch(sw){
		case 1:
			/*原始根探し*/
			utbi_bitset(g.pLen);
			utbi_memory(&g.p, 1);
			utbi_xtoi(g.p, "93275b2366ab7f8fe8570957947fc7504bfa0517b1664b1a442be8cfb920dd68641c7f606e9fe6ffd09f7129bc137145ae836216024e7e6883f84b6ae82db0682e2a00ac6da1a0d597dcf31d3bfae7ec41bc456df9f9398735fb2fcbd940f37925402c864f85e95b811cba4ac254d1a5a460c8557ab479f753dc0d3c77b8b4e3");

			utbi_putx(g.p);puts("");

			j = dh_g(&g);

			for(i=0;i<j;i++){
				printf("g[%d] = %u\n", i, g.g[i]);
			}

			dh_FoutG(g, j);

			free(g.p);
			break;
		case 2:
			/*公開鍵・秘密鍵作り*/
			dpub.pLen = dpri.pLen = 1024;
			dh_pubmalloc(&dpub);
			dh_primalloc(&dpri);

			utbi_memory(&gcd, 4);
			x_1 = gcd + yousosuu;
			foo = x_1 + yousosuu;
			baa = foo + yousosuu;
			
			utbi_xtoi(dpri.p, "93275b2366ab7f8fe8570957947fc7504bfa0517b1664b1a442be8cfb920dd68641c7f606e9fe6ffd09f7129bc137145ae836216024e7e6883f84b6ae82db0682e2a00ac6da1a0d597dcf31d3bfae7ec41bc456df9f9398735fb2fcbd940f37925402c864f85e95b811cba4ac254d1a5a460c8557ab479f753dc0d3c77b8b4e3");
			utbi_fukusha(dpub.p, dpri.p);
			utbi_fukusha_ui(dpri.g, 0xffb);
			utbi_fukusha(dpub.g, dpri.g);

			dh_keygen(&dpub, &dpri, 1);

			printf("DH PRIVATEKEY\n");
			printf("p =\n");
			utbi_putx(dpri.p);puts("");
			printf("g =\n");
			utbi_putx(dpri.g);puts("");
			printf("x =\n");
			utbi_putx(dpri.x);puts("");
			printf("gx =\n");
			utbi_putx(dpri.gx);puts("");

			puts("");

			printf("DH PUBLICKEY\n");
			printf("p =\n");
			utbi_putx(dpub.p);puts("");
			printf("g =\n");
			utbi_putx(dpub.g);puts("");
			printf("gx =\n");
			utbi_putx(dpub.gx);puts("");

			puts("");


			utbi_sa_ui(baa, dpri.p, 1);
			utbi_gcdext(gcd, x_1, foo, dpri.x, baa);
			if(utbi_futougou_ui(gcd, 1)==0){
				printf("good\n");
			}else{
				printf("bad\n");
				putchar('\a');
			}
			utbi_shokika(foo);

			utbi_bekijouyo(foo, dpri.gx, x_1, dpri.p);
			printf("(g^x)^(x^-1) =\n");
			utbi_putx(foo);puts("");

			dh_pubfree(&dpub);
			dh_prifree(&dpri);
			free(gcd);

			break;

		case 3:
			/*共通鍵作り*/
			pub[0].pLen = pub[1].pLen = pri[0].pLen = pri[1].pLen = 1024;

			dh_pubmalloc(&pub[0]);
			dh_pubmalloc(&pub[1]);
			dh_primalloc(&pri[0]);
			dh_primalloc(&pri[1]);
			dh_shrmalloc(&shr[0]);
			dh_shrmalloc(&shr[1]);

			utbi_xtoi(pub[0].p, "93275b2366ab7f8fe8570957947fc7504bfa0517b1664b1a442be8cfb920dd68641c7f606e9fe6ffd09f7129bc137145ae836216024e7e6883f84b6ae82db0682e2a00ac6da1a0d597dcf31d3bfae7ec41bc456df9f9398735fb2fcbd940f37925402c864f85e95b811cba4ac254d1a5a460c8557ab479f753dc0d3c77b8b4e3");
			utbi_fukusha(pub[1].p, pub[0].p);
			utbi_fukusha(pri[0].p, pub[1].p);
			utbi_fukusha(pri[1].p, pri[0].p);

			utbi_fukusha_ui(pub[0].g, 0x7);
			utbi_fukusha(pub[1].g, pub[0].g);
			utbi_fukusha(pri[0].g, pub[1].g);
			utbi_fukusha(pri[1].g, pri[0].g);

/*
			dh_keygen(&pub[0], &pri[0], 1);
*/
			/*[0]が暗号文の送信者*/
			/*[1]は暗号文の受信者*/

			/*[1]の公開鍵・秘密鍵が必要*/
			dh_keygen(&pub[1], &pri[1], 1);

			/*[1]の公開鍵を使って共有鍵を作る。*/
			dh_enkey(&shr[0], &pub[1], 1);
			utbi_shokika(shr[0].y);

			/*[0]->[1]と送信する。*/
			utbi_fukusha(shr[1].gy, shr[0].gy);

			/*[1]の秘密鍵を使って、共有鍵を作る。*/
			dh_dekey(&shr[1], &pri[1]);

			/*gxy[0], gxy[1]が一致するか確認。*/
			if(utbi_futougou(shr[0].gxy, shr[1].gxy)==0){
				printf("good g^xy\n\n");
			}else{
				printf("bad g^xy\n\n");
			}

			dh_keyhash(&h[0], &shr[0]);
			dh_keyhash(&h[1], &shr[1]);

			printf("sha512_h[0] =\n");
			sha512_puthash(h[0]);
			printf("sha512_h[1] =\n");
			sha512_puthash(h[1]);

			sha512_hashtodata(data, &h[0]);

			printf("hashtodata =\n");
			for(i=0,j=0;i<512/8;i++){
				printf("%02x", data[i]);
				if(i%8==7)putchar(' ');
				if(i%32==31)puts("");
			}



			/*key[0], key[1]が一致するか確認。*/
			if(sha512_equal(h[0], h[1])){
				printf("\ngood key\n");
			}else{
				printf("\nbad key\n");
			}

			dh_pubfree(&pub[0]);
			dh_pubfree(&pub[1]);
			dh_prifree(&pri[0]);
			dh_prifree(&pri[1]);
			dh_shrfree(&shr[0]);
			dh_shrfree(&shr[1]);

			break;
		default:
			break;
	}

	printf("何かキーを押してください。");
	getchar();
	return 0;
}
