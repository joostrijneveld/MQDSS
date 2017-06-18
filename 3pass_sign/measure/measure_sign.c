#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include "tools.h"
#include "../randombytes.h"
#include "../sign.h"
#include "../params.h"

#define MLEN 32
#define MEASURE_ROUNDS 200  // More rounds == more meaningful average.

#include "tools.c"

int main()
{
    unsigned char pk[PK_BYTES];
    unsigned char sk[SK_BYTES];
    unsigned char m[MLEN];
    unsigned char m_out[SIG_LEN + MLEN];
    unsigned char sm[SIG_LEN + MLEN];
    unsigned long long smlen;
    unsigned long long mlen;
    unsigned long long cycles[MEASURE_ROUNDS];
    unsigned long long t;
    int result;
    int i;

    printf("Benchmarking key generation..\n");
    memset(cycles, 0, MEASURE_ROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ROUNDS; i++) {
        t = __rdtsc();
        crypto_sign_keypair(pk, sk);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ROUNDS);

    randombytes(m, MLEN);

    printf("Benchmarking signing..\n");
    memset(cycles, 0, MEASURE_ROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ROUNDS; i++) {
        t = __rdtsc();
        crypto_sign(sm, &smlen, m, MLEN, sk);
        cycles[i] = __rdtsc() - t;
    }
    analyse_cycles(cycles, MEASURE_ROUNDS);

    printf("Benchmarking verifying..\n");
    memset(cycles, 0, MEASURE_ROUNDS * sizeof(unsigned long long));
    for (i = 0; i < MEASURE_ROUNDS; i++) {
        t = __rdtsc();
        result = crypto_sign_open(m_out, &mlen, sm, smlen, pk);
        cycles[i] = __rdtsc() - t;
        if (result) {
            printf("  Signature did not verify correctly!\n");
            return 1;
        }
    }
    analyse_cycles(cycles, MEASURE_ROUNDS);

    printf("Signature size: %d bytes (%0.2f KB)\n", SIG_LEN, SIG_LEN / 1024.0);
    return 0;
}
