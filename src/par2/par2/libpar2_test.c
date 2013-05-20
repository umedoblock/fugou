#include "libpar2.h"

void test_make_e_matrix(void)
{
    reed_solomon_t *rds;
    uint bits, bitss = [4, 8, 16, 24, 0];
    int i;

    par2_file_t p2f_, *p2f = NULL;
    par2_matrix_t p2m_, *p2m = NULL;
    par2_encode_t p2e_, *p2e = NULL;

    for(i=0;bitss[i] != 0;i++) {
        bit = bitss[i];
        rds = par2_get_reed_solomon(bits);
        par2_init_p2m(p2m, mem, rds);
    }
    mem = par2_allocate(&p2f_, &p2m_, &p2e_, rds, division);
    if (mem == NULL)
        return PAR2_MALLOC_ERROR;
    mem_ = mem;

}

void test_suite(void)
{
    test_make_e_matrix();
}

int main(void)
{
    par2_big_bang();
    test_suite();
    par2_ultimate_fate_of_the_universe();

    return 0;
}
