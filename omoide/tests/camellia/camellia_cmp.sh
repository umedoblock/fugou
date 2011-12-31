# camellia_cmp.sh
# Copyright (C) 2008 梅どぶろく umedoblock

gcc test_unit_camellia.c -o test_unit_camellia -lutbi_camellia
gcc test_unit_camellia_sample.c camellia_sample.c -o test_unit_camellia_sample
./test_unit_camellia		> cmp0.txt
./test_unit_camellia_sample	> cmp1.txt
md5sum cmp0.txt cmp1.txt
diff -u cmp0.txt cmp1.txt

