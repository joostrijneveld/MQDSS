#include "../params.h"
#include "../mq.h"
#include <stdio.h>

int main() {
    unsigned char x_packed[NPACKED_BYTES];
    gf31 x[N];
    gf31 y[M];
    unsigned char y_packed[MPACKED_BYTES];
    unsigned char fseed[SEED_BYTES];
    signed char F[F_LEN];
    int i;

    fread(x_packed, 1, NPACKED_BYTES, stdin);
    fread(fseed, 1, SEED_BYTES, stdin);

    gf31_nrand_schar(F, F_LEN, fseed, SEED_BYTES);
    gf31_nunpack(x, x_packed, N);

    for (i = 0; i < N; i++) {
        // Need to reduce, since we assume random input
        x[i] = x[i] % 31;
    }

    MQ(y, x, F);

    for (i = 0; i < N; i++) {
        // Need to reduce, since MQ is not guaranteed to reduce
        y[i] = y[i] % 31;
    }

    gf31_npack(y_packed, y, M);

    fwrite(y_packed, 1, MPACKED_BYTES, stdout);
    fclose(stdout);
}
