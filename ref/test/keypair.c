#include <stdio.h>
#include "../api.h"

int main() {
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];

    crypto_sign_keypair(pk, sk);

    fwrite(pk, 1, CRYPTO_PUBLICKEYBYTES, stdout);
    fwrite(sk, 1, CRYPTO_SECRETKEYBYTES, stdout);

    fclose(stdout);
}
