test_rs:
	rm -f ./core
	gcc -Wall -g -D__TEST__ reed_solomon/test_rs.c ./assert_xxx.c libfugou.c reed_solomon/rs.c -o reed_solomon/test_rs
	./reed_solomon/test_rs
gdb:
	gdb ./reed_solomon/test_rs ./core
