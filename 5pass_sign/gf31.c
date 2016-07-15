#include <assert.h>
#include "params.h"
#include "KeccakHash.h"
#include "gf31.h"

int gf31_is31(gf31 x)
{
    /* This function is typically applied to accumulators before hashing, and
    the accumulator is not secret after it is computed.. Maybe we do not need
    to be constant time here. TODO Can we guarantee that the difference between
    '31' and '0' does not reveal anything here? TODO Also, maybe we can do
    something smarter than a very general bit-folding approach? */
    x = ~(x ^ 31);
    x &= x >> 8;
    x &= x >> 4;
    x &= x >> 2;
    x &= x >> 1;
    return (x & 1);
}

gf31 gf31_unique(gf31 x)
{
    char a = gf31_is31(x);
    return x * (1 - a);
}

void vgf31_unique(gf31 *out, gf31 *in, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        out[i] = gf31_unique(in[i]);
    }
}

gf31 gf31_shorten(gf31 x)
{
    gf31 t;

    t = x & 31;
    x >>= 5;
    x += t;
    return x;
}

void vgf31_shorten(gf31 *out, gf31 *in, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        out[i] = gf31_shorten(in[i]);
    }
}

gf31 gf31_signed_shorten(gf31 x)
{
    gf31 t;
    x += 16;
    t = x & 31;
    x >>= 5;
    x += t;
    x -= 16;
    return x;
}

void vgf31_signed_shorten(gf31 *out, gf31 *in, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        out[i] = gf31_signed_shorten(in[i]);
        // Since we would only do this to get in range [-16, 15]..
        assert(out[i] >= -16 && out[i] <= 15);
    }
}

void gf31_nrand(gf31 *out, const int len, const unsigned char *seed, const int seedlen)
{
    int i = 0, j;
    const int buflen = 128;
    unsigned char buf[buflen];
    Keccak_HashInstance keccak;

    Keccak_HashInitialize_SHAKE128(&keccak);
    Keccak_HashUpdate(&keccak, seed, seedlen);
    Keccak_HashFinal(&keccak, buf);

    // TODO should not only use the 5 low bits of a byte!
    while (i < len) {
        Keccak_HashSqueeze(&keccak, buf, buflen * 8);
        for (j = 0; j < buflen && i < len; j++) {
            if ((buf[j] & 31) != 31) {
                out[i] = (buf[j] & 31) - 15;
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

// TODO It should be possible to pack [-15, 15] rather than [0, 30].
// That would hugely reduce the need to convert elements from [0, 31], since
// that means we can use signed_shorten a lot more often.

/* Packs an array of GF31 elements from gf31's to concatenated 5-bit values.
This function assumes that there is sufficient space available to unpack.
Can perform in-place. */
void gf31_npack(unsigned char *out, const gf31 *in, const int n)
{
    int i = 0;
    int j;
    int d = 3;

    for (j = 0; j < n; j++) {
        assert(in[j] >= 0 && in[j] < 31);
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
