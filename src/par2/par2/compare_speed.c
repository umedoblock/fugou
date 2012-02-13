#include <stdio.h>

#define N 16

int main(void)
{
#if 0
    int i, j, n;
    int times = 100000;
    int total = 0;

    for(times=0;times<1000000;times++){
        for(j=0;j<N;j++){
            for(i=0;i<N;i++){
                n = i * j;
                n &= (N - 1);
                total += n;
            }
        }
    }
    printf("total = %d\n", total);
#else
    int i, j, n;
    int times = 100000;
    int gf[N], gfi[N];
    int total = 0;

    for(times=0;times<1000000;times++){
        for(j=0;j<N;j++){
            for(i=0;i<N;i++){
                n = gf[i] + gf[j];
                total += n;
            }
        }
    }
    printf("total = %d\n", total);
#endif

    return 0;
}
