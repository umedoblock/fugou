/* libsha.c
 * Copyright (C) 平成25(2013)年 梅濁酒(umedoblock)
 * FIP180 SECURE HASH STANDARD
 * refs: http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */
/* sha1.c 見てて思う。よく完成させたなー。
 * 正味の話、あれは滅茶苦茶だぜー。
 * さっぱり分からんわ。余りの分かりにくさに再利用するの止めたもん。
 * 構造体とか知らずに、ブリブリ実装を頑張っていたんだなー。
 * C 言語好きだったもんねー。
 */

/* fip180-1.html の使用する用語上、
 * "text" ではなく、"message" がより適切な単語という事は分かっているが、
 * もう今更、修正したくない。面倒くさい。
 * 今後、"message" を使った方が適切であると思われる箇所については、
 * "message" を使う。
 * 最初に意識して、仕様・用語を把握しなかった事が問題であった。
 */
#include "sha.h"
#ifdef SHA_DEBUG
#include "sha_test.h"
#endif

void _sha1_compute(sha1sum_t *sha1sum);
void _sha1_compute_final(sha1sum_t *sha1sum,
                         uint a, uint b, uint c, uint d, uint e);
void _sha1_pad_final_block(sha1sum_t *sha1sum, long_size_t remained_text_size);
void _sha1_load_text_as_block(sha1sum_t *sha1sum, uchar *text);
void _sha1_load_text(
    sha1sum_t *sha1sum, uchar *text, long_size_t text_size);
void _sha1_normalize_endian(sha1sum_t *sha1sum);
void _sha1_set_value(sha1sum_t *sha1sum);
void _sha1_get_hex(char *hex, sha1sum_t *sha1sum);
inline uint _sha1_for_portability_about_endian(uint n);
int _bin2hex(int bin);

/*****************************************************************************/
/* below for library user ****************************************************/
/*****************************************************************************/

long_size_t sha1(sha1sum_t *sha1sum, uchar *text, long_size_t text_size)
{
    size_t remained_text_size, original_message_length_of_bits;

    _sha1_init(sha1sum);

    remained_text_size = text_size;
    while(remained_text_size >= SHA1SUM_BLOCK_SIZE){
        /* growing, growing ... */
        remained_text_size = _sha1_feed(sha1sum,
                                        text + sha1sum->message_size,
                                        remained_text_size);
    }

    /* finally ... */
    original_message_length_of_bits = \
        _sha1_hatch(sha1sum,
                    text + sha1sum->message_size,
                    remained_text_size);

    return original_message_length_of_bits;
}

void sha1_get_digest(uchar *digest, sha1sum_t *sha1sum)
{
    _sha1_set_value(sha1sum);
    memcpy(digest, sha1sum->value, SHA1SUM_HASH_SIZE);
}

void sha1_get_hex(char *hex, sha1sum_t *sha1sum)
{
    _sha1_set_value(sha1sum);
    _sha1_get_hex(hex, sha1sum);
}

long_size_t sha1_as_file(sha1sum_t *sha1sum, FILE *fp)
{
    size_t original_message_length_of_bits;
    size_t remained_size, read_size, append_size;
    uchar buf[SHA1SUM_BLOCK_SIZE * 16]; /* 1024 octets */

    _sha1_init(sha1sum);

    read_size = fread(buf, sizeof(uchar), sizeof(buf), fp);
    while(read_size == sizeof(buf)){
        _sha1_append(sha1sum, buf, read_size);
        read_size = fread(buf, sizeof(uchar), sizeof(buf), fp);
    }

    remained_size = read_size % SHA1SUM_BLOCK_SIZE;
    append_size = read_size - remained_size;
    if (append_size) {
        _sha1_append(sha1sum, buf, append_size);
    }

    /* finally ... */
    original_message_length_of_bits = \
        _sha1_hatch(sha1sum,
                    buf + append_size,
                    remained_size);

    return original_message_length_of_bits;
}

/*****************************************************************************/
/* below for developer using *************************************************/
/*****************************************************************************/

void _sha1_init(sha1sum_t *sha1sum)
{
    sha1sum->message_size = 0ULL;
    sha1sum->H[0] = SHA1SUM_H0;
    sha1sum->H[1] = SHA1SUM_H1;
    sha1sum->H[2] = SHA1SUM_H2;
    sha1sum->H[3] = SHA1SUM_H3;
    sha1sum->H[4] = SHA1SUM_H4;
}

