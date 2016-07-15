#include <stdio.h>
#include <string.h>
#include "../gf31.h"

int test_gf31_x_is_31()
{
    int i;
    gf31 x;
    for (i = -32768; i < 32768; i++) {
        x = i;
        if ((gf31_is31(x) && x != 31) || (!gf31_is31(x) && x == 31)) {
            return 1;
        }
    }
    return 0;
}

int test_gf31_unique()
{
    gf31 x;
    int i;

    for (i = 0; i <= 31; i++) {
        x = i;
        x = gf31_unique(x);
        if (x < 0 || x >= 31) {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int r1, r2;

    r1 = test_gf31_x_is_31();
    printf("Testing gf31_is31.. ");
    printf(r1 ? "FAIL!" : "Success.");
    printf("\n");

    r2 = test_gf31_unique();
    printf("Testing gf31_unique in general.. ");
    printf(r2 ? "FAIL!" : "Success.");
    printf("\n");

    return r1 | r2;
}