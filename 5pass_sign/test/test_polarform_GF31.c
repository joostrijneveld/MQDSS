#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../mq.h"
#include "../params.h"
#include "../gf31.h"

int polarform_GF31_eq(gf31 *x, gf31 *y, signed char *F)
{
    gf31 fx[M];
    gf31 gx[M];
    gf31 buf_n[N];
    gf31 buf_m[M];
    int i;

    memset(fx, 1, M * sizeof(gf31));
    memset(gx, 2, M * sizeof(gf31));
    memset(buf_n, 3, N * sizeof(gf31));
    memset(buf_m, 4, M * sizeof(gf31));

    G(gx, x, y, F);

    for (i = 0; i < N; i++) {
        buf_n[i] = (x[i] + y[i]) % 31;
    }

    MQ(fx, buf_n, F);
    MQ(buf_m, x, F);

    for (i = 0; i < M; i++) {
        fx[i] = (31 + fx[i] - buf_m[i]) % 31;
    }

    MQ(buf_m, y, F);

    for (i = 0; i < M; i++) {
        fx[i] = (31 + fx[i] - buf_m[i]) % 31;
    }

    // To be able to bitwise compare:
    for (i = 0; i < M; i++) {
        gx[i] = gx[i] % 31;
    }

    return memcmp(fx, gx, M);
}

/* By definition of G, G(x, y) = F(x + y) - F(x) - F(y) needs to hold. */
int test_polarform_GF31()
{
    gf31 x[N];
    gf31 y[N];
    signed char F[F_LEN];
    int i;
    unsigned char seed[SEED_BYTES];

    randombytes(seed, SEED_BYTES);
    gf31_nrand(x, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand(y, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand_schar(F, F_LEN, seed, SEED_BYTES);

    if (polarform_GF31_eq(x, y, F)) {
        return 1;
    }

    for (i = 0; i < N; i++) {
        x[i] = 30;
        y[i] = 30;
    }

    for (i = 0; i < F_LEN; i++) {
        F[i] = -15;
    }

    if (polarform_GF31_eq(x, y, F)) {
        return 1;
    }

    return 0;
}

int main()
{
    int r = test_polarform_GF31();
    printf("Testing polar form equation.. ");
    printf(r ? "FAIL!" : "Success.");
    printf("\n");
    return r;
}
