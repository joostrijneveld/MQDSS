#include <stdio.h>
#include <immintrin.h>
#include "mq.h"
#include "params.h"

static inline __m256i reduce_16(__m256i r, __m256i _w31, __m256i _w2114)
{
    __m256i exp = _mm256_mulhi_epi16(r, _w2114);
    return _mm256_sub_epi16(r, _mm256_mullo_epi16(_w31, exp));
}

static inline
void calc_quadratic_terms_rec_32_avx2(unsigned char *xixj, __m256i xi, __m256i xj, __m256i mask_2114, __m256i mask_reduce)
{
    int i;
    unsigned char *cc = &xixj[0];
    __m256i xj1, xj2, r_x1, r_x2;

    xj1 = _mm256_unpacklo_epi8(xj, _mm256_setzero_si256());  // 2,0
    xj2 = _mm256_unpackhi_epi8(xj, _mm256_setzero_si256());  // 3,1

    __m128i xi_128 = _mm256_castsi256_si128(xi);
    for (i = 0; i < 16; i++) {
        __m256i xi_1 = _mm256_broadcastb_epi8(xi_128);
        xi_128 = _mm_srli_si128(xi_128, 1);
        r_x1 = _mm256_maddubs_epi16(xj1, xi_1);
        r_x2 = _mm256_maddubs_epi16(xj2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
    xi_128 = _mm256_extracti128_si256(xi, 1);
    for (i = 0; i < 16; i++) {
        __m256i xi_1 = _mm256_broadcastb_epi8(xi_128);
        xi_128 = _mm_srli_si128(xi_128, 1);
        r_x1 = _mm256_maddubs_epi16(xj1, xi_1);
        r_x2 = _mm256_maddubs_epi16(xj2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
}

static inline
void calc_quadratic_terms_rec_32_g_avx2(unsigned char *xixj, __m256i xi, __m256i xj
        , __m256i yi, __m256i yj, __m256i mask_2114, __m256i mask_reduce)
{
    int i;
    unsigned char *cc = &xixj[0];
    __m256i xj1, xj2, r_x1, r_x2;

    xj1 = _mm256_unpacklo_epi8(yj, xj);  // 2,0
    xj2 = _mm256_unpackhi_epi8(yj, xj);  // 3,1

    __m128i xi_128 = _mm_unpacklo_epi8(_mm256_castsi256_si128(xi), _mm256_castsi256_si128(yi));
    for (i = 0; i < 8; i++) {
        __m256i xi_1 = _mm256_broadcastw_epi16(xi_128);
        xi_128 = _mm_srli_si128(xi_128, 2);
        r_x1 = _mm256_maddubs_epi16(xj1, xi_1);
        r_x2 = _mm256_maddubs_epi16(xj2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
    xi_128 = _mm_unpackhi_epi8(_mm256_castsi256_si128(xi), _mm256_castsi256_si128(yi));
    for (i = 0; i < 8; i++) {
        __m256i xi_1 = _mm256_broadcastw_epi16(xi_128);
        xi_128 = _mm_srli_si128(xi_128, 2);
        r_x1 = _mm256_maddubs_epi16(xj1, xi_1);
        r_x2 = _mm256_maddubs_epi16(xj2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
    xi_128 = _mm_unpacklo_epi8(_mm256_extracti128_si256(xi, 1), _mm256_extracti128_si256(yi, 1));
    for (i = 0; i < 8; i++) {
        __m256i xi_1 = _mm256_broadcastw_epi16(xi_128);
        xi_128 = _mm_srli_si128(xi_128, 2);
        r_x1 = _mm256_maddubs_epi16(xj1, xi_1);
        r_x2 = _mm256_maddubs_epi16(xj2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
    xi_128 = _mm_unpackhi_epi8(_mm256_extracti128_si256(xi, 1), _mm256_extracti128_si256(yi, 1));
    for (i = 0; i < 8; i++) {
        __m256i xi_1 = _mm256_broadcastw_epi16(xi_128);
        xi_128 = _mm_srli_si128(xi_128, 2);
        r_x1 = _mm256_maddubs_epi16(xj1, xi_1);
        r_x2 = _mm256_maddubs_epi16(xj2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
}

static inline
void calc_quadratic_terms_tri_32_avx2(unsigned char *xixj, __m256i xij, __m256i mask_2114, __m256i mask_reduce)
{
    unsigned char *cc = &xixj[0];
    int i;

    __m256i x1, x2, r_x1, r_x2;

    __m256i _x1 = _mm256_unpacklo_epi8(xij, _mm256_setzero_si256());  // 2,0
    __m256i _x2 = _mm256_unpackhi_epi8(xij, _mm256_setzero_si256());  // 3,1

    x1 = _mm256_permute2x128_si256(_x1, _x2, 0x20);
    x2 = _mm256_permute2x128_si256(_x1, _x2, 0x31);

    __m128i xi = _mm256_castsi256_si128(xij);
    for (i = 0; i < 8; i++) {
        __m256i xi_1 = _mm256_broadcastb_epi8(xi);
        xi = _mm_srli_si128(xi, 1);
        r_x1 = _mm256_maddubs_epi16(x2, xi_1);
        xi_1 = _mm256_broadcastb_epi8(xi);
        xi = _mm_srli_si128(xi, 1);
        r_x2 = _mm256_maddubs_epi16(x2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }

    r_x1 = _mm256_maddubs_epi16(x1, x1);
    r_x2 = _mm256_maddubs_epi16(x2, x2);
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    __m256i upside_down_x1 = _mm256_permute4x64_epi64(x1, 0x4e);
    __m256i upside_down_x2 = _mm256_permute4x64_epi64(x2, 0x4e);
    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 2));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 2));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 4));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 4));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 6));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 6));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 8));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 8));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 10));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 10));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 12));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 12));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, _mm256_alignr_epi8(upside_down_x1, x1, 14));
    r_x2 = _mm256_maddubs_epi16(x2, _mm256_alignr_epi8(upside_down_x2, x2, 14));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(x1, upside_down_x1);
    r_x2 = _mm256_maddubs_epi16(x2, upside_down_x2);
    r_x1 = _mm256_permute2x128_si256(r_x1, r_x2, 0x20);
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = _mm256_packs_epi16(r_x1, r_x1);
    r_x1 = _mm256_permute4x64_epi64(r_x2, 0x08);
    _mm_store_si128((__m128i*)cc, _mm256_castsi256_si128(r_x1));
}