long_size_t _sha1_feed(sha1sum_t *sha1sum, uchar *text,
                                           long_size_t remained_text_size)
{
    /* 4. MESSAGE PADDING */
    _sha1_load_text_as_block(sha1sum, text);
    /* 7. COMPUTING THE MESSAGE DIGEST */
    _sha1_compute(sha1sum);

    sha1sum->message_size += SHA1SUM_BLOCK_SIZE;
    remained_text_size -= SHA1SUM_BLOCK_SIZE;
    return remained_text_size;
}

long_size_t _sha1_hatch(sha1sum_t *sha1sum, uchar *text,
                                            long_size_t remained_text_size)
{
    /* for remained text */
    _sha1_load_text(sha1sum, text, remained_text_size);
    _sha1_pad_final_block(sha1sum, remained_text_size);
    _sha1_compute(sha1sum);
    return sha1sum->length; /* original message of bit length */
}

/* must do run _sha1_init() before run _sha1_append().
 * if hope to finish to _sha1_compute(),
 * must do run _sha1_hatch() after run _sha1_append().
 * append_size must be multiple of SHA1SUM_BLOCK_SIZE.
 */
long_size_t _sha1_append(sha1sum_t *sha1sum,
                         uchar *append, long_size_t append_size)
{
    long_size_t remained_text_size, fed_size;

    fed_size = 0ULL;
    remained_text_size = append_size;
    while (remained_text_size) {
        remained_text_size = \
            _sha1_feed(sha1sum,
                       append + fed_size,
                       remained_text_size);
        fed_size += SHA1SUM_BLOCK_SIZE;
    }

    return fed_size;
}

/*****************************************************************************/
/* below no change please ****************************************************/
/*****************************************************************************/

void _sha1_compute(sha1sum_t *sha1sum)
{
    int t;
    uint temp = 0;
    uint a, b, c, d , e;
    /* stands for W, M, H*/
    uint *W = sha1sum->W, *H = sha1sum->H;

    _sha1_normalize_endian(sha1sum);

    /* 7. COMPUTING THE MESSAGE DIGEST */

    /* a. Divide Mi into 16 words W0, W1, ... , W15, where
     *    W0 is the left-most word.
     */
    /*
    memcpy(W, M, SHA1SUM_BLOCK_SIZE);
    */

    /* b. For t = 16 to 79
     *    let Wt = S1(Wt-3 XOR Wt-8 XOR Wt-14 XOR Wt-16).
     */
    for(t=16;t<=79;t++){
        W[t] = Sn((W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]), 1);
    }

    #ifdef SHA_DEBUG
    fprintf(stdout, "_sha1_compute(sha1sum=0x%p)\n", sha1sum);
    _sha1_dump_block_of_W(sha1sum);
    #endif

    /* c. Let A = H0, B = H1, C = H2, D = H3, E = H4. */
    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];

    #ifdef SHA_DEBUG
    _sha1_dump_ABCDE_header();
    _sha1_dump_ABCDE(-1, a, b, c, d, e);
    #endif

    /* d. For t = 0 to 79 do
          TEMP = S5(A) + ft(B,C,D) + E + Wt + Kt;
          E = D; D = C; C = S30(B); B = A; A = TEMP; */
    for(t=0;t<=79;t++){
        if (t<20)
            temp = Sn(a, 5) + ft_0_19(b, c, d) + e + W[t] + Kt_0_19;
        else if (t<40)
            temp = Sn(a, 5) + ft_20_39(b, c, d) + e + W[t] + Kt_20_39;
        else if (t<60)
            temp = Sn(a, 5) + ft_40_59(b, c, d) + e + W[t] + Kt_40_59;
        else if (t<80)
            temp = Sn(a, 5) + ft_60_79(b, c, d) + e + W[t] + Kt_60_79;
        e = d;
        d = c;
        c = Sn(b, 30);
        b = a;
        a = temp;
        #ifdef SHA_DEBUG
        _sha1_dump_ABCDE(t, a, b, c, d, e);
        #endif
    }

    _sha1_compute_final(sha1sum, a, b, c, d, e);
}

void _sha1_compute_final(sha1sum_t *sha1sum,
                         uint a, uint b, uint c, uint d, uint e)
{
    uint *H = sha1sum->H;

    /* e. Let H0 = H0 + A, H1 = H1 + B, H2 = H2 + C,
     *    H3 = H3 + D, H4 = H4 + E.
     */
    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
}

