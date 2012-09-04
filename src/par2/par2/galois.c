/* Copyright 2011 梅どぶろく(umedoblock) */
/*
import math
import sys
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
_calc_log10(int n)
{
    int digits = 0;

    while (n > 0){
        n /= 10;
        digits++;
    }

    return digits;
}

void p_bit(FILE *fp, int n, int length)
{
    int i;

    for(i=0;i<length;i++){
        fprintf(fp, "%c", '0' + ((n >> (length - i - 1)) & 1));
        /*
        fprintf(fp, "n = %d, (length - 1) = %d, ((n >> (length - i)) = %d\n", \
                     n, length - 1, n >> (length - i));
        */
    }
}

int main(int argc, char *argv[])
{
    int i, w, n_bits, n, len;
    int poly, field_max, digits;
    int bit_pattern, galois_field, *s;
    size_t mem_size = 0;

    fprintf(stderr, "argc = %d\n", argc);
    for(i=0;i<argc;i++){
        fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
    }
    fprintf(stderr, "\n");

    if(argc == 1){
        n_bits = 4;
    }else{
        n_bits = atoi(argv[1]);
    }
    fprintf(stderr, "n_bits = %d\n", n_bits);

    w = poly = 1 << n_bits;
    field_max = poly - 1;
    digits = _calc_log10(poly);
    /*
    fprintf(stderr, "w = %d\n", w);
    fprintf(stderr, "digits = %d\n", digits);
    fprintf(stderr, "field_max = %d\n", field_max);
    fprintf(stderr, "poly = %d\n", poly);
    */
    fprintf(stderr, "galois_field =\n");

    mem_size = sizeof(int) * w;
    s = (int *)malloc(mem_size);
    if (s == NULL) {
        fprintf(stderr, "failed malloc(%u).\n", mem_size);
        return -1;
    }
    for (i=1;i<poly;i+=2){
        galois_field = i;
        galois_field += poly;
        /*
        fprintf(stderr, "galois_field = %d\n", galois_field);
        */
        memset(s, '\0', sizeof(int) * w);

        len = 0;
        bit_pattern = 1;
        for (n=0;n<field_max;n++){
            /*
            fprintf(stderr, "n = %d, bit_pattern = %d\n", n, bit_pattern);
            */
            if (s[bit_pattern])
                break;
            len++;
            s[bit_pattern] = 1;
            bit_pattern <<= 1;
            if (bit_pattern & poly)
                bit_pattern ^= galois_field;
        }
        if (len == field_max) {
            fprintf(stderr, "%d, 0b", galois_field);
            p_bit(stderr, galois_field, n_bits + 1);
            fprintf(stderr, "\n");
        }
    }
/*
for galois_field in range(1, poly):
# galois_field = 25
    galois_field += poly
    # set LSB
    possible = set()
    bit_pattern = 1

    for n in range(field_max):
        if bit_pattern in possible:
            break
        possible.add(bit_pattern)
        bit_pattern <<= 1
        if bit_pattern & poly:
            bit_pattern ^= galois_field

    if len(possible) == field_max:
        print(fmt.format(galois_field, galois_field))
      # print('{:4d}, 0b{:b}'.format(galois_field, galois_field))
      # print('possible =', possible)
      # print()

*/

    free(s);

    return 0;
}
