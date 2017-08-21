#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include "tools.h"
#include "../randombytes.h"
#include "../gf31.h"
#include "../mq.h"
#include "../params.h"

#define MEASURE_ASMROUNDS 100000  // More rounds == more meaningful average.
#define MEASURE_ROUNDS 5

#include "tools.c"

int main()
{
    gf31 x[N];
    gf31 fx[M];
    gf31 y[N];
    signed char F[F_LEN];
    int i;
    unsigned char seed[SEED_BYTES];
    unsigned long long cycles[MEASURE_ASMROUNDS];
    unsigned long long t;

    randombytes(seed, SEED_BYTES);
    gf31_nrand(x, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand(y, N, seed, SEED_BYTES);
    randombytes(seed, SEED_BYTES);
    gf31_nrand_schar(F, F_LEN, seed, SEED_BYTES);

    printf("Benchmarking MQ..\n");
    memset(cycles, 0, MEASURE_ASMROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ASMROUNDS; i++) {
        t = __rdtsc();
        MQ(fx, x, F);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ASMROUNDS);

    printf("Benchmarking G..\n");
    memset(cycles, 0, MEASURE_ASMROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ASMROUNDS; i++) {
        t = __rdtsc();
        G(fx, x, y, F);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ASMROUNDS);

    return 0;
}
