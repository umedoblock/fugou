/* test_unit_base64.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../src/base64.h"

void dump(unsigned char *x, unt n)
{
	unt i;
	for(i=0;i<n;i++){
		fprintf(stdout, "%02x", x[i]);
	}
	fprintf(stdout, "\n");
}

#define BUFFER_SIZE 2048

int main(void)
{
	char enco[(BUFFER_SIZE * 14) / 10];
	unsigned char data[BUFFER_SIZE], deco[BUFFER_SIZE];
	unt n = BUFFER_SIZE, ndeco, nenco, i, m;
	int fd = open("/dev/urandom", O_RDONLY), cmp;
	int pass = 0, fail = 0;

	if(fd < 0){
		puts("random error");
		exit(1);
	}

	m = 512;

	for(i=0;i<BUFFER_SIZE - m;i++){
		n = m + i;
		read(fd, data, n);
		nenco = base64_encode(enco, data, n);
		ndeco = base64_decode(deco, enco);

		cmp = memcmp(deco, data, n);
		if(cmp != 0 || n != ndeco || strlen(enco) != nenco){
			puts("--------");
			fail++;
			fprintf(stdout, "base64 error\n");
			fprintf(stdout, "(cmp, n, ndeco) = (%d, %d, %d)\n", cmp, n, ndeco);
			fprintf(stdout, "enco = %s\n", enco);
			dump(data, n);
			dump(deco, n);
		}else{
			pass++;
			//fprintf(stdout, "(cmp, n, ndeco) = (%d, %d, %d)\n", cmp, n, ndeco);
			//fprintf(stdout, "base64 good\n");
		}
	}

	fprintf(stdout, "(pass, fail) = (%d, %d)\n", pass, fail);

	return 0;
}

