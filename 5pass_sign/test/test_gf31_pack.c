#include <stdio.h>
#include <string.h>
#include "../randombytes.h"
#include "../gf31.h"
#include "../params.h"

int test_pack_unpack()
{
    gf31 x[N];
    unsigned char x_packed[NPACKED_BYTES];
    gf31 y[N];
    int i;

    randombytes((unsigned char *)x, N * sizeof(gf31));
    for (i = 0; i < N; i++) {
        x[i] = (x[i] & 31) % 31;
    }

    gf31_npack(x_packed, x, N);
    gf31_nunpack(y, x_packed, N);

    return memcmp(x, y, N);
}

int main()
{
    int r1;

    r1 = test_pack_unpack();
    printf("Testing if unpack(pack(x)) == x.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    return r1;
}