void _sha1_load_text(
    sha1sum_t *sha1sum, uchar *text, long_size_t text_size)
{
    uint *M = sha1sum->W;
    memcpy(M, text, text_size);
}

void _sha1_load_text_as_block(
    sha1sum_t *sha1sum, uchar *text)
{
    _sha1_load_text(sha1sum, text, SHA1SUM_BLOCK_SIZE);
}

void _sha1_pad_final_block(sha1sum_t *sha1sum,
                           long_size_t remained_text_size)
{
    size_t at_least_size, zero_padding_size;
    uint *M = sha1sum->W;
    uchar *m = (uchar *)M;

    m = (uchar *)M;
    m[remained_text_size] = 0x80;

    /* 1ULL means "1" followed. */
    at_least_size = remained_text_size + 1ULL + \
                    SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE;
    if (at_least_size <= SHA1SUM_BLOCK_SIZE) {
        zero_padding_size = SHA1SUM_BLOCK_SIZE - at_least_size;
        if (zero_padding_size) {
            memset(m + remained_text_size + 1, '\0', zero_padding_size);
        }
    }
    else {
        zero_padding_size = SHA1SUM_BLOCK_SIZE - (remained_text_size + 1);
        memset(m + remained_text_size + 1, '\0', zero_padding_size);
        _sha1_compute(sha1sum);

        zero_padding_size = 72;
        memset(m, '\0', zero_padding_size);
    }

    sha1sum->message_size += remained_text_size;
    sha1sum->length = sha1sum->message_size * OCTET;
    ULL2UCHARS(m + SHA1SUM_BLOCK_SIZE - \
                   SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE, sha1sum->length);

    #ifdef SHA_DEBUG
    fprintf(stdout, "at_least_size = %u\n", at_least_size);
    fprintf(stdout, "zero_padding_size = %u\n", zero_padding_size);
    fprintf(stdout, "remained_text_size = %llu\n", remained_text_size);
    fprintf(stdout, "remained_text_size + 1 = %llu\n", remained_text_size + 1);
    fprintf(stdout, "sha1sum->length = 0x%016llx\n", sha1sum->length);
    fprintf(stdout, "\n");
    fprintf(stdout, "SHA1SUM_BLOCK_SIZE = %llu\n", SHA1SUM_BLOCK_SIZE);
    fprintf(stdout, "SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE = %llu\n", \
                     SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE);
    fprintf(stdout, "SHA1SUM_BLOCK_SIZE -\n");
    fprintf(stdout, "SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE = %llu\n", \
                     SHA1SUM_BLOCK_SIZE - \
                     SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE);
    fprintf(stdout, "\n");
    #endif
}

void _sha1_get_hex(char *hex, sha1sum_t *sha1sum)
{
    uint i;
    uchar v;

    for (i=0;i<sizeof(sha1sum->value);i++) {
        v = sha1sum->value[i];
        hex[i*2] = _bin2hex(v >> 4);
        hex[i*2+1] = _bin2hex(v & 0xf);
    }
    hex[i*2] = '\0';
}

#define elementof(x) (sizeof((x)) / sizeof((x[0])))

void _sha1_set_value(sha1sum_t *sha1sum)
{
    uint i;

    for (i=0;i<elementof(sha1sum->H);i++) {
        UINT2UCHARS(sha1sum->value + i * sizeof(uint), sha1sum->H[i]);
    }
}

void _sha1_normalize_endian(sha1sum_t *sha1sum)
{
    uint t;
    uint *M = sha1sum->W;

    for (t=0;t<SHA1SUM_BLOCK_SIZE / sizeof(uint);t++) {
        M[t] = _sha1_for_portability_about_endian(M[t]);
    }
}

inline uint _sha1_for_portability_about_endian(uint n)
{
    uint i;
    uchar *p = (uchar *)(&n);
    uint v = 0;

    for (i=0;i<sizeof(uint);i++) {
        v <<= 8;
        v += p[i];
    }

    return v;
}

void _view_hash(uchar *hash, int len_hash)
{
    int i;

    for (i=0;i<len_hash/8;i++){
        fprintf(stdout, "%02x", hash[i]);
    }
}

int _bin2hex(int bin)
{
    int hex = -1;

    if (bin >= 0x0 && bin <= 0x9) {
        hex = '0' + bin;
    }
    else if (bin >= 0xa && bin <= 0xf) {
        hex = 'a' + (bin - 0xa);
    }
    else {
        hex = -1;
    }

    return hex;
}

