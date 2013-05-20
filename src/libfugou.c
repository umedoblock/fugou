#include "libfugou.h"

void dump(void *ptr, int length, int width)
{
    int i;
    unsigned char *data = (unsigned char *)ptr;
    char fmt[BUFFER_SIZE];

    sprintf(fmt, "0x%%0%dx ", sizeof(void *) * 2);
    fprintf(stderr, "dump() start = %p\n", ptr);

    fprintf(stderr, "%*s", 3 + sizeof(void *) * 2, "");
    for (i=0;i<width;i++){
        fprintf(stderr, "%02x ", (unsigned int )(data + i) & 0x0f);
        if ((i + 1) % 4 == 0) {
            fprintf(stderr, "| ");
        }
    }
    if (((i + 1) % width) != 0)
        fprintf(stderr, "\n");

    for (i=0;i<length;i++) {
        if (i % width == 0) {
            fprintf(stderr, fmt, data + i);
        }
        fprintf(stderr, "%02x ", data[i]);
        if ((i + 1) % 4 == 0) {
            fprintf(stderr, "| ");
        }
        if ((i + 1) % width == 0) {
            fprintf(stderr, "\n");
        }
    }
    if (((length) % width) != 0)
        fprintf(stderr, "\n");
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
