# gcc -Wall -Werror par2/libpar2_sample.c par2/libpar2.c par2/sha1.c -o par2/libpar2_sample 2>&1 | tee /tmp/m
gcc -Wall -Werror \
    par2/libpar2_sample.c par2/libpar2.c par2/sha1.c \
    -o par2/libpar2_sample > /tmp/m 2>&1 && \
./par2/libpar2_sample --decode \
    --header=h8921ea1ae14f3c87964eb041265726ed637ba51e
