#!/bin/bash

TMP_DIR=/tmp
FUGOU_DIR=${TMP_DIR}/fugou

rm -rf ${FUGOU_DIR}
mkdir -p ${FUGOU_DIR}/{reed_solomon,slot,ecc}
mkdir -p ${FUGOU_DIR}/ecc/ecc
cp ./{README,libfugou_base.h,libfugou.[ch],assert_xxx.[ch]} ${FUGOU_DIR}
cp ./reed_solomon/{Makefile,test_rs.c,rs.[ch],expected_vm_of_rsx.c,random_1048576.bin,galois.c} \
   ${FUGOU_DIR}/reed_solomon
cp ./slot/{Makefile,test_slot.c,slot.[ch],sample_slot_of_rs.c} \
   ${FUGOU_DIR}/slot
cp -r ./ecc/{ecdh.py,ecdsa.py,setup.py,test_ecc.py,test_speed.py} ${FUGOU_DIR}/ecc
cp -r ./ecc/ecc/{__init__.py,operands.py,test_speed_gcdext.py,_gcdext.c} ${FUGOU_DIR}/ecc/ecc

cd ${TMP_DIR}
tar czf ./fugou.tar.gz fugou
rm -f ~/fugou.tar.gz
cp ./fugou.tar.gz ~/fugou.tar.gz
