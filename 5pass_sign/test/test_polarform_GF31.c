#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../mq.h"
#include "../params.h"
#include "../gf31.h"

int polarform_GF2_eq(gf31 *x, gf31 *y, gf31 *F)
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

    G_asm(gx, x, y, F);

    for (i = 0; i < N; i++) {
        buf_n[i] = gf31_shorten(x[i] + y[i]);
    }

    MQ_asm(fx, buf_n, F);
    MQ_asm(buf_m, x, F);

    for (i = 0; i < M; i++) {
        fx[i] = gf31_shorten(fx[i] - buf_m[i]);
    }

    MQ_asm(buf_m, y, F);

    for (i = 0; i < M; i++) {
        fx[i] = gf31_shorten(fx[i] - buf_m[i]);
    }

    // To be able to bitwise compare:
    for (i = 0; i < M; i++) {
        fx[i] = gf31_shorten(gf31_shorten(fx[i]));
        if (fx[i] == 31) fx[i] = 0;
    }
    for (i = 0; i < M; i++) {
        gx[i] = gf31_shorten(gf31_shorten(gx[i]));
        if (gx[i] == 31) gx[i] = 0;
    }

    return memcmp(fx, gx, M);
}

/* By definition of G, G(x, y) = F(x + y) - F(x) - F(y) needs to hold. */
int test_polarform_GF2()
{
    gf31 x[N];
    gf31 y[N];
    gf31 F[F_LEN];
    int i;
    unsigned char seed[SEED_BYTES];

    randombytes(seed, SEED_BYTES);
    gf31_nrand(x, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand(y, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand(F, F_LEN, seed, SEED_BYTES);

    if (polarform_GF2_eq(x, y, F)) {
        return 1;
    }

    for (i = 0; i < N; i++) {
        x[i] = -16;
        y[i] = -16;
    }

    for (i = 0; i < F_LEN; i++) {
        F[i] = -15;
    }

    if (polarform_GF2_eq(x, y, F)) {
        return 1;
    }

    return 0;
}

int main()
{
    int r = test_polarform_GF2();
    printf("Testing polar form equation.. ");
    printf(r ? "FAIL!" : "Success.");
    printf("\n");
    return r;
}