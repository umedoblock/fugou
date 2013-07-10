#include "libfugou.h"

FILE *_log = NULL;

void set_logger(FILE *log)
{
    _log = log;
}

int _log_level = DEBUG_;

void set_logger_level(int log_level)
{
    _log_level = log_level;
}

const char *_log_level_names[] = {
    "DUMP", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "BUG"
};

void vlogger2(char *iso_format_time, char *__file__, int __line__, const char *_func_, int level, char *fmt, va_list ap)
{
    /* like a vfprintf(), vsnprintf() */
    FILE *_log = stderr;
    int _log_level = INFO;

    if (_log != NULL && level >= _log_level) {
        fprintf(_log, LOG_FORMAT, iso_format_time, __file__, __line__, _func_,
                                 _log_level_names[level]);
        vfprintf(_log, fmt, ap);
    }
}

/* ~/repos/hg/p2p/umatobi/trunk/sim/basic/log.c p2p_CmnLog() */
int current_isoformat_time(char *ts, size_t ts_size)
{
    int len = 0;
    time_t timer;
    struct tm *t_st;
    long tv_usec;

    /* 現在の時刻を取得 */
#ifdef _WIN32
    /* windows には gettimeofday() がなさそうなので。。。*/
    time(&timer);
    tv_usec = 0;
#else /* Linux */
    struct timeval tv_, *tv = &tv_;
    gettimeofday(tv, NULL);

    timer = tv->tv_sec;
    tv_usec = tv->tv_usec;
#endif

    /* 現在の時刻を構造体用に変換 */
    t_st = localtime(&timer);

    /* '2012-11-02T23:22:27.002481' */
    len += snprintf(ts, ts_size, FORMAT_TIMESTAMP,
        t_st->tm_year + 1900,
        t_st->tm_mon + 1,
        t_st->tm_mday,
        t_st->tm_hour,
        t_st->tm_min,
        t_st->tm_sec,
        tv_usec
    );

    return len;
}

void logger2(char *__file__, int __line__, const char *_func_, int level, char *fmt, ...)
{
    va_list ap;
    char iso_format_time[SS_SIZE];

    current_isoformat_time(iso_format_time, SS_SIZE);

    va_start(ap, fmt);
    vlogger2(iso_format_time, __file__, __line__, _func_, level, fmt, ap);
    va_end(ap);
}

void _debug2(char *__file__, int __line__, const char *_func_, int level, char *fmt, ...)
{
    #define DEBUG
    #ifdef DEBUG
    va_list ap;
    char iso_format_time[SS_SIZE];

    current_isoformat_time(iso_format_time, SS_SIZE);

    va_start(ap, fmt);
    vlogger2(iso_format_time, __file__, __line__, _func_, level, fmt, ap);
    va_end(ap);
    #endif
}

void
_calc_cipher_size(_cipher_size_brother_t *csb,
                   size_t text_size,
                   size_t block_size)
{
    csb->text_size = text_size;
    csb->block_size = block_size;
    csb->snip_size = text_size % block_size;
    if(csb->snip_size == 0)
        csb->padding_size = block_size;
    else
        csb->padding_size = block_size - csb->snip_size;
    csb->cipher_size = block_size + text_size + csb->padding_size;
}

void
_view_cipher_size(_cipher_size_brother_t *csb)
{
    DEBUG2("   csb->text_size = %u\n", csb->text_size);
    DEBUG2(" csb->cipher_size = %u\n", csb->cipher_size);
    DEBUG2("   csb->snip_size = %u\n", csb->snip_size);
    DEBUG2("csb->padding_size = %u\n", csb->padding_size);
    DEBUG2("  csb->block_size = %u\n", csb->block_size);
}

size_t _encrypt_cbc(
    uchar *c,
    uchar *m,
    uchar *iv,
    void *key,
    size_t text_size,
    size_t block_size,
    code_function encrypt
)
{
    int i;
    size_t encrypted_size = 0;
    uchar last_octet, *c_ = c;
    _cipher_size_brother_t csb_, *csb = &csb_;

    _calc_cipher_size(csb, text_size, block_size);
    _view_cipher_size(csb);

    if (c != iv)
        memcpy(c, iv, block_size);
    encrypted_size = block_size;
    DEBUG2("encrypt=%p in _encrypt_cbc() 想像できんっつーのっ！\n",
                  encrypt);
    /*
    [libfugou] [DEBUG] encrypt=(nil) in _encrypt_cbc()
    予想外でしたわ。もう諦めようかと思っていた。
    */

    while(encrypted_size < csb->cipher_size - block_size){
        DEBUG2("encrypted_size=%u < csb->cipher_size=%u\n",
                      encrypted_size, csb->cipher_size);
        c += block_size;
        for(i=0;i<block_size;i++)
            c[i] = iv[i] ^ m[i];
        encrypt(c, c, key);

        iv = c;
        m += block_size;
        encrypted_size += block_size;
    }
    c += block_size;

    memcpy(c, m,  csb->snip_size);
    memset(c + csb->snip_size, 0x00, csb->padding_size);
    last_octet = c[block_size - 1];
    last_octet |= csb->snip_size;
    DEBUG2("   csb->snip_size = %u, 0x%02x in _encrypt_cbc().\n", csb->snip_size, csb->snip_size);
    DEBUG2("csb->padding_size = %u, 0x%02x in _encrypt_cbc().\n", csb->padding_size, csb->padding_size);
    DEBUG2("last_octet = 0x%02x in _encrypt_cbc().\n", last_octet);

    c[block_size - 1] = last_octet;

    for(i=0;i<csb->block_size;i++)
        c[i] ^= iv[i];
    encrypt(c, c, key);

    encrypted_size += block_size;
    DEBUG2("c - c_ = %u in _encrypt_cbc()\n", (unt )(c + block_size - c_));
    DEBUG2("encrypted_size = %u in _encrypt_cbc()\n", encrypted_size);

    return encrypted_size;
}