static inline
void calc_quadratic_terms_tri_32_g_avx2(unsigned char *xiyj_plus_xjyi, __m256i xij, __m256i yij
    , __m256i mask_2114, __m256i mask_reduce)
{
    unsigned char *cc = &xiyj_plus_xjyi[0];
    int i;

    __m256i x1, x2, r_x1, r_x2;
    __m256i _x1 = _mm256_unpacklo_epi8(yij, xij);  // 2,0
    __m256i _x2 = _mm256_unpackhi_epi8(yij, xij);  // 3,1
    x1 = _mm256_permute2x128_si256(_x1, _x2, 0x20);
    x2 = _mm256_permute2x128_si256(_x1, _x2, 0x31);
    __m256i _y1 = _mm256_unpacklo_epi8(xij, yij);  // 2,0
    __m256i _y2 = _mm256_unpackhi_epi8(xij, yij);  // 3,1
    __m256i y1 = _mm256_permute2x128_si256(_y1, _y2, 0x20);
    __m256i y2 = _mm256_permute2x128_si256(_y1, _y2, 0x31);

    __m128i xi = _mm256_castsi256_si128(y1);
    for (i = 0; i <4; i++) {
        __m256i xi_1 = _mm256_broadcastw_epi16(xi);
        xi = _mm_srli_si128(xi, 2);
        r_x1 = _mm256_maddubs_epi16(x2, xi_1);
        xi_1 = _mm256_broadcastw_epi16(xi);
        xi = _mm_srli_si128(xi, 2);
        r_x2 = _mm256_maddubs_epi16(x2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }
    xi = _mm256_extracti128_si256(y1, 1);
    for (i = 0; i < 4; i++) {
        __m256i xi_1 = _mm256_broadcastw_epi16(xi);
        xi = _mm_srli_si128(xi, 2);
        r_x1 = _mm256_maddubs_epi16(x2, xi_1);
        xi_1 = _mm256_broadcastw_epi16(xi);
        xi = _mm_srli_si128(xi, 2);
        r_x2 = _mm256_maddubs_epi16(x2, xi_1);
        r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
        r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
        _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
        cc += 32;
    }

    r_x1 = _mm256_maddubs_epi16(y1, x1);
    r_x2 = _mm256_maddubs_epi16(y2, x2);
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    __m256i upside_down_x1 = _mm256_permute4x64_epi64(x1, 0x4e);
    __m256i upside_down_x2 = _mm256_permute4x64_epi64(x2, 0x4e);

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 2));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 2));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 4));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 4));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 6));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 6));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 8));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 8));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 10));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 10));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 12));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 12));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, _mm256_alignr_epi8(upside_down_x1, x1, 14));
    r_x2 = _mm256_maddubs_epi16(y2, _mm256_alignr_epi8(upside_down_x2, x2, 14));
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = reduce_16(r_x2, mask_reduce, mask_2114);
    _mm256_store_si256((__m256i*)cc, _mm256_packs_epi16(r_x1, r_x2));
    cc += 32;

    r_x1 = _mm256_maddubs_epi16(y1, upside_down_x1);
    r_x2 = _mm256_maddubs_epi16(y2, upside_down_x2);
    r_x1 = _mm256_permute2x128_si256(r_x1, r_x2, 0x20);
    r_x1 = reduce_16(r_x1, mask_reduce, mask_2114);
    r_x2 = _mm256_packs_epi16(r_x1, r_x1);
    r_x1 = _mm256_permute4x64_epi64(r_x2, 0x08);
    _mm_store_si128((__m128i*)cc, _mm256_castsi256_si128(r_x1));
}

