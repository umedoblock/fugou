#include <stdio.h>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#define SWAP_USHORT(x) (((x & 0xff) << 8) + (x >> 8))

uchar _num[2] = {0x01, 0x02};
ushort _num2 = 0x0102;

/*
http://www.math.kobe-u.ac.jp/~kodama/tips-C-endian.html
*/
#include <endian.h>

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
/*
gcc -D__BIG_ENDIAN__
*/
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define __LITTLE_ENDIAN__
        int __endian = 0;
    #elif __BYTE_ORDER == __BIG_ENDIAN
        #define __BIG_ENDIAN__
        int __endian = 1;
    #endif

#endif

unsigned char *p = NULL;

int main(void)
{
    unsigned char buf[80], *ptr = NULL;
    /*
    #ifdef BIG_ENDIAN
        fprintf(stdout, "BIG_ENDIAN\n");
    #elif defined LITTLE_ENDIAN
        fprintf(stdout, "LITTLE_ENDIAN\n");
    #else
        #error no defined BIG_ENDIAN and LITTLE_ENDIAN
    #endif
    */
    fprintf(stdout, "endian  = %u\n", __endian);
    fprintf(stdout, "\n");

    fprintf(stdout, "buf     = %u\n", (uint )buf);
    fprintf(stdout, "buf + 1 = %u\n", (uint )buf + 1);
    fprintf(stdout, "buf < buf + 1 = %u\n", (uint )buf < (uint )buf + 1);
    fprintf(stdout, "\n");
    fprintf(stdout, "ptr     = %u\n", (uint )ptr);
    fprintf(stdout, "ptr + 1 = %u\n", (uint )ptr + 1);
    fprintf(stdout, "ptr < ptr + 1 = %u\n", (uint )ptr < (uint )ptr + 1);

    return 0;
}
