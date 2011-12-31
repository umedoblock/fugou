/* camellia_cbc.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

void strtodata(uchar *data, char *ss, unt n);
void datatostr(char *ss, uchar *data, unt n);

#ifdef __cplusplus
#define BUFFERSIZE (1024)

/****************************ENCRYPT*****************************/
/*2005/03/30*/
void camellia_encrypt_cbc(uchar *c, uchar *m, uchar *iv, CAMELLIA_KEY *cmENkey)
{
	unt *M = (unt *)m, *IV = (unt *)iv, *C = (unt *)c;

	C[0] = M[0] ^ IV[0];
	C[1] = M[1] ^ IV[1];
	C[2] = M[2] ^ IV[2];
	C[3] = M[3] ^ IV[3];
	camellia_encrypt(c, c, cmENkey);
}
uchar *camellia_encrypt_cbc_block(uchar *c, uchar *m, uchar *iv, unt blocks, CAMELLIA_KEY *cmENkey)
{
	unt i;
	for(i=blocks;i>0;i--){
		camellia_encrypt_cbc(c, m, iv, cmENkey);
		iv = c;
		m += CM_BLOCKSIZE;
		c += CM_BLOCKSIZE;
	}

	return iv;
}
unt camellia_encrypt_cbc_pad(uchar *c, uchar *m, uchar *iv, unt Byte, CAMELLIA_KEY *cmENkey)
{
	unt pByte;
	pByte = CM_BLOCKSIZE - Byte;
	memcpy(c, m, Byte);

	camellia_padding(c + Byte, pByte);
	c[CM_BLOCKSIZE-1] &= 0xf0;
	c[CM_BLOCKSIZE-1] |= (uchar)Byte;

    camellia_encrypt_cbc(c, c, iv, cmENkey);
	return 1;
}
int camellia_encrypt_cbc_DataData(uchar *c, uchar *m, uchar *iv, unt Byte, CAMELLIA_KEY *cmENkey)
{
	iv = camellia_encrypt_cbc_block(c, m, iv, Byte/CM_BLOCKSIZE, cmENkey);
	c = iv + CM_BLOCKSIZE;
	m += (Byte & (~(CM_BLOCKSIZE-1)));
	camellia_encrypt_cbc_pad(c, m, iv, Byte&(CM_BLOCKSIZE-1), cmENkey);
	return 1;

}

/*2005/03/30*/
int camellia_encrypt_cbc_FileFile(FILE *fc, FILE *fm, uchar *iv, ulong Hi, ulong Low, CAMELLIA_KEY *cmENkey)
{
	uchar rbuf[BUFFERSIZE+CM_BLOCKSIZE];
	int n;
	register int i;
	ulong rByte, nblocks;//rByte readByte

	//4GByteごとの処理
	while(Hi){
		//ここはあまり高速化には関係ないかも

		//2^15回ループ
		//for(i=0;i<(0x80000000)/(BUFFERSIZE)*2;i++){
		for(i=0;i<0x400000;i++){
			//ここのループが重要
			fread(rbuf, BUFFERSIZE, sizeof(uchar), fm);
			iv = camellia_encrypt_cbc_block(rbuf, rbuf, iv, BUFFERSIZE/CM_BLOCKSIZE, cmENkey);
			fwrite(rbuf, BUFFERSIZE, sizeof(uchar), fc);
			memcpy(&rbuf[BUFFERSIZE], iv, CM_BLOCKSIZE);
			iv = &rbuf[BUFFERSIZE];
		}
		Hi--;
	}

	//4GByte未満の処理
	n = Low / BUFFERSIZE;
	//Low & (BUFFERSIZE-1) = Low % BUFFERSIZE
	Low &= (BUFFERSIZE-1);
	for(i=0;i<n;i++){
		//ここのループが重要
		//128KByteごとに一気に処理
		fread(rbuf, BUFFERSIZE, sizeof(uchar), fm);
		iv = camellia_encrypt_cbc_block(rbuf, rbuf, iv, BUFFERSIZE/CM_BLOCKSIZE, cmENkey);
		fwrite(rbuf, BUFFERSIZE, sizeof(uchar), fc);
		memcpy(&rbuf[BUFFERSIZE], iv, CM_BLOCKSIZE);
		iv = &rbuf[BUFFERSIZE];
	}

	//128KByte未満の処理
	nblocks = (Low/CM_BLOCKSIZE);
	if(nblocks){
		//Low & (~(CM_BLOCKSIZE-1)) = nblocks*CM_BLOCKSIZE
		fread(rbuf, nblocks * CM_BLOCKSIZE, sizeof(uchar), fm);
		iv = camellia_encrypt_cbc_block(rbuf, rbuf, iv, nblocks, cmENkey);
		fwrite(rbuf, nblocks * CM_BLOCKSIZE, sizeof(uchar), fc);
		//ivの置き場所はrbufの最後
		memcpy(&rbuf[BUFFERSIZE], iv, CM_BLOCKSIZE);
		iv = &rbuf[BUFFERSIZE];
	}

	//CM_BLOCKSIZE未満の端数処理, パディング
	//Low & (CM_BLOCKSIZE-1) = Low % CM_BLOCKSIZE
	rByte = Low & (CM_BLOCKSIZE-1);
	fread(rbuf, rByte, sizeof(uchar), fm);
	camellia_encrypt_cbc_pad(rbuf, rbuf, iv, rByte, cmENkey);
	fwrite(rbuf, CM_BLOCKSIZE, sizeof(uchar), fc);

	//暗号文を消去
	memset(rbuf, 0, sizeof(rbuf));

	return 1;

}



