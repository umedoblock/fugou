#include <stdio.h>

#define doing(do_) (do_ ## er)

int main(void)
{
    int i = 0;
    int reader, writer;

    while (i < 10) {
        i++;
        if (i % 2 == 0)
            continue;
        fprintf(stdout, "i = %d\n", i);
    }

    fprintf(stdout, "reader = %d\n", reader);
    fprintf(stdout, "writer = %d\n", writer);
    fprintf(stdout, "\n");
    fprintf(stdout, "reader = %d\n", doing(read));
    fprintf(stdout, "writer = %d\n", doing(writ));
    fprintf(stdout, "\n");
    fprintf(stdout, "doing(read) = %d\n", doing(read));
    fprintf(stdout, "doing(writ) = %d\n", doing(writ));
    return 0;
}
