#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int unt;
void view_hash(uchar *hash, int len_hash);
void sha1(uchar *hash, uchar *data, FILE *fp, int file_size);

/*
	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
*/
/*
openssl dgst -sha1 abc.txt
SHA1(abc.txt)=   a9993e364706816aba3e25717850c26c9cd0d89d
SHA1(abc56.txt)= 84983e441c3bd26ebaae4aa1f95129e5e54670f1
a100.txt   34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f
"a" * 1000000

楽しおってけしからん！
import hashlib
m = hashlib.sha1()
m.update(b'a' * 1000000)
h = m.hexdigest()
print(h)
34aa973cd4c4daa4f61eeb2bdbad27316534016f
*/

int main(int argc, char *argv[])
{
	uchar hash[160/8];
	FILE *fp = NULL;

	if (argc != 2) {
		fprintf(stderr, "usage: %s file_path\n", argv[0]);
		return -1;
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "cannot open \"%s\"\n", argv[1]);
		return -2;
	}

	sha1(hash, NULL, fp, 0);
	fclose(fp);

	fprintf(stdout, "sha1(%s) ", argv[1]);
	view_hash(hash, 160);

	return 0;
}
