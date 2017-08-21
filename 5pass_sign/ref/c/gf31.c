#include <assert.h>
#include <immintrin.h>
#include <stdint.h>
#include "params.h"
#include "fips202.h"
#include "gf31.h"

void vgf31_unique(gf31 *out, gf31 *in)
{
    int i;

    for (i = 0; i < N; i++) {
        out[i] = (1 - (in[i] == 31)) * in[i];
    }
}

void vgf31_shorten_unique(gf31 *out, gf31 *in)
{
    int i;

    for (i = 0; i < N; i++) {
        out[i] = in[i] % 31;
    }
}

void gf31_nrand(gf31 *out, const int len, const unsigned char *seed, const int seedlen)
{
    int i = 0, j;
    uint64_t shakestate[25] = {0};
    unsigned char shakeblock[SHAKE128_RATE];

    shake128_absorb(shakestate, seed, seedlen);

    // TODO should not only use the 5 low bits of a byte!
    while (i < len) {
        shake128_squeezeblocks(shakeblock, 1, shakestate);
        for (j = 0; j < SHAKE128_RATE && i < len; j++) {
            if ((shakeblock[j] & 31) != 31) {
                out[i] = (shakeblock[j] & 31);
                i++;
            }
        }
    }
}

void gf31_nrand_schar(signed char *out, const int len, const unsigned char *seed, const int seedlen)
{
    int i = 0, j;
    uint64_t shakestate[25] = {0};
    unsigned char shakeblock[SHAKE128_RATE];

    shake128_absorb(shakestate, seed, seedlen);

    // TODO should not only use the 5 low bits of a byte!
    while (i < len) {
        shake128_squeezeblocks(shakeblock, 1, shakestate);
        for (j = 0; j < SHAKE128_RATE && i < len; j++) {
            if ((shakeblock[j] & 31) != 31) {
                out[i] = (shakeblock[j] & 31) - 15;
                i++;
            }
        }
    }
}

/* Unpacks an array of packed GF31 elements to one element per gf31.
This function assumes that there is sufficient empty space available at the end
of the array to unpack. Can perform in-place. */
void gf31_nunpack(gf31 *out, const unsigned char *in, const int n)
{
    int i;
    int j = ((n * 5) >> 3) - 1;
    int d = 0;

    for (i = n-1; i >= 0; i--) {
        out[i] = (in[j] >> d) & 31;
        d += 5;
        if (d > 8) {
            d -= 8;
            j--;
            out[i] ^= (in[j] << (5 - d)) & 31;
        }
    }
}

/* Packs an array of GF31 elements from gf31's to concatenated 5-bit values.
This function assumes that there is sufficient space available to unpack.
Can perform in-place. */
void gf31_npack(unsigned char *out, const gf31 *in, const int n)
{
    int i = 0;
    int j;
    int d = 3;

    for (j = 0; j < n; j++) {
        assert(in[j] < 31);
    }

    for (j = 0; j < n; j++) {
        if (d < 0) {
            d += 8;
            out[i] = (out[i] & (255 << (d - 3))) |
                      ((in[j] >> (8 - d)) & ~(255 << (d - 3)));
            i++;
        }
        out[i] = (out[i] & ~(31 << d)) | ((in[j] << d) & (31 << d));
        d -= 5;
    }
}
