#include <stdio.h>
#include <stdlib.h>
#include "tools.h"

#define MEASURE_OFFSET 15  // High/low percentage that should be ignored.

int ullcmp(const void *p1, const void *p2)
{
    return (*(unsigned long long*)p1) - (*(unsigned long long*)p2);
}

static void print_thousands(unsigned long long x, int depth)
{
    if (depth > 0) {
        print_thousands(x / 1000, depth - 1);
        printf(" ");
    }
    if (x > 1000) {
        printf("%03llu", x % 1000);
    }
    else if (x > 0) {
        printf("%3llu", x % 1000);
    }
    else {
        printf("   ");
    }
}

void analyse_cycles(unsigned long long *cycles, int n)
{
    int i;
    unsigned long long mean = 0;
    unsigned long long median = 0;
    unsigned long long mean_offset = 0;

    qsort(cycles, n, sizeof(unsigned long long), ullcmp);
    for (i = 0; i < n; i++) {
        mean += cycles[i];
    }
    mean /= n;

    for (i = MEASURE_OFFSET * n / 100; i < (100 - MEASURE_OFFSET) * n / 100; i++) {
        mean_offset += cycles[i];
    }
    mean_offset /= (100 - 2 * MEASURE_OFFSET) * n / 100;

    median = cycles[i >> 1];
    printf("  Mean:           ");
    print_thousands(mean, 2);
    printf(" cycles\n");

    printf("  Mean (%d%%-%d%%): ", MEASURE_OFFSET, 100 - MEASURE_OFFSET);
    print_thousands(mean_offset, 2);
    printf(" cycles\n");

    printf("  Median:         ");
    print_thousands(median, 2);
    printf(" cycles\n");
}