/****************************DECRYPT*****************************/
void camellia_decrypt_cbc(uchar *m, uchar *c, uchar *iv, CAMELLIA_KEY *cmDEkey)
{
	unt *M = (unt *)m, *IV = (unt *)iv;

	camellia_decrypt(m, c, cmDEkey);
	M[0] ^= IV[0];
	M[1] ^= IV[1];
	M[2] ^= IV[2];
	M[3] ^= IV[3];
}

uchar *camellia_decrypt_cbc_block(uchar *m, uchar *c, uchar *iv, unt blocks, CAMELLIA_KEY *cmDEkey)
{
	unt i;
	for(i=blocks;i>0;i--){
		camellia_decrypt_cbc(m, c, iv, cmDEkey);
		iv = c;
		m += CM_BLOCKSIZE;
		c += CM_BLOCKSIZE;
	}

	return iv;
}
unt camellia_decrypt_cbc_pad(uchar *m, uchar *c, uchar *iv, CAMELLIA_KEY *cmDEkey)
{
	uchar mbuf[CM_BLOCKSIZE];
	int Byte;

	camellia_decrypt_cbc(mbuf, c, iv, cmDEkey);
	Byte = mbuf[CM_BLOCKSIZE-1]&0x0f;
	memcpy(m, mbuf, Byte);

	memset(mbuf, 0, CM_BLOCKSIZE);
	return Byte;
}