#define EVAL_YMM_0(xx) {\
    __m128i tmp = _mm256_castsi256_si128(xx); \
    for (int i = 0; i < 8; i++) { \
        __m256i _xi = _mm256_broadcastw_epi16(tmp); \
        tmp = _mm_srli_si128(tmp, 2); \
        for (int j = 0; j < 4; j++) { \
            __m256i coeff = _mm256_loadu_si256((__m256i const *) F); \
            F += 32; \
            yy[j] = _mm256_add_epi16(yy[j], _mm256_maddubs_epi16(_xi, coeff)); \
        } \
    } \
}

#define EVAL_YMM_1(xx) {\
    __m128i tmp = _mm256_extracti128_si256(xx, 1); \
    for (int i = 0; i < 8; i++) { \
        __m256i _xi = _mm256_broadcastw_epi16(tmp); \
        tmp = _mm_srli_si128(tmp, 2); \
        for (int j = 0; j < 4; j++) { \
            __m256i coeff = _mm256_loadu_si256((__m256i const *) F); \
            F += 32; \
            yy[j] = _mm256_add_epi16(yy[j], _mm256_maddubs_epi16(_xi, coeff)); \
        } \
    } \
}

#define REDUCE_(yy) { \
    yy[0] = reduce_16(yy[0], mask_reduce, mask_2114); \
    yy[1] = reduce_16(yy[1], mask_reduce, mask_2114); \
    yy[2] = reduce_16(yy[2], mask_reduce, mask_2114); \
    yy[3] = reduce_16(yy[3], mask_reduce, mask_2114); \
}

