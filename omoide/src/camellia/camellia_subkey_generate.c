/* camellia_subkey_generate.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

void camellia_subkey(unt subkey[][2], unt KEY[][2], int keysize)
{

	unt sigma1[2] = {0x3BCC908B, 0xA09E667F};
	unt sigma2[2] = {0x4CAA73B2, 0xB67AE858};
	unt sigma3[2] = {0xE94F82BE, 0xC6EF372F};
	unt sigma4[2] = {0xF1D36F1C, 0x54FF53A5};
	unt sigma5[2] = {0xDE682D1D, 0x10E527FA};
	unt sigma6[2] = {0xB3E6C1FD, 0xB05688C2};

	unt *keyLL, *keyLR, *keyRL, *keyRR;
	unt *keyAL, *keyAR, *keyBL, *keyBR;
	unt tmpL[2], tmpR[2], tmp[2], w[2];

	keyRR = subkey[0];
	keyRL = subkey[1];

	keyLR = subkey[2];
	keyLL = subkey[3];

	keyBR = subkey[4];
	keyBL = subkey[5];

	keyAR = subkey[6];
	keyAL = subkey[7];

	if(keysize == 128){
		camellia_copy(keyLR, KEY[0]);
		camellia_copy(keyLL, KEY[1]);
		camellia_copy(tmpL, keyLL);
		camellia_copy(tmpR, keyLR);
	}else {
		if(keysize == 192){
			camellia_copy(keyRL, KEY[0]);
			camellia_copy(keyLR, KEY[1]);
			camellia_copy(keyLL, KEY[2]);
			camellia_not(keyRR, keyRL);
        }else{
			/*keysize == 256*/
			camellia_copy(keyRR, KEY[0]);
			camellia_copy(keyRL, KEY[1]);
			camellia_copy(keyLR, KEY[2]);
			camellia_copy(keyLL, KEY[3]);
		}
		camellia_xor(tmpR, keyLR, keyRR);
		camellia_xor(tmpL, keyLL, keyRL);
	}

	camellia_copy(w, tmpL);
	camellia_F(tmpL, tmpL, sigma1);
	camellia_xor(tmpL, tmpL, tmpR);
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	camellia_F(tmpL, tmpL, sigma2);
	camellia_xor(tmpL, tmpL, tmpR);
	camellia_copy(tmpR, w);

	camellia_xor(tmpL, tmpL, keyLL);
	camellia_xor(tmpR, tmpR, keyLR);

	camellia_copy(w, tmpL);
	camellia_F(tmpL, tmpL, sigma3);
	camellia_xor(tmpL, tmpL, tmpR);
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	camellia_F(tmpL, tmpL, sigma4);
	camellia_xor(tmpL, tmpL, tmpR);
	camellia_copy(tmpR, w);

	camellia_copy(keyAR, tmpR);
	camellia_copy(keyAL, tmpL);

	if(keysize == 128){
		camellia_zero(tmpR);
		camellia_zero(tmpL);
		camellia_zero(tmp);
		camellia_zero(w);
		return;
	}

	camellia_xor(tmpR, tmpR, keyRR);
	camellia_xor(tmpL, tmpL, keyRL);

	camellia_copy(w, tmpL);
	camellia_F(tmpL, tmpL, sigma5);
	camellia_xor(tmpL, tmpL, tmpR);
	camellia_copy(tmpR, w);

	camellia_copy(w, tmpL);
	camellia_F(tmpL, tmpL, sigma6);
	camellia_xor(tmpL, tmpL, tmpR);
	camellia_copy(tmpR, w);

	camellia_copy(keyBR, tmpR);
	camellia_copy(keyBL, tmpL);

	camellia_zero(tmpR);
	camellia_zero(tmpL);
	camellia_zero(tmp);
	camellia_zero(w);

}

