#include <stdio.h>

#include "../api.h"

int main()
{
    printf("Signature size: %d bytes\n", CRYPTO_BYTES);
    printf("Public key size: %d bytes\n", CRYPTO_PUBLICKEYBYTES);
    printf("Private key size: %d bytes\n", CRYPTO_SECRETKEYBYTES);

    return 0;
}
