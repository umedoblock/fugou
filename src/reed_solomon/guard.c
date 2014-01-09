/* http://www.umedoblock.net/trac/fugou/changeset/8dfb2f0278711a10a1173c0932039cc84f0ce350/fugou 
 * の commit 時に考えた高速化が有効か検証する。
 */

#include <stdio.h>
#include <time.h>

typedef unsigned int uint;

int main(void)
{
    /* no need to think about 32bit */
    uint a32, b32, gfi32[7], result, gf_max = 7, guard;
    clock_t s, e, t0, t1;
    int i, j, k, n = 10000000;

    for (i=0;i<7;i++) {
        gfi32[i] = i;
    }

    result = 0U;
    s = clock();
    for (k=0;k<n;k++) {
        for (j=0;j<7;j++) {
            for (i=0;i<7;i++) {
                a32 = gfi32[i];
                b32 = gfi32[j];
                if (a32 >= b32) {
                    result ^= gfi32[a32 - b32];
                }
                else {
                    /* a32 < b32 */
                    result ^= gfi32[gf_max + a32 - b32];
                }
            }
        }
    }
    e = clock();
    fprintf(stdout, "result=0x%08x\n", result);
    fprintf(stdout, "e-s=%ld\n", e - s);

    result = 0U;
    s = clock();
    for (k=0;k<n;k++) {
        for (j=0;j<7;j++) {
            for (i=0;i<7;i++) {
                a32 = gfi32[i];
                b32 = gfi32[j];
                guard = 0U;

                if (a32 < b32)
                    guard = gf_max;

                result ^= gfi32[guard + a32 - b32];
            }
        }
    }
    e = clock();
    fprintf(stdout, "result=0x%08x\n", result);
    fprintf(stdout, "e-s=%ld\n", e - s);

    #if 0
    a32 = gfi32[a];
    b32 = gfi32[b];
    if (a32 >= b32)
        return rs->gfi.u32[a32 - b32];
    /* a32 < b32 */
    return rs->gfi.u32[rs->gf_max + a32 - b32];

    /* 高速化する機会があれば、今(=平成26年1月9日23時11分7秒)思いついた
     * 手法を適用してみたい。
     * あと、RS_gf32, RS_gfi32 使うようにしないと。
     */
    guard = 0U;
    if (a32 < b32)
        guard = rs->gf_max;
    return rs->gfi.u32[guard + a32 - b32];
    #endif
    return 0;
}