/*2005/03/30*/
unt camellia_decrypt_cbc_FileFile(FILE *fm, FILE *fc, uchar *iv, ulong Hi, ulong Low, CAMELLIA_KEY *cmDEkey)
{
	uchar rbuf[BUFFERSIZE+2*CM_BLOCKSIZE];
	int n;
	unt wByte;//writeByte
	register int i;
	ulong nblocks;

	//Low >= 16 * 2
	if(Low < CM_BLOCKSIZE){
		Hi--;
	}
	Low -= CM_BLOCKSIZE;

	//4GByteごとの処理
	while(Hi){
		//ここはあまり高速化には関係ないかも

		//2^15回ループ
		for(i=0;i<0x400000;i++){
			//ここのループが重要
			fread(rbuf+2*CM_BLOCKSIZE, BUFFERSIZE, sizeof(uchar), fc);
			iv = camellia_decrypt_cbc_block(rbuf, rbuf+2*CM_BLOCKSIZE, iv, BUFFERSIZE/CM_BLOCKSIZE, cmDEkey);
			fwrite(rbuf, BUFFERSIZE, sizeof(uchar), fm);
			memcpy(rbuf+CM_BLOCKSIZE, iv, CM_BLOCKSIZE);
			iv = rbuf+CM_BLOCKSIZE;
		}
		Hi--;
	}

	//4GByte未満の処理
	n = Low / BUFFERSIZE;
	//Low & (BUFFERSIZE-1) = Low % BUFFERSIZE
	Low &= (BUFFERSIZE-1);
	for(i=0;i<n;i++){
		//ここのループが重要
		//128KByteごとに一気に処理
		fread(rbuf+2*CM_BLOCKSIZE, BUFFERSIZE, sizeof(uchar), fc);
		iv = camellia_decrypt_cbc_block(rbuf, rbuf+2*CM_BLOCKSIZE, iv, BUFFERSIZE/CM_BLOCKSIZE, cmDEkey);
		fwrite(rbuf, BUFFERSIZE, sizeof(uchar), fm);
		memcpy(rbuf+CM_BLOCKSIZE, iv, CM_BLOCKSIZE);
		iv = rbuf+CM_BLOCKSIZE;
	}

	//128KByte未満の処理
	nblocks = (Low/CM_BLOCKSIZE);
	if(nblocks){
		fread(rbuf+2*CM_BLOCKSIZE, nblocks * CM_BLOCKSIZE, sizeof(uchar), fc);
		iv = camellia_decrypt_cbc_block(rbuf, rbuf+2*CM_BLOCKSIZE, iv, nblocks, cmDEkey);
		fwrite(rbuf, nblocks * CM_BLOCKSIZE, sizeof(uchar), fm);
		//ivの置き場所はrbufの先頭
		memcpy(rbuf+CM_BLOCKSIZE, iv, CM_BLOCKSIZE);
		iv = rbuf+CM_BLOCKSIZE;
	}

	//CM_BLOCKの端数処理
	//最終ブロックを読み込む
	fread(rbuf, CM_BLOCKSIZE, sizeof(uchar), fc);
	//書き出すバイト数を教えてもらう
	wByte = camellia_decrypt_cbc_pad(rbuf, rbuf, iv, cmDEkey);
	//書き込み書き込み
	fwrite(rbuf, wByte, sizeof(uchar), fm);

	//平分を消去
	memset(rbuf, 0, sizeof(rbuf));

	return 1;
}
/*2005/03/30*/
unt camellia_decrypt_cbc_DataData(uchar *m, uchar *c, uchar *iv, unt Byte, CAMELLIA_KEY *cmDEkey)
{
	unt i;
	unt blocks = Byte/CM_BLOCKSIZE;

	i=0;

	/*初期ベクトル分余分*/
	Byte -= CM_BLOCKSIZE;

	iv = camellia_decrypt_cbc_block(m, c, iv, blocks, cmDEkey);
	c = iv + CM_BLOCKSIZE;
	m += (blocks) * CM_BLOCKSIZE;
	i += (blocks) * CM_BLOCKSIZE;

	i += camellia_decrypt_cbc_pad(m, c, iv, cmDEkey);

	return i;
}
/**************************DECRYPT and ENCRYPT***************************/
void camellia_decenc_cbc(uchar *newc, uchar *newiv, CAMELLIA_KEY *cmNEWkey, 
						 uchar *oldc, uchar *oldiv, CAMELLIA_KEY *cmOLDkey)
{
	camellia_decrypt_cbc(newc, oldc, oldiv, cmOLDkey);
	camellia_encrypt_cbc(newc, newc, newiv, cmNEWkey);
}
uchar *camellia_decenc_cbc_block(uchar *newc, uchar *newiv, CAMELLIA_KEY *cmNEWkey, 
								 uchar *oldc, uchar *oldiv, CAMELLIA_KEY *cmOLDkey, 
								 unt blocks)
{
	unt i;
	for(i=blocks;i>0;i--){
		camellia_decenc_cbc(newc, newiv, cmNEWkey, 
							oldc, oldiv, cmOLDkey);
		newiv = newc;
		oldiv = oldc;
		newc += CM_BLOCKSIZE;
		oldc += CM_BLOCKSIZE;
	}

	return newiv;
}
unt camellia_decenc_cbc_pad(uchar *newc, uchar *newiv, CAMELLIA_KEY *cmNEWkey, 
                            uchar *oldc, uchar *oldiv, CAMELLIA_KEY *cmOLDkey)
{
	uchar foo[CM_BLOCKSIZE];
	unt Byte, pByte;

	//前回の詰め物をfoo[]にかき出す。
	camellia_decrypt_cbc(foo, oldc, oldiv, cmOLDkey);
	Byte = foo[CM_BLOCKSIZE-1] & 0x0f;
	pByte = CM_BLOCKSIZE - Byte;

	//新たに詰め物を用意
	camellia_padding(&foo[Byte], pByte);
	foo[CM_BLOCKSIZE-1] &= 0x0f;
	foo[CM_BLOCKSIZE-1] |= Byte;
	camellia_encrypt_cbc(newc, foo, newiv, cmNEWkey);

	return 1;
}