size_t _decrypt_cbc(
    uchar *d,
    uchar *c,
    void *key,
    size_t cipher_size,
    size_t block_size,
    code_function decrypt
)
{
    int i;
    uchar *m = d, *iv, *d_ = d;
    size_t text_size, decrypted_size = 0;
    size_t snip_size;
    _cipher_size_brother_t csb_, *csb = &csb_;

    DEBUG2("decrypt=%p in _decrypt_cbc() くっそー。\n", decrypt);
    DEBUG2("_calc_cipher_size() cipher_size = %u in _decrypt_cbc() \n", cipher_size);
    iv = c;
    c += block_size;
    /*
    うわー、以下が camellia_decrypt_cbc_DataData() のbugの元凶では？
    while(decrypted_size < csb->cipher_size){
    なんか同じbug出てきた。。。
    */
    while(decrypted_size < cipher_size - block_size){
        decrypt(d, c, key);
        for(i=0;i<block_size;i++)
            m[i] = iv[i] ^ d[i];
        iv = c;
        c += block_size;
        d += block_size;
        m = d;
        decrypted_size += block_size;
    }

    DEBUG2("d[-1] = 0x%02x in _decrypt_cbc() \n", d[-1]);
    snip_size = d[-1] % block_size;
    DEBUG2("   snip_size = %u, 0x%02x in _decrypt_cbc().\n", snip_size, snip_size);
    DEBUG2("_calc_cipher_size() snip_size = %u in _decrypt_cbc() \n", snip_size);
    text_size = cipher_size - block_size * 2 + snip_size;
    DEBUG2("_calc_cipher_size() before text_size = %u in _decrypt_cbc() \n", text_size);
    _calc_cipher_size(csb, text_size, block_size);
    DEBUG2("_calc_cipher_size()  after text_size = %u in _decrypt_cbc() \n", text_size);
    DEBUG2("_decrypt_cbc()\n");
    _view_cipher_size(csb);
    DEBUG2("\n");

    DEBUG2("   csb->snip_size = %u, 0x%02x in _decrypt_cbc().\n", snip_size, csb->snip_size);
    DEBUG2("csb->padding_size = %u, 0x%02x in _decrypt_cbc().\n", csb->padding_size, csb->padding_size);
    DEBUG2("d - d_ = %u in _encrypt_cbc()\n", (unt )(d - d_));

    return text_size;
}

void bury_memory(void *ptr, size_t length)
{
    size_t i, index = (size_t )ptr;
    uchar *mem = (uchar *)ptr;

    for(i=0;i<sizeof(void *);i++) {
        mem[i] = (uint )ptr >> (8 * (sizeof(void *) - i - 1));
    }
    for(;i<length;i++) {
        mem[i] = (uchar )(index + i);
    }
}

void dump(void *ptr, int length, int width)
{
    int i;
    unsigned char *data = (unsigned char *)ptr;
    char fmt[SS_SIZE];

    if (! (_log_level == DEBUG_ || _log_level == DUMP)) {
        return;
    }

    sprintf(fmt, "0x%%0%dx ", sizeof(void *) * 2);
    fprintf(_log, "dump(ptr=%p, length=%d, width=%d) start\n",
                          ptr, length, width);

    fprintf(_log, "%*s", 3 + sizeof(void *) * 2, "");
    for (i=0;i<width;i++){
        fprintf(_log, "%02x ", (unsigned int )(data + i) & 0x0f);
        if ((i + 1) % 4 == 0) {
            fprintf(_log, "| ");
        }
    }
    if (((i + 1) % width) != 0)
        fprintf(_log, "\n");

    for (i=0;i<length;i++) {
        if (i % width == 0) {
            fprintf(_log, fmt, data + i);
        }
        fprintf(_log, "%02x ", data[i]);
        if ((i + 1) % 4 == 0) {
            fprintf(_log, "| ");
        }
        if ((i + 1) % width == 0) {
            fprintf(_log, "\n");
        }
    }
    if (((length) % width) != 0)
        fprintf(_log, "\n");
}

/* omoide/src/base64/base64.c */
/*2005/03/25*/

