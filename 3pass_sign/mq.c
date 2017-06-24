#include <string.h>
#include "params.h"
#include "mq.h"

#ifdef REFERENCE

// If we're compiling for non-AVX2, short-circuit the asm functions to ref code

void MQ_asm(unsigned char *fx, const unsigned char *x, const unsigned char *F) {
    MQ(fx, x, F);
}

void G_asm(unsigned char *fx, const unsigned char *x, const unsigned char *y,
           const unsigned char *F) {
    G(fx, x, y, F);
}

#endif

void ROL_NBYTES(unsigned char *out, const unsigned char *in)
{
    int i;
    unsigned char tmp = in[N_BYTES - 1];

    for (i = N_BYTES - 1; i > 0; i--) {
        out[i] = (in[i] << 1) | (in[i - 1] >> 7);
    }
    out[0] = (in[0] << 1) | (tmp >> 7);
}

void MQ(unsigned char *fx, const unsigned char *x, const unsigned char *F)
{
    unsigned char f_accs[M];
    unsigned char x_reg[N_BYTES];
    unsigned char buf;
    int i, j, k;

    memset(fx, 0, M_BYTES);
    memset(f_accs, 0, M);
    memcpy(x_reg, x, N_BYTES);

    for (i = 0; i < (N >> 1); i++) {
        for (j = 0; j < N_BYTES; j++) {
            buf = x_reg[j] & x[j];
            for (k = 0; k < M; k++) {
                f_accs[k] ^= buf & F[(i * 256 + k) * 32 + j];
            }
        }
        ROL_NBYTES(x_reg, x_reg);
    }

    for (j = 0; j < N_BYTES >> 1; j++) {
        buf = x_reg[j] & x[j];
        for (k = 0; k < M; k++) {
            f_accs[k] ^= buf & F[128 * 256 * 32 + k * 16 + j];
        }
    }

    for (i = 0; i < M; i++) {
        f_accs[i] ^= (f_accs[i] >> 4);
        f_accs[i] ^= (f_accs[i] >> 2);
        f_accs[i] ^= (f_accs[i] >> 1);
        fx[i >> 3] ^= (f_accs[i] & 1) << (i & 7);
    }
}

void G(unsigned char *gx, const unsigned char *x, const unsigned char *y,
       const unsigned char *F)
{
    unsigned char g_accs[M];
    unsigned char x_reg[N_BYTES];
    unsigned char y_reg[N_BYTES];
    unsigned char buf;
    int i, j, k;

    memset(gx, 0, M_BYTES);
    memset(g_accs, 0, M);
    memcpy(x_reg, x, N_BYTES);
    memcpy(y_reg, y, N_BYTES);

    for (i = 0; i < (N >> 1); i++) {
        for (j = 0; j < N_BYTES; j++) {
            buf = (x_reg[j] & y[j]) ^ (y_reg[j] & x[j]);
            for (k = 0; k < M; k++) {
                g_accs[k] ^= buf & F[(i * 256 + k) * 32 + j];
            }
        }
        ROL_NBYTES(x_reg, x_reg);
        ROL_NBYTES(y_reg, y_reg);
    }

    for (j = 0; j < N_BYTES >> 1; j++) {
        buf = (x_reg[j] & y[j]) ^ (y_reg[j] & x[j]);
        for (k = 0; k < M; k++) {
            g_accs[k] ^= buf & F[128 * 256 * 32 + k * 16 + j];
        }
    }

    for (i = 0; i < M; i++) {
        g_accs[i] ^= (g_accs[i] >> 4);
        g_accs[i] ^= (g_accs[i] >> 2);
        g_accs[i] ^= (g_accs[i] >> 1);
        gx[i >> 3] ^= (g_accs[i] & 1) << (i & 7);
    }
}