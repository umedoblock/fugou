gcc -Werror -Wall -g par2/libpar2_sample.c par2/libpar2.c par2/sha1.c \
	-o par2/libpar2_sample && \
h=`./par2/libpar2_sample --encode --redundancy=8 --bits=8 \
	--file=par2/par2_sample.dat` && \
echo $h && \
sha1sum h* > sha1sum.txt && \
echo ----------------------------------------- && \
cat $h && \
# rm -f h* \
echo success
