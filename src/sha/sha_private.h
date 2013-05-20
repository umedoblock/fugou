#include "../libfugou_base.h"

#define SHA1SUM_HASH_BITS (160ULL)
#define SHA1SUM_BLOCK_BITS (512ULL)
#define SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_BITS (64ULL)
#define SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_SIZE \
       (SHA1SUM_ORIGINAL_MESSAGE_LENGTH_AREA_BITS / OCTET)

#define UCHARS2UINT(uc) ( \
            ((uint )(uc)[0] << 24) + \
            ((uint )(uc)[1] << 16) + \
            ((uint )(uc)[2] << 8) + \
            ((uint )(uc)[3]) \
        )
#define UINT2UCHARS(uc, ui) do { \
        (uc)[0] = (uchar )((ui) >> 24); \
        (uc)[1] = (uchar )((ui) >> 16); \
        (uc)[2] = (uchar )((ui) >> 8); \
        (uc)[3] = (uchar )((ui)); \
    } while(0)
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

/* 3. OPERATIONS ON WORDS */
#define Sn(X, n)  (((X) << (n)) | ((X) >> (32-(n))))

/* 5. FUNCTINS USED */
#define ft_0_19(B,C,D)  ((B & C) | ((~ B) & D))       /* ( 0 <= t <= 19) */
#define ft_20_39(B,C,D) (B ^ C ^ D)                   /* (20 <= t <= 39) */
#define ft_40_59(B,C,D) ((B & C) | (B & D) | (C & D)) /* (40 <= t <= 59) */
#define ft_60_79(B,C,D) (B ^ C ^ D)                   /* (60 <= t <= 79) */

/* 6. CONSTANTS USED */
#define Kt_0_19  0x5a827999 /* ( 0 <= t <= 19) */
#define Kt_20_39 0x6ed9eba1 /* (20 <= t <= 39) */
#define Kt_40_59 0x8f1bbcdc /* (40 <= t <= 59) */
#define Kt_60_79 0xca62c1d6 /* (60 <= t <= 79) */

/* 7. COMPUTING THE MESSAGE DIGEST */
/* initilized */
#define SHA1SUM_H0 0x67452301
#define SHA1SUM_H1 0xefcdab89
#define SHA1SUM_H2 0x98badcfe
#define SHA1SUM_H3 0x10325476
#define SHA1SUM_H4 0xc3d2e1f0
