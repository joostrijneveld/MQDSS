#include <stdio.h>
#include <immintrin.h>
#include "mq.h"
#include "params.h"

inline static
void pack_store(gf31 *xij, const gf31 *x1, const gf31 *x2)
{
    int j;
    for (j = 0; j < 8; j++) {
        xij[j] = x1[j];
    }
    for (j = 0; j < 8; j++) {
        xij[8 + j] = x2[j];
    }
    for (j = 0; j < 8; j++) {
        xij[16 + j] = x1[8 + j];
    }
    for (j = 0; j < 8; j++) {
        xij[24 + j] = x2[8 + j];
    }
}

inline static
void gen_quad_rec_n16(gf31 *xij, const gf31 *xi, const gf31 *xj)
{
    int i, j, k;
    gf31 x1[16];
    gf31 x2[16];

    k = 0;
    for (i = 0, k = 0; i < 16; i += 2) {
        for (j = 0; j < 16; j++) {
            x1[j] = mod31(xi[i] * xj[j]);
        }
        for (j = 0; j < 16; j++) {
            x2[j] = mod31(xi[i + 1] * xj[j]);
        }

        pack_store(xij + k, x1, x2);
        k += 32;
    }
}

static inline
void ror_16x2(gf31 *r, const gf31 *x, unsigned int n)
{
    unsigned int i;
    for (i = 0; i < 16; i++) {
        r[(i + 16 - n) & 15] = x[i];
    }
    for (i = 0; i < 16; i++) {
        r[((i + 16 - n) & 15) + 16] = x[i + 16];
    }
}

inline static
void generate_quadratic_tri_n32(gf31 *xij, const gf31 *x)
{
    int i, j, k;
    gf31 x1[16];
    gf31 x2[16];
    gf31 x_rot[32];

    k = 0;
    gen_quad_rec_n16(xij, x, x + 16);
    k += 16 * 16;

    for (i = 0; i < 8; i++) {
        ror_16x2(x_rot, x, i);
        for (j = 0; j < 16; j++) {
            x1[j] = mod31(x[j] * x_rot[j]);
        }
        for (j = 0; j < 16; j++) {
            x2[j] = mod31(x[j + 16] * x_rot[j + 16]);
        }
        pack_store(xij + k, x1, x2);
        k += 32;
    }

    ror_16x2(x_rot, x, 8);
    for (j = 0; j < 16; j++) {
        x1[j] = mod31(x[j]*x_rot[j]);
    }
    for (j = 0; j < 16; j++) {
        x2[j] = mod31(x[j+16]*x_rot[j+16]);
    }
    for (j = 0; j < 8; j++) {
        xij[k + j] = x1[j];
    }
    k += 8;
    for (j = 0; j < 8; j++) {
        xij[k + j] = x2[j];
    }
}

inline static
void generate_quadratic_rec_n32(gf31 *xij, const gf31 *xi, const gf31 *xj)
{
    int i, j, k;

    k = 0;
    for (i = 0; i < 32;i++) {
        for (j = 0; j < 32; j++) {
            xij[k] = mod31(xi[i]*xj[j]);
            k++;
        }
    }
}

inline static
void generate_quadratic_terms_n64(gf31 *xij, const gf31 *x)
{
    generate_quadratic_tri_n32(xij, x);
    generate_quadratic_tri_n32(xij + (32 * 33 / 2), x + 32);
    generate_quadratic_rec_n32(xij + (32 * 33), x, x + 32);
}

inline static
void gen_xiyj_p_xjyi_rec_n16(gf31 *xij,
                             const gf31 *xi, const gf31 *xj,
                             const gf31 *yi, const gf31 *yj)
{
    int i, j, k;
    gf31 x1[16];
    gf31 x2[16];

    k = 0;
    for (i = 0; i < 16; i += 2) {
        for (j = 0; j < 16; j++) {
            x1[j] = mod31(xi[i]*yj[j] + yi[i]*xj[j]);
        }
        for (j = 0; j < 16; j++) {
            x2[j] = mod31(xi[i + 1]*yj[j] + yi[i + 1]*xj[j]);
        }

        pack_store(xij + k, x1, x2);
        k += 32;
    }
}

