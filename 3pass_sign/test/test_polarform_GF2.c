#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../mq.h"
#include "../params.h"

/* By definition of G, G(x, y) = F(x + y) - F(x) - F(y) needs to hold. */
int test_polarform_GF2()
{
    unsigned char x[N_BYTES];
    unsigned char y[N_BYTES];
    unsigned char F[F_BYTES];
    unsigned char buf_n[N_BYTES];
    unsigned char buf_m[M_BYTES];
    unsigned char fx[M_BYTES];
    unsigned char gx[M_BYTES];
    int i;

    randombytes(x, N_BYTES);
    randombytes(y, N_BYTES);
    randombytes(F, F_BYTES);

    G(gx, x, y, F);

    for (i = 0; i < N_BYTES; i++) {
        buf_n[i] = x[i] ^ y[i];
    }

    MQ(fx, buf_n, F);
    MQ(buf_m, x, F);

    for (i = 0; i < M_BYTES; i++) {
        fx[i] ^= buf_m[i];
    }

    MQ(buf_m, y, F);

    for (i = 0; i < M_BYTES; i++) {
        fx[i] ^= buf_m[i];
    }

    return memcmp(fx, gx, M_BYTES);
}

int main()
{
    int r = test_polarform_GF2();
    printf("Testing polar form equation.. ");
    printf(r ? "FAIL!" : "Success.");
    printf("\n");
    return r;
}