void camellia_subkey_all(unt subkey_all[][2], unt subkey[][2], int keysize)
{
	int i;

	unt *keyL, *keyR, *keyLL, *keyLR, *keyRL, *keyRR;
	unt *keyA, *keyB, *keyAL, *keyAR, *keyBL, *keyBR;


	/*一時副鍵*/
	unt tmp[8][2];

	keyRR = keyR = tmp[0];
	keyRL = tmp[1];

	keyLR = keyL = tmp[2];
	keyLL = tmp[3];

	keyBR = keyB = tmp[4];
	keyBL = tmp[5];

	keyAR = keyA = tmp[6];
	keyAL = tmp[7];

	camellia_copy(keyRR, subkey[0]);
	camellia_copy(keyRL, subkey[1]);
	camellia_copy(keyLR, subkey[2]);
	camellia_copy(keyLL, subkey[3]);
	camellia_copy(keyBR, subkey[4]);
	camellia_copy(keyBL, subkey[5]);
	camellia_copy(keyAR, subkey[6]);
	camellia_copy(keyAL, subkey[7]);

	if(keysize==128){

		/*kw1, kw2*/
		camellia_copy(subkey_all[0], keyLL);
		camellia_copy(subkey_all[1], keyLR);
		

		/*k1, k2*/
		camellia_copy(subkey_all[2], keyAL);
		camellia_copy(subkey_all[3], keyAR);
		

		/*k3, k4*/
		camellia_hkaiten(keyL, 15);
		camellia_copy(subkey_all[4], keyLL);
		camellia_copy(subkey_all[5], keyLR);
		

		/*k5, k6*/
		camellia_hkaiten(keyA, 15);
		camellia_copy(subkey_all[6], keyAL);
		camellia_copy(subkey_all[7], keyAR);
		

		/*kL1, kL2*/
		camellia_hkaiten(keyA, 15);
		camellia_copy(subkey_all[8], keyAL);
		camellia_copy(subkey_all[9], keyAR);
		

		/*k7, k8*/
		camellia_hkaiten(keyL, 30);
		camellia_copy(subkey_all[10], keyLL);
		camellia_copy(subkey_all[11], keyLR);
		

		/*k9*/
		camellia_hkaiten(keyA, 15);
		camellia_copy(subkey_all[12], keyAL);
		

		/*k10*/
		camellia_hkaiten(keyL, 15);
		camellia_copy(subkey_all[13], keyLR);
		

		/*k11, k12*/
		camellia_hkaiten(keyA, 15);
		camellia_copy(subkey_all[14], keyAL);
		camellia_copy(subkey_all[15], keyAR);
		

		/*kL3, kL4*/
		camellia_hkaiten(keyL, 17);
		camellia_copy(subkey_all[16], keyLL);
		camellia_copy(subkey_all[17], keyLR);
		

		/*k13, k14*/
		camellia_hkaiten(keyL, 17);
		camellia_copy(subkey_all[18], keyLL);
		camellia_copy(subkey_all[19], keyLR);
		

		/*k15, k16*/
		camellia_hkaiten(keyA, 34);
		camellia_copy(subkey_all[20], keyAL);
		camellia_copy(subkey_all[21], keyAR);
		

		/*k17, k18*/
		camellia_hkaiten(keyL, 17);
		camellia_copy(subkey_all[22], keyLL);
		camellia_copy(subkey_all[23], keyLR);
		

		/*kw3, kw4*/
		camellia_hkaiten(keyA, 17);
		camellia_copy(subkey_all[24], keyAL);
		camellia_copy(subkey_all[25], keyAR);
		
	}else{
		/*(keysize == 192 || keysize == 256)*/

		/*kw1, kw2*/
		camellia_copy(subkey_all[0], keyLL);
		camellia_copy(subkey_all[1], keyLR);
		

		/*k1, k2*/
		camellia_copy(subkey_all[2], keyBL);
		camellia_copy(subkey_all[3], keyBR);
		

		/*k3, k4*/
		camellia_hkaiten(keyR, 15);
		camellia_copy(subkey_all[4], keyRL);
		camellia_copy(subkey_all[5], keyRR);
		

		/*k5, k6*/
		camellia_hkaiten(keyA, 15);
		camellia_copy(subkey_all[6], keyAL);
		camellia_copy(subkey_all[7], keyAR);
		

		/*kL1, kL2*/
		camellia_hkaiten(keyR, 15);
		camellia_copy(subkey_all[8], keyRL);
		camellia_copy(subkey_all[9], keyRR);
		

		/*k7, k8*/
		camellia_hkaiten(keyB, 30);
		camellia_copy(subkey_all[10], keyBL);
		camellia_copy(subkey_all[11], keyBR);
		

		/*k9, k10*/
		camellia_hkaiten(keyL, 45);
		camellia_copy(subkey_all[12], keyLL);
		camellia_copy(subkey_all[13], keyLR);
		

		/*k11, k12*/
		camellia_hkaiten(keyA, 30);
		camellia_copy(subkey_all[14], keyAL);
		camellia_copy(subkey_all[15], keyAR);
		

		/*kL3, kL4*/
		camellia_hkaiten(keyL, 15);
		camellia_copy(subkey_all[16], keyLL);
		camellia_copy(subkey_all[17], keyLR);
		

		/*k13, k14*/
		camellia_hkaiten(keyR, 30);
		camellia_copy(subkey_all[18], keyRL);
		camellia_copy(subkey_all[19], keyRR);
		

		/*k15, k16*/
		camellia_hkaiten(keyB, 30);
		camellia_copy(subkey_all[20], keyBL);
		camellia_copy(subkey_all[21], keyBR);
		

		/*k17, k18*/
		camellia_hkaiten(keyL, 17);
		camellia_copy(subkey_all[22], keyLL);
		camellia_copy(subkey_all[23], keyLR);
		

		/*kL5, kL6*/
		camellia_hkaiten(keyA, 32);
		camellia_copy(subkey_all[24], keyAL);
		camellia_copy(subkey_all[25], keyAR);
		

		/*k19, k20*/
		camellia_hkaiten(keyR, 34);
		camellia_copy(subkey_all[26], keyRL);
		camellia_copy(subkey_all[27], keyRR);
		

		/*k21, k22*/
		camellia_hkaiten(keyA, 17);
		camellia_copy(subkey_all[28], keyAL);
		camellia_copy(subkey_all[29], keyAR);
		

		/*k23, k24*/
		camellia_hkaiten(keyL, 34);
		camellia_copy(subkey_all[30], keyLL);
		camellia_copy(subkey_all[31], keyLR);
		

		/*kw3, kw4*/
		camellia_hkaiten(keyB, 51);
		camellia_copy(subkey_all[32], keyBL);
		camellia_copy(subkey_all[33], keyBR);
		
	}

	/*一時副鍵を消去*/
	for(i=0;i<16;i++){
		tmp[i/2][i&1] = 0;
	}
}
