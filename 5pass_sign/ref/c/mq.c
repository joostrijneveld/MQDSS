#include <stdio.h>
#include <immintrin.h>
#include "mq.h"
#include "params.h"

// This is very slow and should be overridden for your particular platform
void MQ(gf31 *fx, const gf31 *x, const signed char *F)
{
    int i, j, k;
    gf31 xixj;
    int Fi = 0;

    for (i = 0; i < M; i++) {
        fx[i] = 0;
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j <= i; j++) {
            xixj = (x[i] * x[j]) % 31;
            for (k = 0; k < M; k++) {
                fx[k] += (xixj * (F[Fi] + 31));
                Fi++;
            }
        }
        for (k = 0; k < M; k++) {
            fx[k] %= 31;
        }
    }

    for (i = 0; i < N; i++) {
        for (k = 0; k < M; k++) {
            fx[k] += x[i] * (F[Fi++] + 31);
            fx[k] %= 31;
        }
    }
}

void G(gf31 *fx, const gf31 *x, const gf31 *y, const signed char *F)
{
    int i, j, k;
    gf31 xiyj_plus_xjyi;
    int Fi = 0;

    for (i = 0; i < M; i++) {
        fx[i] = 0;
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j <= i; j++) {
            xiyj_plus_xjyi = (x[i] * y[j] + x[j] * y[i]) % 31;
            for (k = 0; k < M; k++) {
                fx[k] += xiyj_plus_xjyi * (F[Fi] + 31);
                Fi++;
            }
        }
        for (k = 0; k < M; k++) {
            fx[k] %= 31;
        }
    }
}
