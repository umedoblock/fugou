/* test_unit_par2_file.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../src/par2.h"

int main(int argc, char **argv)
{
	char filename[80], ss[80];
	int n_parity;
	int i;

	fprintf(stdout, "    argc = %d\n", argc);
	for(i=0;i<argc;i++){
		fprintf(stdout, "argv[%d] = %s\n", i, argv[i]);
	}

	if(	argc == 4 && strcmp(argv[1], "-e") == 0){
		n_parity = atoi(argv[3]);
	}else if(argc == 3 && strcmp(argv[1], "-d") == 0){
	}else{
		fprintf(stdout, "./par2_file -e filename n_parity\n");
		fprintf(stdout, "./par2_file -d filename\n");
		return -1;
	}
	strcpy(filename, argv[2]);

	if(strcmp(argv[1], "-e")==0){
		fprintf(stdout, "par2_encode_file(\".\", \"%s\", %d)\n",
				filename, n_parity);
		par2_encode_file(".", filename, n_parity);
	}else if(strcmp(argv[1], "-d")==0){
		fprintf(stdout, "par2_decode_file(\".\", \"%s\")\n", filename);
		par2_decode_file(".", filename);
	}else{
		fprintf(stdout, "\"%s\" は間違っています。", argv[1]);
	}

	return 0;
}

