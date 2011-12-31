/* test_sha.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/sha.h"

int main(void)
{
  unt h[32];
  SHA512 h512;
  memset((void *)h, 0, 32*sizeof(unt));

///////////////////////////////////////////////////////////////
  fprintf(stdout, "sha-1\n");

  sha1_file(h, "abc.txt");
  fprintf(stdout, "abc.txt    "); sha_puthash(h, 160); puts("");

  sha1_file(h, "abc56.txt");
  fprintf(stdout, "abc56.txt  "); sha_puthash(h, 160);puts(""); 

  sha1_file(h, "a100.txt");
  fprintf(stdout, "a100.txt   "); sha_puthash(h, 160);puts(""); 
  fprintf(stdout, "\n");
///////////////////////////////////////////////////////////////
  fprintf(stdout, "sha-256\n");

  sha256_file(h, "abc.txt");
  fprintf(stdout, "abc.txt    ");sha_puthash(h, 256);puts("");

  sha256_file(h, "abc56.txt");
  fprintf(stdout, "abc56.txt  ");sha_puthash(h, 256);puts("");

  sha256_file(h, "a100.txt");
  fprintf(stdout, "a100.txt   ");sha_puthash(h, 256);puts("");
  fprintf(stdout, "\n");
////////////////////////////////////////////////////////////////
  fprintf(stdout, "sha-512\n");

  sha512_FileName(&h512, "abc.txt");
  fprintf(stdout, "abc.txt    ");sha512_puthash(h512);puts("");

  sha512_FileName(&h512, "abc112.txt");
  fprintf(stdout, "abc112.txt ");sha512_puthash(h512);puts("");

  sha512_FileName(&h512, "a100.txt");
  fprintf(stdout, "a100.txt   ");sha512_puthash(h512);puts("");
  fprintf(stdout, "\n");
/////////////////////////////////////////////////////////////////
  fprintf(stdout, "sha-384\n");

  sha384_file_name(&h512, "abc.txt");
  fprintf(stdout, "abc.txt    ");sha_puthash(h512.hash[0], 384);puts("");

  sha384_file_name(&h512, "abc112.txt");
  fprintf(stdout, "abc112.txt ");sha_puthash(h512.hash[0], 384);puts("");

  sha384_file_name(&h512, "a100.txt");
  fprintf(stdout, "a100.txt   ");sha_puthash(h512.hash[0], 384);puts("");
  fprintf(stdout, "\n");
/////////////////////////////////////////////////////////////

  return 0;
}

