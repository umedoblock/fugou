/* test_decode_parity.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../src/par2.h"

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

int x[] = {
	1, 2, 4, 7, 8, 11, 13, 14,
	16, 17, 19, 22, 23, 26, 28, 29, 31,
	32, 34, 37, 38, 41, 43, 44, 46, 47,
	49, 52, 53, 56, 58, 59, 61, 62, 64,
	67, 68, 71, 73, 74, 76, 77, 79, 82,
	83, 86, 88, 89, 91, 92, 94, 97, 98,
	101, 103, 104, 106, 107, 109, 112, 113, 116,
	118, 119, 121, 122, 124, 127, 128, 131, 133,
	134, 136, 137, 139, 142, 143, 146, 148, 149,
	151, 152, 154, 157, 158, 161, 163, 164, 166,
	167, 169, 172, 173, 176, 178, 179, 181, 182,
	184, 187, 188, 191, 193, 194, 196, 197, 199,
	202, 203, 206, 208, 209, 211, 212, 214, 217,
	218, 221, 223, 224, 226, 227, 229, 232, 233,
	236, 238, 239, 241, 242, 244, 247, 248, 251,
	253, 254, 256, 257, 259, 262, 263, 266, 268,
	269, 271, 272, -1
};

tagpar2_decode *par2_decode_block_malloc(int n_parity);

int main(int argc, char *argv[])
{
	int i, n_parity = 108;
	unsigned char flg[PAR2_PARITY_FLG_SIZE] = {0};
	tagpar2_decode *dec;

		fprintf(stdout, "argc = %d\n", argc);
	if(argc < 2){
		fprintf(stdout, "n_parity");
		return -1;
	}
	n_parity = atoi(argv[1]);

	memset(flg, 0, sizeof(flg));
	for(i=0;i<n_parity;i++){
		bitset(flg, x[i]);
	}
fprintf(stdout, "n_parity = %d\n", n_parity);
	dec = par2_decode_block_malloc(n_parity);
	par2_decode_parity(flg, dec, n_parity);
	par2_decode_block_free(dec);

	fprintf(stdout, "test_decode_parity finish.\n");
	return 0;
}

