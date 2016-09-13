#include <stdio.h>
#include <string.h>
#include "../gf31.h"
#include "../params.h"

int test_gf31shorten_unique()
{
    gf31 x[N];
    int i, j;

    // The function is expected to work for 15 bits.
    for (i = 0; i < 32768; i++) {
        for (j = 0; j < N; j++) {
            x[j] = i;
        }
        vgf31_shorten_unique(x, x);

        for (j = 0; j < N; j++) {
            if (x[j] != (i % 31)) {
                return 1;
            }
        }
    }
    return 0;
}

int main()
{
    int r1;

    r1 = test_gf31shorten_unique();
    printf("Testing vgf31_shorten_unique(x) = x %% 31.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    return r1;
}
