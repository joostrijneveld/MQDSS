#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "randombytes.h"
#include "api.h"
#include "params.h"
#include "mq.h"
#include "gf31.h"
#include "fips202.h"

static void HR(unsigned char *R, const unsigned char *sk,
               const unsigned char *m, const unsigned int mlen)
{
    uint64_t s_inc[26];

    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, sk, SK_BYTES);
    shake256_inc_absorb(s_inc, m, mlen);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(R, HASH_BYTES, s_inc);
}

static void Hdigest(unsigned char *D,
                    const unsigned char *pk, const unsigned char *R,
                    const unsigned char *m, const unsigned int mlen)
{
    uint64_t s_inc[26];

    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, pk, PK_BYTES);
    shake256_inc_absorb(s_inc, R, HASH_BYTES);
    shake256_inc_absorb(s_inc, m, mlen);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(D, HASH_BYTES, s_inc);
}

static void Hsigma0(unsigned char *sigma0, const unsigned char *commits)
{
    shake256(sigma0, HASH_BYTES, commits, HASH_BYTES * ROUNDS * 2);
}

/* Takes two arrays of N packed elements and an array of M packed elements,
   and computes a HASH_BYTES commitment. */
static void com_0(unsigned char *c,
           const unsigned char *rho,
           const unsigned char *inn, const unsigned char *inn2,
           const unsigned char *inm)
{
    uint64_t s_inc[26];

    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, rho, HASH_BYTES);
    shake256_inc_absorb(s_inc, inn, NPACKED_BYTES);
    shake256_inc_absorb(s_inc, inn2, NPACKED_BYTES);
    shake256_inc_absorb(s_inc, inm, MPACKED_BYTES);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(c, HASH_BYTES, s_inc);
}

/* Takes an array of N packed elements and an array of M packed elements,
   and computes a HASH_BYTES commitment. */
static void com_1(unsigned char *c,
           const unsigned char *rho,
           const unsigned char *inn, const unsigned char *inm)
{
    uint64_t s_inc[26];

    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, rho, HASH_BYTES);
    shake256_inc_absorb(s_inc, inn, NPACKED_BYTES);
    shake256_inc_absorb(s_inc, inm, MPACKED_BYTES);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(c, HASH_BYTES, s_inc);
}

