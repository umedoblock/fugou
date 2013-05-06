#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long int ulonglong;

#define ULL2UCHARS(uc, ull) do { \
        (uc)[0] = (uchar )((ull) >> 56); \
        (uc)[1] = (uchar )((ull) >> 48); \
        (uc)[2] = (uchar )((ull) >> 40); \
        (uc)[3] = (uchar )((ull) >> 32); \
        (uc)[4] = (uchar )((ull) >> 24); \
        (uc)[5] = (uchar )((ull) >> 16); \
        (uc)[6] = (uchar )((ull) >> 8); \
        (uc)[7] = (uchar )((ull)); \
    } while(0)

int main(void)
{
    ulonglong n = 0x1023456789abcdef;
    ulonglong n2 = 0x20aa99cad3a49608;
    uchar data[8];
    int i;

    ULL2UCHARS(data, n);
    for(i=0;i<8;i++) {
        fprintf(stdout, "data[%d] = 0x%02x\n", i, data[i]);
    }
    fprintf(stdout, "\n");

    ULL2UCHARS(data, n2);
    for(i=0;i<8;i++) {
        fprintf(stdout, "data[%d] = 0x%02x\n", i, data[i]);
    }

    return 0;
}
