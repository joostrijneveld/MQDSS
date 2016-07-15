#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include "tools.h"
#include "../randombytes.h"
#include "../mq.h"
#include "../params.h"

#define MEASURE_ASMROUNDS 10000  // More rounds == more meaningful average.
#define MEASURE_ROUNDS 100

int main()
{
    unsigned char x[N_BYTES];
    unsigned char y[N_BYTES];
    unsigned char fx[M_BYTES];
    unsigned char F[F_BYTES];
    unsigned long long cycles[MEASURE_ASMROUNDS];
    unsigned long long t;
    int i;

    randombytes(x, N_BYTES);
    randombytes(y, N_BYTES);
    randombytes(F, F_BYTES);

    printf("Benchmarking MQ_asm..\n");
    memset(cycles, 0, MEASURE_ASMROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ASMROUNDS; i++) {
        t = __rdtsc();
        MQ_asm(fx, x, F);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ASMROUNDS);

    printf("Benchmarking MQ..\n");
    memset(cycles, 0, MEASURE_ROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ROUNDS; i++) {
        t = __rdtsc();
        MQ(fx, x, F);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ROUNDS);

    printf("Benchmarking G_asm..\n");
    memset(cycles, 0, MEASURE_ASMROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ASMROUNDS; i++) {
        t = __rdtsc();
        G_asm(fx, x, y, F);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ASMROUNDS);

    printf("Benchmarking G..\n");
    memset(cycles, 0, MEASURE_ROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ROUNDS; i++) {
        t = __rdtsc();
        G(fx, x, y, F);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ROUNDS);

    return 0;
}
