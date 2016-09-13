#include <string.h>
#include <assert.h>
#include <openssl/sha.h>
#include "randombytes.h"
#include "sign.h"
#include "params.h"
#include "mq.h"
#include "gf31.h"
#include "SimpleFIPS202.h"
#include "KeccakHash.h"

void H(unsigned char *out, const unsigned char *in, const unsigned int len)
{
    SHA3_256(out, in, len);
}

void com_0(unsigned char *c,
          const unsigned char *inn, const unsigned char *inn2,
          const unsigned char *inm)
{
    unsigned char buffer[2*NPACKED_BYTES + MPACKED_BYTES];
    memcpy(buffer, inn, NPACKED_BYTES);
    memcpy(buffer + NPACKED_BYTES, inn2, NPACKED_BYTES);
    memcpy(buffer + 2*NPACKED_BYTES, inm, MPACKED_BYTES);
    SHA3_256(c, buffer, 2*NPACKED_BYTES + MPACKED_BYTES);
}

void com_1(unsigned char *c, const unsigned char *inn, const unsigned char *inm)
{
    unsigned char buffer[NPACKED_BYTES + MPACKED_BYTES];
    memcpy(buffer, inn, NPACKED_BYTES);
    memcpy(buffer + NPACKED_BYTES, inm, MPACKED_BYTES);
    SHA3_256(c, buffer, NPACKED_BYTES + MPACKED_BYTES);
}

void crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
    signed char F[F_LEN];
    gf31 sk_gf31[N];
    gf31 pk_gf31[M];

    randombytes(sk, SK_BYTES);
    memcpy(sk + SEED_BYTES, pk, SEED_BYTES);
    gf31_nrand_schar(F, F_LEN, pk, SEED_BYTES);
    gf31_nrand(sk_gf31, N, sk, SEED_BYTES);
    MQ(pk_gf31, sk_gf31, F);
    vgf31_unique(pk_gf31, pk_gf31);
    gf31_npack(pk + HASH_BYTES, pk_gf31, M);
}

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk)
{
    signed char F[F_LEN];
    // Concatenated for convenient H(). TODO Perhaps we can store this in sm instead.
    // TODO is it strictly necessary that h0 is included as input for h1 = H(..)?
    unsigned char D_sigma0_h0_sigma1[HASH_BYTES * 3 + ROUNDS * (NPACKED_BYTES + MPACKED_BYTES)];
    unsigned char *D = D_sigma0_h0_sigma1;
    unsigned char *sigma0 = D_sigma0_h0_sigma1 + HASH_BYTES;
    unsigned char *h0 = D_sigma0_h0_sigma1 + 2*HASH_BYTES;
    unsigned char *t1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES;
    unsigned char *e1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES + ROUNDS * NPACKED_BYTES;
    unsigned char h[HASH_BYTES];
    unsigned char h1[((ROUNDS >> 3) + 7) & ~7];
    unsigned char rnd_seed[HASH_BYTES + SK_BYTES];
    gf31 sk_gf31[N];
    gf31 rnd[(2 * N + M) * ROUNDS];  // Concatenated for easy RNG.
    gf31 *r0 = rnd;
    gf31 *t0 = rnd + N * ROUNDS;
    gf31 *e0 = rnd + 2 * N * ROUNDS;
    gf31 r1[N * ROUNDS];
    gf31 t1[N * ROUNDS];
    gf31 e1[M * ROUNDS];
    gf31 gx[M * ROUNDS];
    unsigned char packbuf0[NPACKED_BYTES];
    unsigned char packbuf1[NPACKED_BYTES];
    unsigned char packbuf2[MPACKED_BYTES];
    unsigned char c[HASH_BYTES * ROUNDS * 2];
    gf31 alpha;
    int alpha_count = 0;
    unsigned char b;
    int i, j;
    Keccak_HashInstance keccak;

    gf31_nrand_schar(F, F_LEN, sk+SEED_BYTES, SEED_BYTES);

    assert(SIG_LEN > SEED_BYTES);
    memcpy(sm + SIG_LEN - SEED_BYTES, sk, SEED_BYTES);
    memcpy(sm + SIG_LEN, m, mlen);
    H(sm, sm + SIG_LEN - SEED_BYTES, mlen + SEED_BYTES);  // Compute R.
    memcpy(sm + SIG_LEN - HASH_BYTES, sm, HASH_BYTES);
    H(D, sm + SIG_LEN - HASH_BYTES, mlen + HASH_BYTES);

    sm += HASH_BYTES;  // Compensate for prefixed R.

    memcpy(rnd_seed, sk, SEED_BYTES);
    memcpy(rnd_seed + SEED_BYTES, D, HASH_BYTES);
    gf31_nrand(rnd, (2 * N + M) * ROUNDS, rnd_seed, SEED_BYTES + HASH_BYTES);

    gf31_nrand(sk_gf31, N, sk, SEED_BYTES);

    for (i = 0; i < ROUNDS; i++) {
        for (j = 0; j < N; j++) {
            r1[j + i*N] = 31 + sk_gf31[j] - r0[j + i*N];
        }
        G(gx + i*M, t0 + i*N, r1 + i*N, F);
    }
    for (i = 0; i < ROUNDS * M; i++) {
        gx[i] += e0[i];
    }
    for (i = 0; i < ROUNDS; i++) {
        gf31_npack(packbuf0, r0 + i*N, N);
        gf31_npack(packbuf1, t0 + i*N, N);
        gf31_npack(packbuf2, e0 + i*M, M);
        com_0(c + HASH_BYTES * (2*i + 0), packbuf0, packbuf1, packbuf2);
        vgf31_shorten_unique(r1 + i*N, r1 + i*N);
        vgf31_shorten_unique(gx + i*M, gx + i*M);
        gf31_npack(packbuf0, r1 + i*N, N);
        gf31_npack(packbuf1, gx + i*M, M);
        com_1(c + HASH_BYTES * (2*i + 1), packbuf0, packbuf1);
    }

    H(sigma0, c, HASH_BYTES * ROUNDS * 2);  // Compute sigma_0.
    Keccak_HashInitialize_SHAKE128(&keccak);
    Keccak_HashUpdate(&keccak, D_sigma0_h0_sigma1, 2 * HASH_BYTES * 8);
    Keccak_HashFinal(&keccak, h0);
    Keccak_HashSqueeze(&keccak, h0, HASH_BYTES * 8);

    memcpy(sm, sigma0, HASH_BYTES);
    sm += HASH_BYTES;  // Compensate for sigma_0.

    memcpy(h, h0, HASH_BYTES);  // Since we want to preserve the original h0.
    for (i = 0; i < ROUNDS; i++) {
        do {
            alpha = h[alpha_count] & 31;
            alpha_count++;
            if (alpha_count == HASH_BYTES) {
                alpha_count = 0;
                Keccak_HashSqueeze(&keccak, h, HASH_BYTES * 8);
            }
        } while (alpha == 31);
        for (j = 0; j < N; j++) {
            t1[i*N + j] = alpha * r0[j + i*N] - t0[j + i*N] + 31;
        }
        MQ(e1 + i*M, r0 + i*N, F);
        for (j = 0; j < N; j++) {
            e1[i*N + j] = alpha * e1[j + i*M] - e0[j + i*M] + 31;
        }
        vgf31_shorten_unique(t1 + i*N, t1 + i*N);
        vgf31_shorten_unique(e1 + i*N, e1 + i*N);
    }
    gf31_npack(t1packed, t1, N * ROUNDS);
    gf31_npack(e1packed, e1, M * ROUNDS);

    memcpy(sm, t1packed, NPACKED_BYTES * ROUNDS);
    sm += NPACKED_BYTES * ROUNDS;
    memcpy(sm, e1packed, MPACKED_BYTES * ROUNDS);
    sm += MPACKED_BYTES * ROUNDS;

    SHAKE128(h1, ((ROUNDS >> 3) + 7) & ~7, D_sigma0_h0_sigma1, 3*HASH_BYTES + ROUNDS*(NPACKED_BYTES + MPACKED_BYTES));

    for (i = 0; i < ROUNDS; i++) {
        b = (h1[(i >> 3)] >> (i & 7)) & 1;
        if (b == 0) {
            gf31_npack(sm, r0+i*N, N);
        } else if (b == 1) {
            gf31_npack(sm, r1+i*N, N);
        }
        memcpy(sm + NPACKED_BYTES, c + HASH_BYTES * (2*i + (1 - b)), HASH_BYTES);
        sm += NPACKED_BYTES + HASH_BYTES;
    }
    *smlen = SIG_LEN + mlen;
    return 0;
}

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    gf31 r[N];
    gf31 t[N];
    gf31 e[M];
    signed char F[F_LEN];
    gf31 pk_gf31[M];
    unsigned char D_sigma0_h0_sigma1[HASH_BYTES * 3 + ROUNDS * (NPACKED_BYTES + MPACKED_BYTES)];
    unsigned char *D = D_sigma0_h0_sigma1;
    unsigned char *sigma0 = D_sigma0_h0_sigma1 + HASH_BYTES;
    unsigned char *h0 = D_sigma0_h0_sigma1 + 2*HASH_BYTES;
    unsigned char *t1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES;
    unsigned char *e1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES + ROUNDS * NPACKED_BYTES;
    unsigned char h1[((ROUNDS >> 3) + 7) & ~7];
    unsigned char c[HASH_BYTES * ROUNDS * 2];
    memset(c, 0, HASH_BYTES*2);
    gf31 x[N];
    gf31 y[M];
    gf31 z[M];
    unsigned char packbuf0[NPACKED_BYTES];
    unsigned char packbuf1[MPACKED_BYTES];
    unsigned char h[HASH_BYTES];
    int i, j;
    gf31 alpha;
    int alpha_count = 0;
    unsigned char b;
    Keccak_HashInstance keccak;

    memcpy(m, sm, HASH_BYTES);  // Copy R to m.
    memcpy(m + HASH_BYTES, sm + SIG_LEN, smlen - SIG_LEN);  // Copy message.
    H(D, m, smlen - SIG_LEN + HASH_BYTES);
    sm += HASH_BYTES;

    gf31_nrand_schar(F, F_LEN, pk, SEED_BYTES);
    pk += SEED_BYTES;
    gf31_nunpack(pk_gf31, pk, M);

    memcpy(sigma0, sm, HASH_BYTES);

    Keccak_HashInitialize_SHAKE128(&keccak);
    Keccak_HashUpdate(&keccak, D_sigma0_h0_sigma1, 2 * HASH_BYTES * 8);
    Keccak_HashFinal(&keccak, h0);
    Keccak_HashSqueeze(&keccak, h0, HASH_BYTES * 8);
    memcpy(h, h0, HASH_BYTES);

    sm += HASH_BYTES;

    memcpy(t1packed, sm, ROUNDS * NPACKED_BYTES);
    sm += ROUNDS*NPACKED_BYTES;
    memcpy(e1packed, sm, ROUNDS * MPACKED_BYTES);
    sm += ROUNDS*MPACKED_BYTES;

    SHAKE128(h1, ((ROUNDS >> 3) + 7) & ~7, D_sigma0_h0_sigma1, 3*HASH_BYTES + ROUNDS*(NPACKED_BYTES + MPACKED_BYTES));

    for (i = 0; i < ROUNDS; i++) {
        do {
            alpha = h[alpha_count] & 31;
            alpha_count++;
            if (alpha_count == HASH_BYTES) {
                alpha_count = 0;
                Keccak_HashSqueeze(&keccak, h, HASH_BYTES * 8);
            }
        } while (alpha == 31);
        b = (h1[(i >> 3)] >> (i & 7)) & 1;

        gf31_nunpack(r, sm, N);
        gf31_nunpack(t, t1packed + NPACKED_BYTES*i, N);
        gf31_nunpack(e, e1packed + MPACKED_BYTES*i, M);

        if (b == 0) {
            MQ(y, r, F);
            for (j = 0; j < N; j++) {
                x[j] = alpha * r[j] - t[j] + 31;
            }
            for (j = 0; j < N; j++) {
                y[j] = alpha * y[j] - e[j] + 31;
            }
            vgf31_shorten_unique(x, x);
            vgf31_shorten_unique(y, y);
            gf31_npack(packbuf0, x, N);
            gf31_npack(packbuf1, y, M);
            com_0(c + HASH_BYTES*(2*i + 0), sm, packbuf0, packbuf1);
        } else {
            MQ(y, r, F);
            G(z, t, r, F);
            for (j = 0; j < N; j++) {
                y[j] = alpha * (31 + pk_gf31[j] - y[j]) - z[j] - e[j] + 62;
            }
            vgf31_shorten_unique(y, y);
            gf31_npack(packbuf0, y, M);
            com_1(c + HASH_BYTES*(2*i + 1), sm, packbuf0);
        }
        sm += NPACKED_BYTES;
        memcpy(c + HASH_BYTES*(2*i + (1 - b)), sm, HASH_BYTES);
        sm += HASH_BYTES;
    }

    H(c, c, HASH_BYTES * ROUNDS * 2);
    if (memcmp(c, sigma0, HASH_BYTES)) {
        return 1;
    }

    *mlen = smlen - SIG_LEN;
    memcpy(m, sm, smlen - SIG_LEN);
    return 0;
}
