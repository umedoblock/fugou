# 参考
#
# 多段makeしよう
# http://d.hatena.ne.jp/tanakaBox/20070110/1168429231
# make = make --no-print-directory
# $(make) -C dirA
#
# GNU make 日本語訳
# http://www.ecoop.net/coop/translated/GNUMake3.77/make_toc.jp.html#TOC49
#
# make の再帰利用
# http://www.ecoop.net/coop/translated/GNUMake3.77/make_5.jp.html#SEC49

default : A B
	@echo this is top dir
A :
	$(MAKE) -C dirA
B :
	$(MAKE) -C dirB
