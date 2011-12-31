/* test_unit_par2.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../src/par2.h"

#define N_PARITY 255
#define MEGABYTE (1024 * 1024)
#define N_MB (1 * MEGABYTE)
//#define N_MB (1 * 64)

extern unsigned char par2_add_8[256][256];
extern unsigned char par2_mul_8[256][256];
extern unsigned char par2_div_8[256][256];
extern unsigned char par2_f_8[256][256];

typedef struct {
	unsigned char **pinv;
	unsigned char **px;
	unsigned char *inv;
	unsigned char *x;
	unsigned char *xtmp;
} tagpar2_decode;

int par2_encode_block(
	unsigned char *parity, unsigned char *data, int n_parity);
tagpar2_decode *par2_decode_block_malloc(int n_parity);

void dump(unsigned char *buf, int buflen, int width)
{
	int i;
	for(i=0;i<buflen;i++){
		fprintf(stdout, "%3d, ", buf[i]);
		if((i+1)%width==0){
			fprintf(stdout, "\n");
		}
	}
}

int main(int argc, char *argv[])
{
	unsigned char test_0[4] = {10, 200, 55, 11};
	unsigned char pari_0[4], buf_0[4];
	unsigned char flg[(256*2-1)/8+1] = {0};
	tagpar2_decode *dec;
	char *b;
	int n_parity = 4, cmp;

	fprintf(stdout, "初期値\n");
	dump(test_0, n_parity, n_parity);
	fprintf(stdout, "\n");

	par2_encode_block(pari_0, test_0, n_parity);
	fprintf(stdout, "parity\n");
	dump(pari_0, n_parity, n_parity);
	fprintf(stdout, "\n");

	pari_0[0] = pari_0[1];
	pari_0[1] = test_0[1];
	fprintf(stdout, "集まったparity&data\n");
	dump(pari_0, n_parity, n_parity);
	fprintf(stdout, "\n");

	bitset(flg, 1);
	bitset(flg, 5);
	bitset(flg, 6);
	bitset(flg, 7);
	fprintf(stdout, "flg = 0x%02x\n", flg[0]);
	fprintf(stdout, "\n");

	dec = par2_decode_block_malloc(n_parity);
	par2_decode_parity(flg, dec, n_parity);
	par2_decode_block(buf_0, pari_0, n_parity, dec);
	par2_decode_block_free(dec);

	fprintf(stdout, "復号後の値\n");
	dump(buf_0, n_parity, n_parity);
	fprintf(stdout, "\n");

	cmp = memcmp(test_0, buf_0, 4);
	if(cmp == 0){
		b = "true";
	}else{
		b = "false";
	}
	fprintf(stdout, "元のデータと復号後の値の一致:\n%s\n", b);


	return 0;
}