inline static
void generate_xiyj_p_xjyi_tri_n32(gf31 *xij, const gf31 *x, const gf31 *y)
{
    int i, j, k;
    gf31 x1[16];
    gf31 x2[16];
    gf31 x_rot[32];
    gf31 y_rot[32];

    k = 0;
    gen_xiyj_p_xjyi_rec_n16(xij, x, x + 16, y, y + 16);
    k += 16 * 16;

    for (i = 0; i < 8;i++) {
        ror_16x2(x_rot, x, i);
        ror_16x2(y_rot, y, i);
        for (j = 0; j < 16; j++) {
            x1[j] = mod31(x[j] * y_rot[j] + y[j] * x_rot[j]);
        }
        for (j = 0; j < 16; j++) {
            x2[j] = mod31(x[j + 16]*y_rot[j + 16] + y[j + 16]*x_rot[j + 16]);
        }
        pack_store(xij + k, x1, x2);
        k += 32;
    }

    ror_16x2(x_rot, x, 8);
    ror_16x2(y_rot, y, 8);
    for (j = 0; j < 16; j++) {
        x1[j] = mod31(x[j]*y_rot[j] + y[j]*x_rot[j]);
    }
    for (j = 0; j < 16; j++) {
        x2[j] = mod31(x[j + 16]*y_rot[j + 16] + y[j + 16]*x_rot[j + 16]);
    }
    for (j = 0; j < 8; j++) {
        xij[k + j] = x1[j];
    }
    k += 8;
    for (j = 0; j < 8; j++) {
        xij[k + j] = x2[j];
    }
}

inline static
void generate_xiyj_p_xjyi_rec_n32(gf31 *xij,
                                  const gf31 *xi, const gf31 *xj,
                                  const gf31 *yi, const gf31 *yj)
{
    int i, j, k;

    k = 0;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            xij[k] = mod31(xi[i]*yj[j] + yi[i]*xj[j]);
            k++;
        }
    }
}

inline static
void generate_xiyj_p_xjyi_terms_n64(gf31 *xij, const gf31 *x, const gf31 *y)
{
    generate_xiyj_p_xjyi_tri_n32(xij, x, y);
    generate_xiyj_p_xjyi_tri_n32(xij + (32 * 33 / 2), x + 32, y + 32);
    generate_xiyj_p_xjyi_rec_n32(xij + (32 * 33), x, x + 32, y, y + 32);
}

void MQ(gf31 *fx, const gf31 *x, const signed char *F)
{
    int i, j;
    gf31 _xij[N*(N+1) >> 1];
    int Fi = 0;
    int r[M] = {0};

    generate_quadratic_terms_n64(_xij, x);

    for (i = 0; i < N; i += 2) {
        for (j = 0; j < M; j++) {
            r[j] += ((int)x[i])*((int)F[Fi]) + ((int)x[i+1])*((int)F[Fi+1]);
            Fi += 2;
        }
    }

    for (i = 0; i < (N*(N+1)) >> 1; i += 2) {
        for (j = 0; j < M; j++) {
            r[j] += ((int)_xij[i])*((int)F[Fi]) + ((int)_xij[i+1])*((int)F[Fi+1]);
            Fi += 2;
        }
    }

    for (i = 0; i < M; i++) {
        fx[i] = mod31((r[i] >> 15) + (r[i] & 0x7FFF));
    }
}

void G(gf31 *fx, const gf31 *x, const gf31 *y, const signed char *F)
{
    int i, j;
    gf31 _xij[N*(N+1) >> 1];
    int Fi = 0;
    int r[M] = {0};

    generate_xiyj_p_xjyi_terms_n64(_xij, x, y);

    Fi += N*M;

    for (i = 0; i < (N*(N+1)) >> 1; i += 2) {
        for (j = 0; j < M; j++) {
            r[j] += ((int)_xij[i])*((int)F[Fi]) + ((int)_xij[i+1])*((int)F[Fi+1]);
            Fi += 2;
        }
    }

    for (i = 0; i < M; i++) {
        fx[i] = mod31((r[i] >> 15) + (r[i] & 0x7FFF));
    }
}
