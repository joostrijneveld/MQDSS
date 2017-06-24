#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../mq.h"
#include "../params.h"
#include "../gf31.h"

int test_consistency()
{
    gf31 x[N];
    gf31 y[N];
    gf31 fx[M];
    gf31 fx2[M];
    signed char F[F_LEN];
    int i;
    unsigned char seed[SEED_BYTES];

    randombytes(seed, SEED_BYTES);
    gf31_nrand(x, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand(y, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand_schar(F, F_LEN, seed, SEED_BYTES);

    randombytes((unsigned char *)fx, M * sizeof(gf31));

    MQ(fx, x, F);
    for (i = 0; i < 100; i++) {
        randombytes((unsigned char *)fx2, M * sizeof(gf31));
        MQ(fx2, x, F);
        if (memcmp(fx, fx2, M)) {
            return 1;
        }
    }

    G(fx, x, y, F);
    for (i = 0; i < 100; i++) {
        randombytes((unsigned char *)fx2, M * sizeof(gf31));
        G(fx2, x, y, F);
        if (memcmp(fx, fx2, M)) {
            return 1;
        }
    }

    return 0;
}

int main()
{
    printf("Testing if MQ and G are deterministic.. ");
    fflush(stdout);
    int r = test_consistency();
    printf(r ? "FAIL!" : "Success.");
    printf("\n");
    return r;
}
