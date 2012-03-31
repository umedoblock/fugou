/* sha.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "sha.h"

void hash2data(uchar *data, unt *hash, int hashlen)
{
	unt t;
	int i, j, k;

	for(i=hashlen/(8*sizeof(unt))-1,j=0;i>=0;i--,j++){
		t = hash[i];
		for(k=0;k<(int)sizeof(unt);k++){
			data[sizeof(unt)*j+k] =
				(t >> (8 * (sizeof(unt)-1-k))) & 0xff;
		}
	}
}
int sha(uchar *hash, uchar *data, unt datasize, int shatype)
{
	int hashlen;
	SHA512 h512;
	switch(shatype){
		case 1:
		case 160:
		case 256:
		default:
			//SHA-1, SHA-256に関しては、ソースコードが酷すぎる。
			//触らぬ神に祟り無し
			/* こんな所にも書いてあった。
			 * sha.h にも書きましたが、改めてそう思いました。
			 * 平成24年 3月 31日 土曜日 11:49:32
			 */
			/* 思うだけでは無責任なので特に酷いsha1()を直しておきました。
			 * 最初、これ本当に俺が書いたか？と疑いましたが、
			 * 何となく見覚えがあったので私が書いたのだと認めました。
			 * 平成24年  3月 31日 土曜日 16:50:48
			 */
			hashlen = -1;
			break;
		case 512:
			hashlen = 512;
			sha512_Data(&h512, data, datasize);
			sha512_hashtodata(hash, &h512);
			break;
		case 384:
			hashlen = 384;
			sha384_Byte(&h512, data, datasize);
			hash2data(hash, h512.hash[0], hashlen);
			break;
	}
	return hashlen;
}

int sprintfsha(char *ss, uchar *data, unt datasize, int shatype)
{
	uchar buf[512/8];
	size_t len = 0;
	int i, hashlen;

	hashlen = sha(buf, data, datasize, shatype);
	if(hashlen < 0){
		sprintf(ss, "not supported.");
	}else{
		for(i=0;i<hashlen/8;i++){
			sprintf(ss + len, "%02x", buf[i]);
			len += 2;
		}
	}
	return hashlen;
}

void sharandom(uchar *random, unt randomsize, uchar *seed, unt seedsize)
{
	sha512_RandomData(random, randomsize, seed, seedsize);
}

