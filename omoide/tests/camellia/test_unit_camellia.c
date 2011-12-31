/* test_unit_camellia.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../src/camellia.h"
char *str2unt(unt *x, char *str);
void pStr128(char *ss, uchar *d)
{
	int i;
	strcpy(ss, "");
	for(i=0;i<128/8;i++){
		sprintf(ss + strlen(ss), "%02x", d[i]);
		if(i+1 == 8){
			sprintf(ss + strlen(ss), " ");
		}
	}
}
char *str2unt(unt *x, char *str)
{
	int i;
	unsigned int tmp, foo = 0;

	for(i=0;i<4;i++){
		str[2] = '\0';
		sscanf(str, "%x", &tmp);
		str[2] = ' ';
		foo <<= 8;
		foo += tmp;
		str += 3;
	}

	*x = foo;

	return str;
}
void str2key(CAMELLIA_KEY *k, char *str)
{
	int i, key32 = k->keysize/32;
	unt tmp;

//	fprintf(stdout, "\nkey = %s", str);
	for(i=0;i<key32;i++){
		str = str2unt(&tmp, str);
		k->key[(key32-i-1)/2][1-(i%2)] = tmp;
	}
}
void str2data(uchar *data, char *str)
{
	int i;
	unsigned int tmp;
	for(i=0;i<16;i++){
		str[2] = '\0';
		sscanf(str, "%x", &tmp);
		str[2] = ' ';
		data[i] = tmp;
		str += 3;
	}
}
void pinit(uchar *d)
{
	int i;
	for(i=0;i<8;i++){
		d[i] = i * 0x22 + 0x01;
	}
	for(;i<16;i++){
		d[i] = (8-i)*0x22 + 0xfe;
	}
}
void key128(CAMELLIA_KEY *cmk)
{
	cmk->key[1][1] = 0x01234567;
	cmk->key[1][0] = 0x89abcdef;
	cmk->key[0][1] = 0xfedcba98;
	cmk->key[0][0] = 0x76543210;
	cmk->keysize   = 128;
}
void key192(CAMELLIA_KEY *cmk)
{
	cmk->key[2][1] = 0x01234567;
	cmk->key[2][0] = 0x89abcdef;
	cmk->key[1][1] = 0xfedcba98;
	cmk->key[1][0] = 0x76543210;
	cmk->key[0][1] = 0x00112233;
	cmk->key[0][0] = 0x44556677;
	cmk->keysize   = 192;
}
void key256(CAMELLIA_KEY *cmk)
{
	cmk->key[3][1] = 0x01234567;
	cmk->key[3][0] = 0x89abcdef;
	cmk->key[2][1] = 0xfedcba98;
	cmk->key[2][0] = 0x76543210;
	cmk->key[1][1] = 0x00112233;
	cmk->key[1][0] = 0x44556677;
	cmk->key[0][1] = 0x8899aabb;
	cmk->key[0][0] = 0xccddeeff;
	cmk->keysize   = 256;
}
void keyset(CAMELLIA_KEY *cmk)
{
	int ks = cmk->keysize;
	if(ks == 128){
		key128(cmk);
	}else if(ks == 192){
		key192(cmk);
	}else if(ks == 256){
		key256(cmk);
	}else{
		fprintf(stdout, "keysize error\n");
		exit(1);
	}
}
void test_1(void)
{
	CAMELLIA_KEY cmk;
	uchar m[128/8], c[128/8], d[128/8];
	int keysize[3] = {128, 192, 256};
	char *ct[] = {
		"6767313854966973 0857065648eabe43",
		"b4993401b3e996f8 4ee5cee7d79b09b9",
		"9acc237dff16d76c 20ef7c919e3a7509"
	};
	char mss[80], css[80], dss[80];
	int i;

	pinit(m);

	for(i=0;i<3;i++){
		fprintf(stdout, "--\n");
		cmk.keysize = keysize[i];
		keyset(&cmk);
		camellia_keygen(&cmk);
		camellia_encrypt(c, m, &cmk);
		pStr128(mss, m);
		fprintf(stdout, "msg = %s\n", mss);

		pStr128(css, c);
		fprintf(stdout, "cry = %s\n", css);
		fprintf(stdout, " ct = %s\n", ct[i]);

		camellia_decrypt(d, c, &cmk);
		pStr128(dss, d);
		fprintf(stdout, "dec = %s\n", dss);

		if(strcmp(css, ct[i])==0){
			fprintf(stdout, "encrypt good\n");
		}else{
			fprintf(stdout, "encrypt bad\n");
		}
		if(strcmp(dss, mss)==0){
			fprintf(stdout, "decrypt good\n");
		}else{
			fprintf(stdout, "decrypt bad\n");
		}
	}
}
void test_2(void)
{
	CAMELLIA_KEY cmk;
	uchar m[128/8], c[128/8], d[128/8], ct[128/8];
	char ss[128], mss[80], css[80], ctss[80], dss[80];
	char kstr[128], mstr[128], cstr[128];
	char *keysize = "Camellia with ";
	FILE *fp = fopen("./t_camellia.txt", "r");
	int i, n = 0, good = 0, bad = 0;

	if(fp == NULL){
		fprintf(stdout, "test_2() failed not found t_camellia.txt\n");
	}

	while(fgets(ss, 128-1, fp) != NULL){
ss[strlen(ss)-1] = '\0';
		if(strncmp(ss, keysize, strlen(keysize)-1) == 0){
			cmk.keysize = atoi(ss + strlen(keysize));
		}
		if(ss[0] == 'K'){
			str2key(&cmk, ss + 11);
			strcpy(kstr, ss);
			camellia_keygen(&cmk);
			n = 1;
		}
		if(ss[0] == 'P'){
			strcpy(mstr, ss);
			str2data(m, ss + 11);
			pStr128(mss, m);
		}
		if(ss[0] == 'C' && ss[1] == ' '){
			strcpy(cstr, ss);
			str2data(ct, ss + 11);
			pStr128(ctss, ct);
			camellia_encrypt(c, m, &cmk);
			pStr128(css, c);
			camellia_decrypt(d, c, &cmk);
			pStr128(dss, d);
			if(strcmp(mss, dss) != 0 || strcmp(css, ctss) != 0){
				fprintf(stdout, "--\n");
				fprintf(stdout, "No.%03d", n);
				fprintf(stdout, " bad\n");
				fprintf(stdout, "keysize = %d\n", cmk.keysize);
				fprintf(stdout, " m = %s\n d = %s\n", mss, dss);
				fprintf(stdout, " c = %s\nct = %s\n", css, ctss);
				fprintf(stdout, "%s\n", kstr);
				fprintf(stdout, "%s\n", mstr);
				fprintf(stdout, "%s\n", cstr);
				bad++;
			}else{
//				fprintf(stdout, " good\n");
				good++;
			}
			n++;
		}
	}
	fprintf(stdout, "good = %d, bad = %d\n", good, bad);
}
int main(void)
{
	//test_1();
	test_2();
	return 0;
}

