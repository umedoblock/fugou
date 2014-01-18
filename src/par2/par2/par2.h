/* par2.h
 * Copyright (C) 2008 梅濁酒 umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PAR2_BIT 8
#define PAR2_PRIME_POLY 285
#define PAR2_COUNT (1 << PAR2_BIT)
#define PAR2_MAX (PAR2_COUNT - 1)

#define BUFFER_SIZE (1024 * 1024)

#define PAR2_MAX_N_PARITY 256
#define PAR2_PARITY_FLG_SIZE ((((2*PAR2_MAX_N_PARITY)-1)/8)+1)
#define PATH_SIZE (256 - 1)
#define FILENAME_SIZE PATH_SIZE

int par2_encode_data(
	unsigned char *parity, unsigned char *data,
	int datalen, int n_parity
);

int par2_decode_data(
	unsigned char *data, unsigned char *parity,
	int datalen, int n_parity, unsigned char *flg
);

int par2_encode_file(char *filepath, char *filename, int n_parity);
int par2_decode_file(char *filepath, char *filename);

void bitset(unsigned char *bit, int no);
void bitclear(unsigned char *bit, int no);
int bitcheck(unsigned char *bit, int no);

typedef struct {
	int buflen;
	int roffset;
	int n_parity;
	unsigned char flg[PAR2_PARITY_FLG_SIZE];
	FILE *fi[PAR2_MAX_N_PARITY];
	FILE *fo;
	FILE *fparity;
	unsigned char data[BUFFER_SIZE];
	unsigned char parity[BUFFER_SIZE];
	unsigned char rbuf[BUFFER_SIZE];
	size_t high;
	size_t low;
} tagpar2_decode_file;

#define PAR2_ERROR_PATH_LENGTH -1
#define PAR2_ERROR_PARITY_FILE_GA_MITSUKARIMASEN -2
#define PAR2_ERROR_NOT_FOUND_PARITY_FILE -2
#define PAR2_ERROR_KAKIKOMI_DEKIMASEN_DECODE_FILE -3
#define PAR2_ERROR_NOT_WRITE_DECODE_FILE -3
#define PAR2_ERROR_DECODE_FILE_MEMORY -4
#define PAR2_ERROR_HITUYOUNA_PARITY_TARINAI -5
