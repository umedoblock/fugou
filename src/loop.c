#include <stdio.h>

int main(void)
{
    int i = 0;

    while (i < 10) {
        i++;
        if (i % 2 == 0)
            continue;
        fprintf(stdout, "i = %d\n", i);
    }
    fprintf(stdout, "\n");

    for (i=0;i<10;i++) {
        fprintf(stdout, "i = %d\n", i);
        continue; /* 無限 loop ならず */
    }

    for (i=0;i<10;i++) {
        fprintf(stdout, "i = %d\n", i);
        i--;
        /* 小細工すると、当然ながら無限 loop */
    }

    return 0;
}
