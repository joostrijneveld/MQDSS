#include <stdio.h>
#include "../api.h"

#define MLEN 32

int main() {
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char m[MLEN];
    unsigned char sm[CRYPTO_BYTES + MLEN];
    unsigned long long smlen;

    fread(sk, 1, CRYPTO_SECRETKEYBYTES, stdin);
    fread(m, 1, MLEN, stdin);
    crypto_sign(sm, &smlen, m, MLEN, sk);

    fwrite(sm, 1, CRYPTO_BYTES + MLEN, stdout);
    fclose(stdout);
}
