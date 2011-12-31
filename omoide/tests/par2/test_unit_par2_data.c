/* test_unit_par2_data.c
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

int par2_bufwrite(
	FILE **fo, int n_parity, int buflen,
	unsigned char *wbuf, unsigned char *data
)
{
	int i, woffset = buflen / n_parity;

	par2_tateyoko(wbuf, data, n_parity, buflen / n_parity);
	for(i=0;i<n_parity;i++){
		fwrite(wbuf+woffset*i, 1, woffset, fo[i]);
	}
	return 0;
}

void memrandom(unsigned char *buf, int datalen)
{
	int i;
	unsigned int seed = 0;

	for(i=0;i<datalen;i++){
		buf[i] = 0xffUL & rand_r(&seed);
	}
}

void test_merge(
	unsigned char *merge_data, unsigned char *pari_data,
	unsigned char *test_data, int datalen, int n_parity,
	unsigned char *flg)
{
	int i, j, k, n_block = datalen / n_parity;
	int count = 0;

	k = 0;
	for(i=0;i<2*n_parity;i++){
		if(bitcheck(flg, i)){
			count++;
			for(j=0;j<n_block;j++){
				merge_data[j*n_parity+i] =
					test_data[j*n_parity+i];
			}
		}else{
			for(;k<n_parity;k++){
				if(bitcheck(flg, n_parity + k)){
					count++;
					for(j=0;j<n_block;j++){
						merge_data[j*n_parity+i] =
							pari_data[j*n_parity+k];
					}
					k++;
					break;
				}
			}
		}
		if(count >= n_parity){
			break;
		}
	}
}


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

int main(int argc, char *argv[])
{
	unsigned char *test_data,*merge_data;
	unsigned char *pari_data, *dec_data;
	unsigned char flg[PAR2_PARITY_FLG_SIZE] = {0};
	char *b, ss[80];
	int datalen, n_parity, cmp, i, padlen, buflen;
	FILE *fp, **fo, *fw;

	if(argc < 3){
		fprintf(stderr, "test datalen, n_parity\n");
		exit(1);
	}

	datalen  = atoi(argv[1]);
	n_parity = atoi(argv[2]);
	padlen   = (n_parity - (datalen % n_parity)) % n_parity;
	datalen += padlen;

	fprintf(stderr, "test datalen = %d, n_parity = %d\n",
			datalen, n_parity);

	test_data = (unsigned char *)malloc(datalen);
	pari_data = (unsigned char *)malloc(datalen);
	merge_data = (unsigned char *)malloc(datalen);
	dec_data  = (unsigned char *)malloc(datalen);
	fo = (FILE **)malloc(sizeof(FILE *) * 2*n_parity);

	if(	test_data  == NULL || pari_data == NULL ||
		merge_data == NULL || dec_data == NULL){
		fprintf(stderr, "memory 不足\n");
		exit(1);
	}

	//memrandom(test_data, datalen);
	fp = fopen("rand_copy", "rb");
	fw = fopen("rand_copy.decode", "wb");
	for(i=0;i<2*n_parity;i++){
		sprintf(ss, "rand_copy.%d", i);
		fo[i] = fopen(ss, "wb");
		fprintf(stderr, "fopen(%s, \"wb\")\n", ss);
	}

	fread(test_data, 1, datalen, fp);
	memset(test_data + datalen - padlen, 0, padlen);
	fprintf(stdout, "初期値\n");
	//dump(test_data, datalen, n_parity);
	fprintf(stdout, "\n");

	par2_encode_data(pari_data, test_data, datalen, n_parity);
puts("koko0");
	par2_bufwrite(fo, n_parity, datalen, merge_data, test_data);
puts("koko1");
	par2_bufwrite(fo+n_parity, n_parity, datalen, merge_data, pari_data);
puts("koko2");

	fprintf(stdout, "parity\n");
	//dump(pari_data, datalen, n_parity);
	fprintf(stdout, "\n");

	memset(flg, 0, sizeof(flg));
	for(i=0;i<n_parity;i++){
		bitset(flg, n_parity+i);
		//bitset(flg, x[i]);
	}
//	for(i=0;i<2*n_parity;i++){
//		if((i%2)==1){
//			continue;
//		}
//		bitset(flg, i);
//	}
	fprintf(stdout, "flg = \n");
	for(i=0;i<(2*n_parity-1)/8+1;i++){
		fprintf(stdout, "%02x, ", flg[i]);
	}
	fprintf(stdout, "\n");

	fprintf(stdout, "集まったparity&data\n");
	test_merge(merge_data, pari_data, test_data,
			datalen, n_parity, flg);
	//dump(merge_data, datalen, n_parity);
	fprintf(stdout, "\n");

	par2_decode_data(dec_data, merge_data, datalen, n_parity, flg);

	fprintf(stdout, "復号後の値\n");
	//dump(dec_data, datalen, n_parity);
	fprintf(stdout, "\n");
	fwrite(dec_data, 1, datalen - padlen, fw);
	cmp = memcmp(test_data, dec_data, datalen-padlen);
	if(cmp == 0){
		b = "true";
	}else{
		b = "false";
	}
	fprintf(stdout, "元のデータと復号後の値の一致:\n%s\n", b);

	for(i=2*n_parity-1;i>=0;i--){
		fclose(fo[i]);
	}
	free(fo);
	fclose(fw);
	fclose(fp);
	free(dec_data);
	free(merge_data);
	free(pari_data);
	free(test_data);

	return 0;
}