void MQ(gf31 *fx, const gf31 *x, const signed char *F)
{
    int i;

    __m256i mask_2114 = _mm256_set1_epi32(2114*65536 + 2114);
    __m256i mask_reduce = _mm256_srli_epi16(_mm256_cmpeq_epi16(mask_2114, mask_2114), 11);

    __m256i xi[4];
    xi[0] = _mm256_loadu_si256((__m256i const *) (x));
    xi[1] = _mm256_loadu_si256((__m256i const *) (x+16));
    xi[2] = _mm256_loadu_si256((__m256i const *) (x+32));
    xi[3] = _mm256_loadu_si256((__m256i const *) (x+48));

    __m256i _zero = _mm256_setzero_si256();
    xi[0] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[0]), xi[0]);
    xi[1] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[1]), xi[1]);
    xi[2] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[2]), xi[2]);
    xi[3] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[3]), xi[3]);

    __m256i x1 = _mm256_packs_epi16(xi[0], xi[1]);
    x1 = _mm256_permute4x64_epi64(x1, 0xd8);  // 3,1,2,0
    __m256i x2 = _mm256_packs_epi16(xi[2], xi[3]);
    x2 = _mm256_permute4x64_epi64(x2, 0xd8);  // 3,1,2,0

    __m256i yy[4];
    yy[0] = _zero;
    yy[1] = _zero;
    yy[2] = _zero;
    yy[3] = _zero;


    EVAL_YMM_0(x1)
    EVAL_YMM_1(x1)
    EVAL_YMM_0(x2)
    EVAL_YMM_1(x2)
    REDUCE_(yy)

    __m256i xixj[32];

    calc_quadratic_terms_tri_32_avx2((unsigned char *)&xixj[0], x1, mask_2114, mask_reduce);
    for (i = 0 ; i < 16 ; i+=2) {
        EVAL_YMM_0(xixj[i])
        EVAL_YMM_1(xixj[i])
        EVAL_YMM_0(xixj[i+1])
        EVAL_YMM_1(xixj[i+1])
        REDUCE_(yy)
    }
    EVAL_YMM_0(xixj[16])
    REDUCE_(yy)

    calc_quadratic_terms_tri_32_avx2((unsigned char *)&xixj[0], x2, mask_2114, mask_reduce);
    for (i = 0 ; i < 16 ; i+=2) {
        EVAL_YMM_0(xixj[i])
        EVAL_YMM_1(xixj[i])
        EVAL_YMM_0(xixj[i+1])
        EVAL_YMM_1(xixj[i+1])
        REDUCE_(yy)
    }
    EVAL_YMM_0(xixj[16])
    REDUCE_(yy)

    calc_quadratic_terms_rec_32_avx2((unsigned char *)&xixj[0], x1, x2, mask_2114, mask_reduce);
    for (i = 0; i < 32; i+=2) {
        EVAL_YMM_0(xixj[i])
        EVAL_YMM_1(xixj[i])
        EVAL_YMM_0(xixj[i+1])
        EVAL_YMM_1(xixj[i+1])
        REDUCE_(yy)
    }

    yy[0] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[0]), yy[0]);
    yy[1] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[1]), yy[1]);
    yy[2] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[2]), yy[2]);
    yy[3] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[3]), yy[3]);

    for (i = 0; i < 4; ++i) {
        _mm256_storeu_si256((__m256i*)(fx+i*16), yy[i]);
    }
}

