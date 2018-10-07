# make.sh
# Copyright (C) 2008 梅どぶろく umedoblock

#!/bin/bash

list=(DH camellia utbi_sanjutsu base64 par2 sha)

for l in ${list[@]} ; do
  echo ---------------------
  cd src/$l
  echo "$l=" $l
  echo pwd = $PWD
  make clean
  make
  make install
  echo
  cd ../../
done

