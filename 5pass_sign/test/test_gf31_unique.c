#include <stdio.h>
#include <string.h>
#include "../gf31.h"
#include "../params.h"

int test_vgf31_unique()
{
    int i, j;
    gf31 x[N];

    for (i = 0; i <= 31; i++) {
        for (j = 0; j < N; j++) {
            x[j] = i;
        }
        vgf31_unique(x, x);

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

    r1 = test_vgf31_unique();
    printf("Testing vgf31_unique.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    return r1;
}