void G(gf31 *fx, const gf31 *x, const gf31 *y, const signed char *F)
{
    int i;

    __m256i mask_2114 = _mm256_set1_epi32(2114*65536 + 2114);
    __m256i mask_reduce = _mm256_srli_epi16(_mm256_cmpeq_epi16(mask_2114, mask_2114), 11);
    __m256i _zero = _mm256_setzero_si256();

    __m256i xi[4];
    xi[0] = _mm256_loadu_si256((__m256i const *) (x));
    xi[1] = _mm256_loadu_si256((__m256i const *) (x+16));
    xi[2] = _mm256_loadu_si256((__m256i const *) (x+32));
    xi[3] = _mm256_loadu_si256((__m256i const *) (x+48));

    xi[0] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[0]), xi[0]);
    xi[1] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[1]), xi[1]);
    xi[2] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[2]), xi[2]);
    xi[3] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[3]), xi[3]);

    __m256i x1 = _mm256_packs_epi16(xi[0], xi[1]);
    x1 = _mm256_permute4x64_epi64(x1, 0xd8);  // 3,1,2,0
    __m256i x2 = _mm256_packs_epi16(xi[2], xi[3]);
    x2 = _mm256_permute4x64_epi64(x2, 0xd8);  // 3,1,2,0

    xi[0] = _mm256_loadu_si256((__m256i const *) (y));
    xi[1] = _mm256_loadu_si256((__m256i const *) (y+16));
    xi[2] = _mm256_loadu_si256((__m256i const *) (y+32));
    xi[3] = _mm256_loadu_si256((__m256i const *) (y+48));

    xi[0] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[0]), xi[0]);
    xi[1] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[1]), xi[1]);
    xi[2] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[2]), xi[2]);
    xi[3] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_zero, xi[3]), xi[3]);

    __m256i y1 = _mm256_packs_epi16(xi[0], xi[1]);
    y1 = _mm256_permute4x64_epi64(y1, 0xd8);  // 3,1,2,0
    __m256i y2 = _mm256_packs_epi16(xi[2], xi[3]);
    y2 = _mm256_permute4x64_epi64(y2, 0xd8);  // 3,1,2,0

    __m256i yy[4];
    yy[0] = _zero;
    yy[1] = _zero;
    yy[2] = _zero;
    yy[3] = _zero;

    F += 64*64;

    __m256i xixj[32];

    calc_quadratic_terms_tri_32_g_avx2((unsigned char *)&xixj[0], x1, y1, mask_2114, mask_reduce);
    for (i = 0 ; i < 16 ; i+=2) {
        EVAL_YMM_0(xixj[i])
        EVAL_YMM_1(xixj[i])
        EVAL_YMM_0(xixj[i+1])
        EVAL_YMM_1(xixj[i+1])
        REDUCE_(yy)
    }
    EVAL_YMM_0(xixj[16])
    REDUCE_(yy)

    calc_quadratic_terms_tri_32_g_avx2((unsigned char *)&xixj[0], x2, y2, mask_2114, mask_reduce);
    for (i = 0 ; i < 16 ; i+=2) {
        EVAL_YMM_0(xixj[i])
        EVAL_YMM_1(xixj[i])
        EVAL_YMM_0(xixj[i+1])
        EVAL_YMM_1(xixj[i+1])
        REDUCE_(yy)
    }
    EVAL_YMM_0(xixj[16])
    REDUCE_(yy)

    calc_quadratic_terms_rec_32_g_avx2((unsigned char *)&xixj[0], x1, x2, y1, y2, mask_2114, mask_reduce);
    for (i = 0; i < 32; i+=2) {
        EVAL_YMM_0(xixj[i])
        EVAL_YMM_1(xixj[i])
        EVAL_YMM_0(xixj[i+1])
        EVAL_YMM_1(xixj[i+1])
        REDUCE_(yy)
    }

    yy[0] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[0]), yy[0]);
    yy[1] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[1]), yy[1]);
    yy[2] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[2]), yy[2]);
    yy[3] = _mm256_add_epi16(mask_reduce&_mm256_cmpgt_epi16(_mm256_setzero_si256(), yy[3]), yy[3]);

    for (i = 0; i < 4; ++i) {
        _mm256_storeu_si256((__m256i*)(fx+i*16), yy[i]);
    }
}
