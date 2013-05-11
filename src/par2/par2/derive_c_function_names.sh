#!/bin/sh

# http://d.hatena.ne.jp/eel3/20120116/1326675419

# 解説通りでは使えなかった。
# ctags --sort=no --fields=K -f - $* | awk '
# BEGIN {
#     FS = "\t"
# }
#
# $NF == "function" {
#     print $1
# }'

# どうも、最後から何列目、という指定では無理臭い。
# $(NF) == "f" という指定でも駄目だった。
# NAME=fugou; ctags -R -f - ~/repos/hg/${NAME}/src/par2/par2/libpar2.c --sort=no --fields=K $* | awk '
# BEGIN {
#     FS = "\t"
# }
#
# $(NF - 1) == "f" {
#     print $1
# }'

# そもそも、どんな形式で出力されるか知らないので、調べてみた。
# _par2_read_header_of_hash       /home/umetaro/repos/hg/fugou/src/par2/par2/libpar2.c    /^static int _par2_read_header_of_hash($/;"     f       file:
# par2_big_bang   /home/umetaro/repos/hg/fugou/src/par2/par2/libpar2.c    /^int par2_big_bang(void)$/;"   f

# static 宣言があると最後に "file:" という文字列が追加されてしまっている。
# これでは、外部に公開する関数、内部のみで使用する関数、両方を抜き出す事が出来ない。

# 以下のように、先頭から4列目の文字列が、"f"のものを抜き出すようにした。
NAME=fugou; ctags -R -f - ~/repos/hg/${NAME}/src/par2/par2/libpar2.c --sort=no --fields=K $* | awk '
BEGIN {
    FS = "\t"
}

$4 == "f" {
    print $1
}'

# おまけ。
# $ ctags --list-kinds=C
# c  classes
# d  macro definitions
# e  enumerators (values inside an enumeration)
# f  function definitions
# g  enumeration names
# l  local variables [off]
# m  class, struct, and union members
# n  namespaces
# p  function prototypes [off]
# s  structure names
# t  typedefs
# u  union names
# v  variable definitions
# x  external and forward variable declarations [off]