unt camellia_decenc_cbc_FileFile(FILE *newfc, uchar *newiv, CAMELLIA_KEY *cmNEWkey, FILE *oldfc, uchar *oldiv, CAMELLIA_KEY *cmOLDkey, ulong Hi, ulong Low)
{
	uchar rbuf[BUFFERSIZE+2*CM_BLOCKSIZE];
	register int i;

	if(Low < CM_BLOCKSIZE){
		Hi--;
	}
	Low -= CM_BLOCKSIZE;

	while(Hi){
		//2^15回ループ
		for(i=0;i<0x400000;i++){
			//ここのループが重要
			fread(rbuf+2*CM_BLOCKSIZE, BUFFERSIZE, sizeof(uchar), oldfc);
			newiv = camellia_decenc_cbc_block(rbuf, newiv, cmNEWkey, 
											  rbuf+2*CM_BLOCKSIZE, oldiv, cmOLDkey, 
											  BUFFERSIZE/CM_BLOCKSIZE);
			fwrite(rbuf, BUFFERSIZE, sizeof(uchar), newfc);
			memcpy(rbuf+CM_BLOCKSIZE, oldiv, CM_BLOCKSIZE);
			//ここはバグだろう。oldivの使い道がよくわからない。
			oldiv += BUFFERSIZE;
		}
		Hi--;
		
	}

}

//コメントアウトしているので関数の引数を更新すること。
/****************************base64&ss*************************/
int camellia_encrypt_cbc_ssBase64(char *base64, char *ss, CAMELLIA_KEY *cmENkey)
{
	uchar *mp, *cp;
	unt len;
	unt mBytes, cBytes;

	len = (unt)strlen(ss);
	
	mBytes = len*(unt)sizeof(char);
	cBytes = camellia_cBytes(mBytes);

	mp = (uchar *)malloc(mBytes+cBytes);
	if(mp == NULL){
		return 0;
	}
	cp = mp + mBytes;

	strtodata(mp, ss, len);
	
	//camellia_encrypt_cbc_DataData(cp, mp, mBytes, cmENkey);
	
	base64_encode(base64, cp, cBytes);

	mp = cp;
	for(len=0;len<cBytes+mBytes;len++){
		*mp = 0;
		mp++;
	}
	free(cp);

	cp = mp = NULL;

	return 1;
}

int camellia_decrypt_cbc_Base64ss(char *ss, char *base64, CAMELLIA_KEY *cmDEkey)
{
	uchar *mp, *cp;
	unt mBytes, cBytes;
	unt len;

	len = (unt)strlen(base64);

	cBytes = (len/4)*3;

	cp = (uchar *)malloc(sizeof(uchar)*cBytes+sizeof(uchar)*(cBytes));
	if(cp == NULL){
		return 0;
	}
	mp = cp + sizeof(uchar)*cBytes;

	cBytes = base64_decode(cp, base64);

	//mBytes = camellia_decrypt_cbc_DataData(mp, cp, cBytes, cmDEkey);

	datatostr(ss, mp, mBytes);

	ss[mBytes/2] = '\0';

	mp = cp;
	for(len=0;len<2*sizeof(uchar)*cBytes;len++){
		*mp= 0;
		mp++;
	}
	free(cp);

	mp = cp = NULL;

	return 1;
}






/****************************data&ss***************************/
int camellia_encrypt_cbc_DataBase64(char *base64, uchar *data, unt nBytes, CAMELLIA_KEY *cmENkey)
{
	uchar *cp;
	unt len;
	unt cBytes;

	cBytes = camellia_cBytes(nBytes);

	cp = (uchar *)malloc(cBytes*sizeof(uchar));
	if(cp == NULL){
		return 0;
	}
//	camellia_encrypt_cbc_DataData(cp, data, nBytes, cmENkey);
	
	base64_encode(base64, cp, cBytes);

	for(len=0;len<cBytes;len++){
		cp[len] = 0;
	}
	free(cp);

	cp = NULL;

	return 1;
}

unt camellia_decrypt_cbc_Base64Data(uchar *data, char *base64, CAMELLIA_KEY *cmDEkey)
{
	uchar *cp;
	unt mBytes, cBytes;
	unt len;

	len = (unt)strlen(base64);

	cBytes = (len/4)*3;

	cp = (uchar *)malloc(sizeof(uchar)*cBytes);
	if(cp == NULL){
		return 0;
	}

	cBytes = base64_decode(cp, base64);

	mBytes = camellia_decrypt_cbc_DataData(data, cp, cBytes, cmDEkey);

	for(len=0;len<sizeof(uchar)*cBytes;len++){
		cp[len] = 0;
	}
	free(cp);

	cp = NULL;

	cBytes = len = 0;

	return mBytes;
}
#endif

