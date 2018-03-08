#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../api.h"
#include "../params.h"

#define MLEN 32

int main()
{
    unsigned char pk[PK_BYTES];
    unsigned char sk[SK_BYTES];
    unsigned char m[MLEN];
    unsigned char m_out[SIG_LEN + MLEN];
    unsigned char sm[SIG_LEN + MLEN];
    unsigned long long smlen;
    unsigned long long mlen;
    int i;

    printf("Testing signature correctness..");
    fflush(stdout);

    for (i = 0; i < 100; i++) {
        crypto_sign_keypair(pk, sk);
        randombytes(m, MLEN);
        crypto_sign(sm, &smlen, m, MLEN, sk);

        if (smlen != SIG_LEN + MLEN) {
            printf("  FAIL! Signature length does not match (%llu / %d)!\n",
                   smlen, SIG_LEN + MLEN);
            return 1;
        }
        else if (crypto_sign_open(m_out, &mlen, sm, smlen, pk)) {
            printf("  FAIL! Signature did not verify correctly!\n");
            return 1;
        }
        else if (mlen != MLEN) {
            printf("  FAIL! Message length does not match!\n");
            return 1;
        }
        else if (memcmp(m, m_out, MLEN)) {
            printf("  FAIL! Message does not match!\n");
            return 1;
        }
    }
    printf("  Success.\n");
    return 0;
}
