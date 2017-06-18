#include <stdio.h>
#include <string.h>
#include "../gf31.h"
#include "../params.h"
#include "../randombytes.h"

#define LEN 1024

int test_gf31_nrand()
{
    int i;
    unsigned char seed[SEED_BYTES];
    gf31 x[LEN];

    randombytes(seed, SEED_BYTES);
    gf31_nrand(x, LEN, seed, SEED_BYTES);

    for (i = 0; i < LEN; i++) {
        if (x[i] > 30) {
            return 1;
        }
    }
    return 0;
}

int test_gf31_nrand_schar()
{
    int i;
    unsigned char seed[SEED_BYTES];
    signed char x[LEN];

    randombytes(seed, SEED_BYTES);
    gf31_nrand_schar(x, LEN, seed, SEED_BYTES);

    for (i = 0; i < LEN; i++) {
        if (x[i] > 15 || x[i] < -15) {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int r1, r2;

    r1 = test_gf31_nrand();
    printf("Testing range [0, 30] of gf31_nrand.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r2 = test_gf31_nrand_schar();
    printf("Testing range [-15, 15] of gf31_nrand_schar.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    return r1 | r2;
}
