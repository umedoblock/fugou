/* test_2_sha.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/sha.h"

int main(void)
{
	char *hash[][3] = {
		{"ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
		 "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909",
		 "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b"},

		{"cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7",
		 "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039",
		 "9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985"}
	};
	char *testdata[3] = {
		"abc", "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", NULL
	};
	int shatype[2] = {512, 384};
	unt len;
	int i, j, pass = 0, fail = 0;
	char ss[512/4+1];

	testdata[2] = (char *)malloc(100 * 10000 + 1);
	if(testdata[2] == NULL){
		fprintf(stdout, "malloc error\n");
	}
	memset(testdata[2], 'a', 100 * 10000);
	testdata[2][100 * 10000] = '\0';

	for(j=0;j<2;j++){
		for(i=0;i<3;i++){
			len = (unt )strlen(testdata[i]);
			sprintfsha(ss, testdata[i], len, shatype[j]);
			if(strcmp(hash[j][i], ss) != 0){
				fprintf(stdout, "error sha%d testdata[%d]\n", shatype[j], i);
				fail++;
			}else{
				pass++;
			}
		}
	}
	fprintf(stdout, "pass, fail = %d, %d\n", pass, fail);

	free(testdata[2]);

	return 0;
}

