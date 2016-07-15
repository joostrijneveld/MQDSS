#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../sign.h"
#include "../params.h"
#include "../mq.h"

extern void ROL256_asm(unsigned char *out, const unsigned char *in);

int test_ROL256_asm_circular()
{
    unsigned char x[N_BYTES];
    unsigned char x_bak[N_BYTES];
    int i;

    randombytes(x, N_BYTES);
    memcpy(x_bak, x, N_BYTES);

    for (i = 0; i < 256; i++) {
        ROL256_asm(x, x);
    }

    return memcmp(x, x_bak, N_BYTES);
}

int test_ROL256_asm()
{
    unsigned char x[N_BYTES];
    unsigned char x_out[N_BYTES];
    int i;

    randombytes(x, N_BYTES);

    for (i = 0; i < 256; i++) {
        ROL_NBYTES(x_out, x);
        ROL256_asm(x, x);

        if (memcmp(x, x_out, N_BYTES)) {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int r1, r2;

    r1 = test_ROL256_asm();
    printf("Comparing ROL_NBYTES and ROL256_asm.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r2 = test_ROL256_asm_circular();
    printf("Testing if ROL256_asm is circular.. ");
    printf(r2 ? "FAIL!" : "Success.");
    printf("\n");

    return r1 | r2;
}