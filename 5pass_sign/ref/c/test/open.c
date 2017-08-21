#include "../api.h"
#include "../sign.h"
#include <stdio.h>

#define MLEN 32

int main() {
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sm[CRYPTO_BYTES + MLEN];
    unsigned char m[CRYPTO_BYTES + MLEN];
    unsigned long long mlen;

    fread(pk, 1, CRYPTO_PUBLICKEYBYTES, stdin);
    fread(sm, 1, CRYPTO_BYTES + MLEN, stdin);

    return crypto_sign_open(m, &mlen, sm, CRYPTO_BYTES + MLEN, pk);
}