/*
 * Generates an MQDSS key pair.
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
    signed char F[F_LEN];
    unsigned char skbuf[SEED_BYTES * 2];
    gf31 sk_gf31[N];
    gf31 pk_gf31[M];

    // Expand sk to obtain a seed for F and the secret input s.
    // We also expand to obtain a value for sampling r0, t0 and e0 during
    //  signature generation, but that is not relevant here.
    randombytes(sk, SEED_BYTES);
    shake256(skbuf, SEED_BYTES * 2, sk, SEED_BYTES);

    memcpy(pk, skbuf, SEED_BYTES);
    gf31_nrand_schar(F, F_LEN, pk, SEED_BYTES);
    gf31_nrand(sk_gf31, N, skbuf + SEED_BYTES, SEED_BYTES);
    MQ(pk_gf31, sk_gf31, F);
    vgf31_unique(pk_gf31, pk_gf31);
    gf31_npack(pk + SEED_BYTES, pk_gf31, M);

    return 0;
}

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen, const uint8_t *sk)
{
    signed char F[F_LEN];
    unsigned char skbuf[SEED_BYTES * 4];
    gf31 pk_gf31[M];
    unsigned char pk[SEED_BYTES + MPACKED_BYTES];
    // Concatenated for convenient hashing.
    unsigned char D_sigma0_h0_sigma1[HASH_BYTES * 3 + ROUNDS * (NPACKED_BYTES + MPACKED_BYTES)];
    unsigned char *D = D_sigma0_h0_sigma1;
    unsigned char *sigma0 = D_sigma0_h0_sigma1 + HASH_BYTES;
    unsigned char *h0 = D_sigma0_h0_sigma1 + 2*HASH_BYTES;
    unsigned char *t1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES;
    unsigned char *e1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES + ROUNDS * NPACKED_BYTES;
    uint64_t shakestate[25] = {0};
    unsigned char shakeblock[SHAKE256_RATE];
    unsigned char h1[((ROUNDS + 7) & ~7) >> 3];
    unsigned char rnd_seed[HASH_BYTES + SEED_BYTES];
    unsigned char rho[2 * ROUNDS * HASH_BYTES];
    unsigned char *rho0 = rho;
    unsigned char *rho1 = rho + ROUNDS * HASH_BYTES;
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

    shake256(skbuf, SEED_BYTES * 4, sk, SEED_BYTES);

    gf31_nrand_schar(F, F_LEN, skbuf, SEED_BYTES);

    HR(sig, sk, m, mlen);

    memcpy(pk, skbuf, SEED_BYTES);
    gf31_nrand(sk_gf31, N, skbuf + SEED_BYTES, SEED_BYTES);
    MQ(pk_gf31, sk_gf31, F);
    vgf31_unique(pk_gf31, pk_gf31);
    gf31_npack(pk + SEED_BYTES, pk_gf31, M);

    Hdigest(D, pk, sig, m, mlen);

    sig += HASH_BYTES;  // Compensate for prefixed R.

    memcpy(rnd_seed, skbuf + 2*SEED_BYTES, SEED_BYTES);
    memcpy(rnd_seed + SEED_BYTES, D, HASH_BYTES);
    shake256(rho, 2 * ROUNDS * HASH_BYTES, rnd_seed, SEED_BYTES + HASH_BYTES);

    memcpy(rnd_seed, skbuf + 3*SEED_BYTES, SEED_BYTES);
    memcpy(rnd_seed + SEED_BYTES, D, HASH_BYTES);
    gf31_nrand(rnd, (2 * N + M) * ROUNDS, rnd_seed, SEED_BYTES + HASH_BYTES);

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
        com_0(c + HASH_BYTES * (2*i + 0), rho0 + i*HASH_BYTES, packbuf0, packbuf1, packbuf2);
        vgf31_shorten_unique(r1 + i*N, r1 + i*N);
        vgf31_shorten_unique(gx + i*M, gx + i*M);
        gf31_npack(packbuf0, r1 + i*N, N);
        gf31_npack(packbuf1, gx + i*M, M);
        com_1(c + HASH_BYTES * (2*i + 1), rho1 + i*HASH_BYTES, packbuf0, packbuf1);
    }

    Hsigma0(sigma0, c);
    shake256_absorb(shakestate, D_sigma0_h0_sigma1, 2 * HASH_BYTES);
    shake256_squeezeblocks(shakeblock, 1, shakestate);

    memcpy(h0, shakeblock, HASH_BYTES);

    memcpy(sig, sigma0, HASH_BYTES);
    sig += HASH_BYTES;  // Compensate for sigma_0.

    for (i = 0; i < ROUNDS; i++) {
        do {
            alpha = shakeblock[alpha_count] & 31;
            alpha_count++;
            if (alpha_count == SHAKE256_RATE) {
                alpha_count = 0;
                shake256_squeezeblocks(shakeblock, 1, shakestate);
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

    memcpy(sig, t1packed, NPACKED_BYTES * ROUNDS);
    sig += NPACKED_BYTES * ROUNDS;
    memcpy(sig, e1packed, MPACKED_BYTES * ROUNDS);
    sig += MPACKED_BYTES * ROUNDS;

    shake256(h1, ((ROUNDS + 7) & ~7) >> 3, D_sigma0_h0_sigma1, 3*HASH_BYTES + ROUNDS*(NPACKED_BYTES + MPACKED_BYTES));

    for (i = 0; i < ROUNDS; i++) {
        b = (h1[(i >> 3)] >> (i & 7)) & 1;
        if (b == 0) {
            gf31_npack(sig, r0+i*N, N);
        } else if (b == 1) {
            gf31_npack(sig, r1+i*N, N);
        }
        memcpy(sig + NPACKED_BYTES, c + HASH_BYTES * (2*i + (1 - b)), HASH_BYTES);
        memcpy(sig + NPACKED_BYTES + HASH_BYTES, rho + (i + b * ROUNDS) * HASH_BYTES, HASH_BYTES);
        sig += NPACKED_BYTES + 2*HASH_BYTES;
    }

    *siglen = SIG_LEN;

    return 0;
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk)
{
    gf31 r[N];
    gf31 t[N];
    gf31 e[M];
    signed char F[F_LEN];
    gf31 pk_gf31[M];
    // Concatenated for convenient hashing.
    unsigned char D_sigma0_h0_sigma1[HASH_BYTES * 3 + ROUNDS * (NPACKED_BYTES + MPACKED_BYTES)];
    unsigned char *D = D_sigma0_h0_sigma1;
    unsigned char *sigma0 = D_sigma0_h0_sigma1 + HASH_BYTES;
    unsigned char *h0 = D_sigma0_h0_sigma1 + 2*HASH_BYTES;
    unsigned char *t1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES;
    unsigned char *e1packed = D_sigma0_h0_sigma1 + 3*HASH_BYTES + ROUNDS * NPACKED_BYTES;
    unsigned char h1[((ROUNDS + 7) & ~7) >> 3];
    unsigned char c[HASH_BYTES * ROUNDS * 2];
    memset(c, 0, HASH_BYTES*2);
    gf31 x[N];
    gf31 y[M];
    gf31 z[M];
    unsigned char packbuf0[NPACKED_BYTES];
    unsigned char packbuf1[MPACKED_BYTES];
    uint64_t shakestate[25] = {0};
    unsigned char shakeblock[SHAKE256_RATE];
    int i, j;
    gf31 alpha;
    int alpha_count = 0;
    unsigned char b;

    if (siglen != SIG_LEN) {
        return -1;
    }

    Hdigest(D, pk, sig, m, mlen);

    sig += HASH_BYTES;

    gf31_nrand_schar(F, F_LEN, pk, SEED_BYTES);
    pk += SEED_BYTES;
    gf31_nunpack(pk_gf31, pk, M);

    memcpy(sigma0, sig, HASH_BYTES);

    shake256_absorb(shakestate, D_sigma0_h0_sigma1, 2 * HASH_BYTES);
    shake256_squeezeblocks(shakeblock, 1, shakestate);

    memcpy(h0, shakeblock, HASH_BYTES);

    sig += HASH_BYTES;

    memcpy(t1packed, sig, ROUNDS * NPACKED_BYTES);
    sig += ROUNDS*NPACKED_BYTES;
    memcpy(e1packed, sig, ROUNDS * MPACKED_BYTES);
    sig += ROUNDS*MPACKED_BYTES;

    shake256(h1, ((ROUNDS + 7) & ~7) >> 3, D_sigma0_h0_sigma1, 3*HASH_BYTES + ROUNDS*(NPACKED_BYTES + MPACKED_BYTES));

    for (i = 0; i < ROUNDS; i++) {
        do {
            alpha = shakeblock[alpha_count] & 31;
            alpha_count++;
            if (alpha_count == SHAKE256_RATE) {
                alpha_count = 0;
                shake256_squeezeblocks(shakeblock, 1, shakestate);
            }
        } while (alpha == 31);
        b = (h1[(i >> 3)] >> (i & 7)) & 1;

        gf31_nunpack(r, sig, N);
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
            com_0(c + HASH_BYTES*(2*i + 0), sig + HASH_BYTES + NPACKED_BYTES, sig, packbuf0, packbuf1);
        } else {
            MQ(y, r, F);
            G(z, t, r, F);
            for (j = 0; j < N; j++) {
                y[j] = alpha * (31 + pk_gf31[j] - y[j]) - z[j] - e[j] + 62;
            }
            vgf31_shorten_unique(y, y);
            gf31_npack(packbuf0, y, M);
            com_1(c + HASH_BYTES*(2*i + 1), sig + HASH_BYTES + NPACKED_BYTES, sig, packbuf0);
        }
        memcpy(c + HASH_BYTES*(2*i + (1 - b)), sig + NPACKED_BYTES, HASH_BYTES);
        sig += NPACKED_BYTES + 2*HASH_BYTES;
    }

    Hsigma0(c, c);
    if (memcmp(c, sigma0, HASH_BYTES)) {
        return 1;
    }

    return 0;
}

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk)
{
    size_t siglen;

    crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

    memmove(sm + SIG_LEN, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    /* The API caller does not necessarily know what size a signature should be
       but MQDSS signatures are always exactly SIG_LEN. */
    if (smlen < SIG_LEN) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    *mlen = smlen - SIG_LEN;

    if (crypto_sign_verify(sm, SIG_LEN, sm + SIG_LEN, (size_t)*mlen, pk)) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    /* If verification was successful, move the message to the right place. */
    memmove(m, sm + SIG_LEN, *mlen);

    return 0;
}
