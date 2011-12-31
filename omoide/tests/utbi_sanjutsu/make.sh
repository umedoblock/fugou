# make.sh
# Copyright (C) 2008 梅どぶろく umedoblock

gcc     test_utbi_sanjutsu.c -o test_utbi_sanjutsu    -lutbi_sanjutsu
gcc -pg test_utbi_sanjutsu.c -o test_utbi_sanjutsu_pg -lutbi_sanjutsu
