#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../sign.h"
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

    printf("Testing signature correctness..");

    crypto_sign_keypair(pk, sk);
    randombytes(m, MLEN);
    crypto_sign(sm, &smlen, m, MLEN, sk);

    if (smlen != SIG_LEN + MLEN) {
        printf("  Signature length does not match (%llu / %d)!\n",
               smlen, SIG_LEN + MLEN);
        return 1;
    }
    else if (crypto_sign_open(m_out, &mlen, sm, smlen, pk)) {
        printf("  Signature did not verify correctly!\n");
        return 1;
    }
    else if (mlen != MLEN) {
        printf("  Message length does not match!\n");
        return 1;
    }
    else if (memcmp(m, m_out, MLEN)) {
        printf("  Message does not match!\n");
        return 1;
    }
    printf("  Success.\n");
    return 0;
}