unt base64_encode(char *ss, uchar *data, unt n)
{
    unt i, j, m;
    char base[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unt foo=0;
    char t;

    i=0;
    m=0;
    while(n>3){
        n -= 3;
        foo <<= 8;
        foo |= (unt)data[i];
        i++;
        foo <<= 8;
        foo |= (unt)data[i];
        i++;
        foo <<= 8;
        foo |= (unt)data[i];
        i++;
        ss[m] = t = base[0x3f&(foo>>18)];
        m++;
        ss[m] = t = base[0x3f&(foo>>12)];
        m++;
        ss[m] = t = base[0x3f&(foo>>6)];
        m++;
        ss[m] = t = base[0x3f&foo];
        m++;
    }

    for(j=0;j<n;j++){
        foo <<= 8;
        foo |= (unt)data[i];
        i++;
    }
    for(;j<3;j++){
        foo <<= 8;
    }

    if(n==3){
        ss[m] = base[0x3f&(foo>>18)];
        m++;
        ss[m] = base[0x3f&(foo>>12)];
        m++;
        ss[m] = base[0x3f&(foo>>6)];
        m++;
        ss[m] = base[0x3f&foo];
        m++;
    }else if(n==2){
        ss[m] = base[0x3f&(foo>>18)];
        m++;
        ss[m] = base[0x3f&(foo>>12)];
        m++;
        ss[m] = base[0x3f&(foo>>6)];
        m++;
        ss[m] = '=';
        m++;
    }else{
        ss[m] = base[0x3f&(foo>>18)];
        m++;
        ss[m] = base[0x3f&(foo>>12)];
        m++;
        ss[m] = '=';
        m++;
        ss[m] = '=';
        m++;
    }
    ss[m] = '\0';

    return m;
}

/*2005/03/25*/
unt base64_decode(uchar *data, char *ss)
{
    #define XXXX 0
    const char base[128] = {
        XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, XXXX, XXXX, 0x3e, XXXX, XXXX, XXXX, 0x3f,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
        0x3c, 0x3d, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
        0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
        0x17, 0x18, 0x19, XXXX, XXXX, XXXX, XXXX, XXXX,
        XXXX, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
        0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
        0x31, 0x32, 0x33, XXXX, XXXX, XXXX, XXXX, XXXX
    };
    int i, j, k, m;
    unsigned int foo=0, buf=0;
    unt n;

    n = (unt)strlen(ss);
    i=0;
    m=0;
    while(n>4){
        n -= 4;
        for(j=0;j<4;j++,i++){
            foo <<= 8;
            foo |= (unt)base[(int )ss[i]];
        }
        buf = ((foo&0x3f000000)>>6) | ((foo&0x3f0000)>>4) | ((foo&0x3f00)>>2) | (foo&0x3f);
        data[m] = (uchar)(0xff&(buf >> 16));
        m++;
        data[m] = (uchar)(0xff&(buf >> 8));
        m++;
        data[m] = (uchar)(0xff&(buf));
        m++;
    }
    j=0;
    k=i;
    for(j=0;j<4&&(ss[k] != '=');){
        j++;
        k++;
    }
    n -= j;
    for(k=0;k<j;k++){
        foo <<= 8;
        foo |= (unt)base[(int )ss[i]];
        i++;
    }
    for(;k<4;k++){
        foo <<= 8;
    }
    buf = ((foo&0x3f000000)>>6) | ((foo&0x3f0000)>>4) | ((foo&0x3f00)>>2) | (foo&0x3f);

    //fprintf(stdout, "m = %d\n", m);
    //fprintf(stdout, "(i, j, k, m) = (%d, %d, %d, %d)\n", i, j, k, m);
    if(j==4){
        data[m] = (uchar)(0xff&(buf >> 16));
        m++;
        data[m] = (uchar)(0xff&(buf >> 8));
        m++;
        data[m] = (uchar)(0xff&(buf));
        m++;
    }else if(j==3){
        data[m] = (uchar)(0xff&(buf >> 16));
        m++;
        data[m] = (uchar)(0xff&(buf >> 8));
        m++;
    }else if(j==2){
        data[m] = (uchar)(0xff&(buf >> 16));
        m++;
    }
    return m;
}

/*2005/03/25*/
int base64_equal(uchar *x, uchar *y, unt n)
{

    for(;n>0&&*x==*y;n--){
        ;
    }

    return !(n);
}

/* omoide/src/base64/ss.c */
void strtodata(uchar *data, char *ss, unt n)
{
    unt i;
    uchar *bp = data;
    int ch;

    for(i=0;i<n;i++){
        ch = *ss;
        *bp++ = ch >> 8;
        *bp++ = ch;
        ss++;
    }
}

void datatostr(char *ss, uchar *data, unt n)
{
    uchar *bp = data;
    unt i;
    int ch;

    for(i=0;i<n/2;i++){
        ch = 0;
        //*ss <<= 8;
        ch |= *bp++;
        ch <<= 8;
        ch |= *bp++;
        *ss = ch;
        ss++;
    }
}
