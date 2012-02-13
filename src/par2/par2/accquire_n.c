/*
def mul(x, y, mod):
    return x * y % mod

lim = 65536
for mod in range(lim + 1, lim * 2):
    appreciate = True
    for j in range(1, lim):
        s = set()
        for i in range(1, lim):
            m = mul(i, j, mod)
            # print('i =', i, m)
            if m in s:
                appreciate = False
                break
            s.add(m)

    if appreciate:
        print('appreciate modulus', mod)
        print('appreciate modulus', mod, file=sys.stderr)
    else:
        # print('not appreciate modulus', mod)
        pass
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int unt;
unt mul(unt x, unt y, unt mod)
{
    return x * y % mod;
}

int main(int argc, char *argv[])
{
    int lim, mod, i, j, appreciate;
    int *s = NULL;
    unt m;

    fprintf(stderr, "argc = %d\n", argc);
    for(i=0;i<argc;i++){
        fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
    }
    if(argc == 1){
        lim = 256;
    }else{
        lim = atoi(argv[1]);
    }
    // for mod in range(lim + 1, lim * 2):
    for(mod=lim+1; mod < lim * 2; mod++){
        appreciate = 1;
        for(j=1; j<lim;j++){
            s = (int *)malloc(sizeof(int) * lim * 2);
            memset(s, 0, sizeof(int) * lim * 2);
            for(i=1;i<lim;i++){
                m = mul(i, j, mod);
                if(s[m]){
                    appreciate = 0;
                    break;
                }
                s[m] = 1;
            }
            free(s);
        }

        if(appreciate){
            fprintf(stderr, "appreciate modulus %d\n", mod);
        }
        else{
            //printf("not appreciate modulus %d\n", mod);
        }
    }
    return 0;
}
