#include <stdio.h>

/* $ gcc -Werror -Wall -W mem.c -o mem && ./mem */
int main(void)
{
    char *mem = NULL, *mem_ = NULL;
    unsigned int ui;
    size_t allocate_size;
    /*
    char *mem = NULL, *mem_ = NULL, ss[80] = {0};
    size_t allocate_size = 0;
    */

    mem = (char *)0;
    mem_ = (char *)1;
    ui = mem - mem_;
    allocate_size = ui;
    fprintf(stdout, "ui = %u\n", ui);
    fprintf(stdout, "allocate_size = %u\n", allocate_size);
    fprintf(stdout, "ui == allocate_size = %d\n", ui == allocate_size);
    fprintf(stderr, "allocate_size = %d, mem - mem_ = %d\n", \
                     allocate_size, (mem - mem_));
    fprintf(stderr, "allocate_size = %u, mem - mem_ = %u\n", \
                     allocate_size, (mem - mem_));
    fprintf(stderr, "(int )allocate_size == (mem - mem_) is %d\n", \
                     (int )allocate_size == (mem - mem_));
    fprintf(stderr, "allocate_size == (unsigned int )(mem - mem_) is %d\n", \
                     allocate_size == (unsigned int )(mem - mem_));
    fprintf(stderr, "allocate_size == (size_t )(mem - mem_) is %d\n", \
                     allocate_size == (size_t )(mem - mem_));

    return 0;
}
