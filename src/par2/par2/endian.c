#include <stdio.h>

typedef unsigned int uint;

unsigned char *p = NULL;
#if p < p + 1
    #define BIG_ENDIAN
#else
    #define LITTLE_ENDIAN
#endif

int main(void)
{
    unsigned char buf[80], *ptr = NULL;
    #ifdef BIG_ENDIAN
        fprintf(stdout, "BIG_ENDIAN\n");
    #elif defined LITTLE_ENDIAN
        fprintf(stdout, "LITTLE_ENDIAN\n");
    #else
        #error no defined BIG_ENDIAN and LITTLE_ENDIAN
    #endif

    fprintf(stdout, "buf     = %u\n", (uint )buf);
    fprintf(stdout, "buf + 1 = %u\n", (uint )buf + 1);
    fprintf(stdout, "buf < buf + 1 = %u\n", (uint )buf < (uint )buf + 1);
    fprintf(stdout, "\n");
    fprintf(stdout, "ptr     = %u\n", (uint )ptr);
    fprintf(stdout, "ptr + 1 = %u\n", (uint )ptr + 1);
    fprintf(stdout, "ptr < ptr + 1 = %u\n", (uint )ptr < (uint )ptr + 1);

    return 0;
}
