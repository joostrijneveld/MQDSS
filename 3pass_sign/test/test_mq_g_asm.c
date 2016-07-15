#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../mq.h"
#include "../params.h"

int test_MQ_asm()
{
    unsigned char x[N_BYTES];
    unsigned char F[F_BYTES];
    unsigned char fx_rect[M_BYTES];
    unsigned char fx_asm[M_BYTES];

    randombytes(x, N_BYTES);
    randombytes(F, F_BYTES);

    memset(fx_rect, 1, M_BYTES);
    memset(fx_asm, 2, M_BYTES);

    MQ(fx_rect, x, F);
    MQ_asm(fx_asm, x, F);

    return memcmp(fx_asm, fx_rect, M_BYTES);
}

int test_G_asm()
{
    unsigned char x[N_BYTES];
    unsigned char y[N_BYTES];
    unsigned char F[F_BYTES];
    unsigned char gx[M_BYTES];
    unsigned char gx_asm[M_BYTES];

    randombytes(x, N_BYTES);
    randombytes(y, N_BYTES);
    randombytes(F, F_BYTES);

    memset(gx, 0, M_BYTES);
    memset(gx_asm, 0, M_BYTES);

    G(gx, x, y, F);
    G_asm(gx_asm, x, y, F);

    return memcmp(gx_asm, gx, M_BYTES);
}

int main()
{
    int r1, r2;

    r1 = test_MQ_asm();
    printf("Comparing MQ and MQ_asm.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r2 = test_G_asm();
    printf("Comparing G and G_asm.. ");
    printf(r2 ? "FAIL!" : "Success.");
    printf("\n");

    return r1 | r2;
}