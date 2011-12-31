# test.sh
# Copyright (C) 2008 梅どぶろく umedoblock

gcc test_unit_DH_simple.c -o test_unit_DH_simple -lutbi_sanjutsu
./test_unit_DH_simple 1024 53
