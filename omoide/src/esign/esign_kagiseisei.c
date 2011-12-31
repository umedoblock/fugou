/* esign_kagiseisei.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <time.h>
#include "esign.h"

int esign_kagiseisei(ESIGN_PUBLICKEY *pubkey, ESIGN_PRIVATEKEY *prikey, unt k, unt op)
{
	unt tmp;
	uchar *buf;

	prikey->pLen = pubkey->pLen = k;
	prikey->e = pubkey->e = 4*k;

	if(pubkey->e < k){
		return INVALID;
	}


	for(;;){
		utbi_bitset((unt)prikey->pLen);
		utbi_memory(&prikey->p, 2);
		prikey->q = prikey->p + yousosuu;

		/*素数を作る*/
		utbi_sosuu_seisei(prikey->p, (int)prikey->pLen, op);
		utbi_sosuu_seisei(prikey->q, (int)prikey->pLen, op);
		/*ここで素数ができた。*/

		/*ESIGNの仕様書の説明*/
		/*p>qを満すように*/
		if(utbi_futougou(prikey->p, prikey->q) < 0){
			utbi_irekae(prikey->p, prikey->q);
		}

		buf = (uchar *)malloc(2*(sizeof(uchar)*4*yousosuu));
		utbi_itodata(buf, prikey->p);
		utbi_itodata(buf+(sizeof(uchar)*4*yousosuu), prikey->q);

		free(prikey->p);
		tmp = prikey->pLen/8;
		utbi_bitset(3*prikey->pLen);

		utbi_memory(&prikey->p, 5);
		prikey->q = prikey->p + yousosuu;
		prikey->pq = prikey->q + yousosuu;
		prikey->n = prikey->pq + yousosuu;
		pubkey->n = prikey->n + yousosuu;

		utbi_datatoi(prikey->p, buf, tmp);
		utbi_datatoi(prikey->q, buf+tmp, tmp);
		free(buf);

		/*公開鍵n=(p^2)*qを作る*/
		utbi_seki(prikey->pq, prikey->p, prikey->q);
		utbi_seki(prikey->n, prikey->p, prikey->pq);
		if(*(prikey->n+(yousosuu-1)) & 0x80000000){
			utbi_fukusha(pubkey->n, prikey->n);
			break;
		}
		/*素数の作り直し*/
		free(prikey->p);
	}

	return GOOD;

}
