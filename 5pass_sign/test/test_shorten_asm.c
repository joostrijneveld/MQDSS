#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../sign.h"
#include "../params.h"
#include "../mq.h"

extern void signed_shorten_gf31_asm(gf31 *out, const gf31 *in);
extern void shorten_gf31_asm(gf31 *out, const gf31 *in);

int test_shorten_gf31_asm()
{
    gf31 x[16];
    gf31 y[16];
    gf31 z;
    int i, j;

    for (i = -32768; i < 32768; i++) {
        for (j = 0; j < 16; j++) {
            x[j] = i;
        }
        z = gf31_shorten(x[0]);
        shorten_gf31_asm(y, x);
        for (j = 0; j < 16; j++) {
            if (z != y[j]) {
                return 1;
            }
        }
    }
    return 0;
}

int test_signed_shorten_gf31_asm()
{
    gf31 x[16];
    gf31 y[16];
    gf31 z;
    int i, j;

    for (i = -32768; i < 32768; i++) {
        for (j = 0; j < 16; j++) {
            x[j] = i;
        }
        z = gf31_signed_shorten(x[0]);
        signed_shorten_gf31_asm(y, x);
        for (j = 0; j < 16; j++) {
            if (z != y[j]) {
                return 1;
            }
        }
    }
    return 0;
}

int main()
{
    int r1, r2;

    r1 = test_shorten_gf31_asm();
    printf("Comparing shorten_gf31 and shorten_gf31_asm.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r2 = test_signed_shorten_gf31_asm();
    printf("Comparing signed_shorten_gf31 and signed_shorten_gf31_asm.. ");
    printf(r2 ? "FAIL!" : "Success.");
    printf("\n");

    return r1 | r2;
}