/* test_unit_camellia_sample.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int unt;
typedef unsigned char uchar;
typedef unsigned char Byte;
typedef unsigned long Word;

void Camellia_Ekeygen( const int, const Byte *, Byte * );
void Camellia_Encrypt( const int, const Byte *, const Byte *, Byte * );
void Camellia_Decrypt( const int, const Byte *, const Byte *, Byte * );

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
void str2data(uchar *data, char *str, int len)
{
	int i;
	unsigned int tmp;
	for(i=0;i<len/8;i++){
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
void test_2(void)
{
	uchar m[128/8], c[128/8], k[32], e[12800], d[128/8], ct[128/8];
	char ss[128], mss[80], css[80], ctss[80], dss[80];
	char kstr[128], mstr[128], cstr[128];
	char *keysize = "Camellia with ";
	FILE *fp = fopen("./t_camellia.txt", "r");
	int n = 0, good = 0, bad = 0;
	int keylen;

	if(fp == NULL){
		fprintf(stdout, "test_2() failed not found t_camellia.txt\n");
	}

	while(fgets(ss, 128-1, fp) != NULL){
ss[strlen(ss)-1] = '\0';
		if(strncmp(ss, keysize, strlen(keysize)-1) == 0){
			keylen = atoi(ss + strlen(keysize));
		}
		if(ss[0] == 'K'){
			str2data(k, ss + 11, keylen);
			Camellia_Ekeygen(keylen, k, e);
			strcpy(kstr, ss);
			n = 1;
		}
		if(ss[0] == 'P'){
			str2data(m, ss + 11, 128);
			strcpy(mstr, ss);
			pStr128(mss, m);
		}
		if(ss[0] == 'C' && ss[1] == ' '){
			str2data(ct, ss + 11, 128);
			strcpy(cstr, ss);
			pStr128(ctss, ct);
			Camellia_Encrypt(keylen, m, e, c);
			pStr128(css, c);
			Camellia_Decrypt(keylen, c, e, d);
			pStr128(dss, d);
			if(strcmp(mss, dss) != 0 || strcmp(css, ctss) != 0){
				fprintf(stdout, "--\n");
				fprintf(stdout, "No.%03d", n);
				fprintf(stdout, " bad\n");
				fprintf(stdout, "keysize = %d\n", keylen);
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

