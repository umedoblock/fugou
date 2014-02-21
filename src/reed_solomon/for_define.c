#include <stdio.h>

typedef unsigned int uint;
typedef unsigned short ushort;

typedef union {
    ushort u16;
    uint u32;
} number_t;

uint mul16(uint n, number_t *num)
{
    return n * num->u16;
}

uint mul32(uint n, number_t *num)
{
    return n * num->u32;
}

#define _mulXXX(X) \
    for (;;) { \
        _TMP = n * num->u##X; \
        break; \
    }

uint _mul16(uint n, number_t *num)
{
    ushort _TMP;

    _mulXXX(16);

    return _TMP;
}

uint _mul32(uint n, number_t *num)
{
    ushort _TMP;

    _mulXXX(16);

    return _TMP;
}

int main(void)
{
    number_t num_, *num = &num_;
    uint n = 0;

    n = 16;
    num->u16 = 16;
    fprintf(stdout, "mul16(n=16, 16) = %u\n", mul16(n, num));
    fprintf(stdout, "_mul16(n=16, 16) = %u\n", _mul16(n, num));

    n = 32;
    num->u32 = 32;
    fprintf(stdout, "mul32(n=32, 32) = %u\n", mul32(n, num));
    fprintf(stdout, "_mul32(n=32, 32) = %u\n", _mul32(n, num));

    return 0;
}
