#include <stdio.h>
#include <string.h>
#include "../gf31.h"

int test_gf31shorten()
{
    gf31 x;
    int i;

    for (i = -32768; i < 32768; i++) {
        x = i;
        x = gf31_shorten(gf31_shorten(gf31_shorten(x)));
        if (x == 31) {
            x = 0;  // Since shorten does not reduce < 31, but <= 31.
        }
        if (! (x == (i % 31) || (x == (i % 31) + 31 && (i % 31) < 0))) {
            return 1;
        }
    }
    return 0;
}

int test_shorten_toposdomain()
{
    gf31 x;
    int i;

    for (i = -16; i <= 15; i++) {
        x = gf31_shorten(i);
        if (x < 0) {
            return 1;
        }
    }
    return 0;
}

int test_signed_shorten()
{
    gf31 x;
    int i;

    for (i = -32; i <= 32; i++) {
        x = gf31_signed_shorten(i);
        if (x < -16 || x > 15) {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int r1, r2, r3;

    r1 = test_gf31shorten();
    printf("Testing gf31_shorten^3(x) ≈ x %% 31.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r2 = test_shorten_toposdomain();
    printf("Testing if gf31_shorten([-16, 15]) >= 0.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r3 = test_signed_shorten();
    printf("Testing if gf31_signed_shorten([-32, 32]) in [-16, 15].. ");
    printf(r3 ? "FAIL!" : "Success.");
    printf("\n");

    return r1 | r2 | r3;
}