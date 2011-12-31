# sha.sh
# Copyright (C) 2008 梅どぶろく umedoblock

gcc test_sha.c -o test_sha -lutbi_sha
./test_sha > test_sha.txt
md5sum ./test_sha.txt checksum.txt
diff -u ./test_sha.txt checksum.txt
gcc test_2_sha.c -o test_2_sha -lutbi_sha
./test_2_sha
