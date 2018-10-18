# fugou library
#
# Copyright (c) 2012-2018 梅濁酒(umedoblock)
#
# Thease softwares are released under the MIT License.
# https://github.com/umedoblock/fugou
fugou
|-- omoide, 純粋だった頃の思ひ出。
|   |       初めてのprogramming。
|   |       20歳の頃，cygwin, Visual Studio .net を使って
|   |       C 言語の勉強をしていました。
|   |-- src
|   |   |-- utbi_sanjutsu
|   |   |       多倍長整数ライブラリ, 1 file 1 function 主義
|   |   |-- DH
|   |   |   Diffee Hellman key exchange dependents on utbi_sanjutsu library.
|   |   |-- camellia
|   |   |   対称鍵 encrypt
|   |   |-- base64
|   |   |   base64 encode
|   |   |-- esign
|   |   |   電子署名 dependents on utbi_sanjutsu library.
|   |   |-- par2
|   |   |   par2 encode
|   |   `-- sha
|   |       SHA sum
|   |-- tests
|   |   |-- DH
|   |   |-- base64
|   |   |-- camellia
|   |   |-- par2
|   |   |-- sha
|   |   `-- utbi_sanjutsu
|   |-- tp
|   |   |-- DH
|   |   |-- base64
|   |   |-- camellia
|   |   |-- camellia_asm
|   |   |-- camellia_tmp
|   |   |-- esign
|   |   |-- randomfile
|   |   |-- sha
|   |   `-- utbi_seki_prof
|   `-- 梅暗号
        C=M*e1+r*e2 mod n, e1=e*a, e2=e*x
`-- src
    |-- libfugou
    |   slot and reed_solomon dependent this library.
    |-- slot
    |   efficientry use every binary function
    |   sample show you that how tu use slot with reed solomon.
    |-- reed_solomon
    |   reed solomon encode
    |-- ecc
    |   楕円曲線暗号 library
    |   You use a ecdh and ecdsa.
    |-- camellia
    |   対称鍵暗号
    |-- sha
    |   SHA functions
    |-- par2
    |   par2 encode
    |-- montgomery
    |   montgomery 乗算(has the bug !!!, Don't cry !!!)
    |-- oraga,      junk
    |-- bar_ext,    junk
    |-- foo,        junk
    |-- foo_ext,    junk
    |-- make_chain, junk
    `-- tsa, time stump authority, but empty !!!!!!!!!!!!!!!!!!!!!!!!
