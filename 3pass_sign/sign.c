#include <string.h>
#include <assert.h>
#include "randombytes.h"
#include "sign.h"
#include "params.h"
#include "mq.h"
#include "SimpleFIPS202.h"
#include "KeccakHash.h"

static void expand_seed(unsigned char* out, const unsigned int len,
                        const unsigned char *seed, const unsigned int seedlen)
{
    SHAKE128(out, len, seed, seedlen);
}

void H(unsigned char *out, const unsigned char *in, const unsigned int len)
{
    SHA3_256(out, in, len);
}

void com(unsigned char *c, const unsigned char *inn, const unsigned char *inm)
{
    unsigned char buffer[N_BYTES + M_BYTES];
    memcpy(buffer, inn, N_BYTES);
    memcpy(buffer+N_BYTES, inm, M_BYTES);
    SHA3_256(c, buffer, N_BYTES + M_BYTES);
}

void crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
    unsigned char F[F_BYTES];

    randombytes(sk, SK_BYTES);
    memcpy(sk + SEED_BYTES, pk, SEED_BYTES);
    expand_seed(F, F_BYTES, pk, SEED_BYTES);
    MQ_asm(pk + HASH_BYTES, sk, F);
}

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk)
{
    unsigned char F[F_BYTES];
    unsigned char D_sigma0[HASH_BYTES * 2];  // Concatenated for convenient H().
    unsigned char *D = D_sigma0;
    unsigned char *sigma0 = D_sigma0 + HASH_BYTES;
    unsigned char rnd_seed[HASH_BYTES + SK_BYTES];
    unsigned char rnd[(2 * N_BYTES + M_BYTES) * ROUNDS];  // Concatenated for easy RNG.
    unsigned char *r0 = rnd;
    unsigned char *t0 = rnd + N_BYTES * ROUNDS;
    unsigned char *e0 = rnd + 2 * N_BYTES * ROUNDS;
    unsigned char r1[N_BYTES * ROUNDS];
    unsigned char t1[N_BYTES * ROUNDS];
    unsigned char e1[M_BYTES * ROUNDS];
    unsigned char gx[M_BYTES * ROUNDS];
    unsigned char c[HASH_BYTES * ROUNDS * 3];
    unsigned char h[HASH_BYTES];
    unsigned char ch;
    int i, j, ch_count = 0;
    Keccak_HashInstance keccak;

    expand_seed(F, F_BYTES, sk+SEED_BYTES, SEED_BYTES);

    assert(SIG_LEN > SK_BYTES);
    memcpy(sm + SIG_LEN - SK_BYTES, sk, SK_BYTES);
    memcpy(sm + SIG_LEN, m, mlen);
    H(sm, sm + SIG_LEN - SK_BYTES, mlen + SK_BYTES);  // Compute R.
    memcpy(sm + SIG_LEN - HASH_BYTES, sm, HASH_BYTES);
    H(D, sm + SIG_LEN - HASH_BYTES, mlen + HASH_BYTES);

    sm += HASH_BYTES;  // Compensate for prefixed R.

    memcpy(rnd_seed, sk, SK_BYTES);
    memcpy(rnd_seed + SK_BYTES, D, HASH_BYTES);
    expand_seed(rnd, (2 * N_BYTES + M_BYTES) * ROUNDS,
                rnd_seed, SK_BYTES + HASH_BYTES);

    for (i = 0; i < ROUNDS * N_BYTES; i++) {
        t1[i] = r0[i] ^ t0[i];
    }
    for (i = 0; i < ROUNDS; i++) {
        for (j = 0; j < N_BYTES; j++) {
            r1[j + i*N_BYTES] = r0[j + i*N_BYTES] ^ sk[j];
        }
        MQ_asm(e1 + i*M_BYTES, r0 + i*N_BYTES, F);
        G_asm(gx + i*M_BYTES, t0 + i*N_BYTES, r1 + i*N_BYTES, F);
    }
    for (i = 0; i < ROUNDS * M_BYTES; i++) {
        e1[i] ^= e0[i];
        gx[i] ^= e0[i];
    }
    for (i = 0; i < ROUNDS; i++) {
        com(c + HASH_BYTES * (3*i + 0), r1 + i*N_BYTES, gx + i*M_BYTES);
        com(c + HASH_BYTES * (3*i + 1), t0 + i*N_BYTES, e0 + i*M_BYTES);
        com(c + HASH_BYTES * (3*i + 2), t1 + i*N_BYTES, e1 + i*M_BYTES);
    }

    H(sigma0, c, HASH_BYTES * ROUNDS * 3);  // Compute sigma_0.
    memcpy(sm, sigma0, HASH_BYTES);
    sm += HASH_BYTES;  // Compensate for sigma_0.

    Keccak_HashInitialize_SHAKE128(&keccak);
    Keccak_HashUpdate(&keccak, D_sigma0, 2 * HASH_BYTES * 8);
    Keccak_HashFinal(&keccak, h);
    Keccak_HashSqueeze(&keccak, h, HASH_BYTES * 8);

    for (i = 0; i < ROUNDS; i++) {
        do {
            ch = h[ch_count >> 2] >> ((ch_count & 3) << 1) & 3;
            ch_count++;
            if (ch_count == HASH_BYTES * 4) {
                ch_count = 0;
                Keccak_HashSqueeze(&keccak, h, HASH_BYTES * 8);
            }
        } while (ch == 3);

        if (ch == 0) {
            memcpy(sm, r0+i*N_BYTES, N_BYTES);
            memcpy(sm+N_BYTES, t1+i*N_BYTES, N_BYTES);
            memcpy(sm+2*N_BYTES, e1+i*M_BYTES, M_BYTES);
            memcpy(sm+2*N_BYTES+M_BYTES, c + HASH_BYTES * 3*i, HASH_BYTES);
        }
        else if (ch == 1) {
            memcpy(sm, r1+i*N_BYTES, N_BYTES);
            memcpy(sm+N_BYTES, t1+i*N_BYTES, N_BYTES);
            memcpy(sm+2*N_BYTES, e1+i*M_BYTES, M_BYTES);
            memcpy(sm+2*N_BYTES+M_BYTES, c + HASH_BYTES * (3*i + 1), HASH_BYTES);
        }
        else if (ch == 2) {
            memcpy(sm, r1+i*N_BYTES, N_BYTES);
            memcpy(sm+N_BYTES, t0+i*N_BYTES, N_BYTES);
            memcpy(sm+2*N_BYTES, e0+i*M_BYTES, M_BYTES);
            memcpy(sm+2*N_BYTES+M_BYTES, c + HASH_BYTES * (3*i + 2), HASH_BYTES);
        }
        sm += 2*N_BYTES + M_BYTES + HASH_BYTES;
    }
    *smlen = SIG_LEN + mlen;
    return 0;
}

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    const unsigned char *r, *t, *e;
    unsigned char F[F_BYTES];
    unsigned char D_sigma0[HASH_BYTES * 2];  // Concatenated for convenient H().
    unsigned char *D = D_sigma0;
    unsigned char *sigma0 = D_sigma0 + HASH_BYTES;
    unsigned char c[HASH_BYTES * ROUNDS * 3];
    unsigned char x[N_BYTES];
    unsigned char y[M_BYTES];
    unsigned char z[M_BYTES];
    unsigned char h[HASH_BYTES];
    int ch, i, j, ch_count = 0;
    Keccak_HashInstance keccak;

    memcpy(m, sm, HASH_BYTES);  // Copy R to m.
    memcpy(m + HASH_BYTES, sm + SIG_LEN, smlen - SIG_LEN); // Copy message.
    H(D, m, smlen - SIG_LEN + HASH_BYTES);
    sm += HASH_BYTES;

    expand_seed(F, F_BYTES, pk, SEED_BYTES);
    pk += SEED_BYTES;

    memcpy(sigma0, sm, HASH_BYTES);
    sm += HASH_BYTES;

    Keccak_HashInitialize_SHAKE128(&keccak);
    Keccak_HashUpdate(&keccak, D_sigma0, 2 * HASH_BYTES * 8);
    Keccak_HashFinal(&keccak, h);
    Keccak_HashSqueeze(&keccak, h, HASH_BYTES * 8);

    for (i = 0; i < ROUNDS; i++) {
        do {
            ch = h[ch_count >> 2] >> ((ch_count & 3) << 1) & 3;
            ch_count++;
            if (ch_count == HASH_BYTES * 4) {
                ch_count = 0;
                Keccak_HashSqueeze(&keccak, h, HASH_BYTES * 8);
            }
        } while (ch == 3);

        r = sm;
        t = sm + N_BYTES;
        e = sm + 2*N_BYTES;
        sm += 2*N_BYTES + M_BYTES;

        if (ch == 0) {
            for (j = 0; j < N_BYTES; j++) {
                x[j] = r[j] ^ t[j];
            }
            MQ_asm(y, r, F);

            for (j = 0; j < M_BYTES; j++) {
                y[j] ^= e[j];
            }
            com(c + HASH_BYTES * (3*i + 1), x, y);
            com(c + HASH_BYTES * (3*i + 2), t, e);
        }
        else if (ch == 1) {
            MQ_asm(y, r, F);
            G_asm(z, t, r, F);
            for (j = 0; j < M_BYTES; j++) {
                y[j] ^= pk[j] ^ z[j] ^ e[j];
            }
            com(c + HASH_BYTES * (3*i + 0), r, y);
            com(c + HASH_BYTES * (3*i + 2), t, e);
        }
        else if (ch == 2) {
            G_asm(y, t, r, F);
            for (j = 0; j < M_BYTES; j++) {
                y[j] ^= e[j];
            }
            com(c + HASH_BYTES * (3*i + 0), r, y);
            com(c + HASH_BYTES * (3*i + 1), t, e);
        }

        memcpy(c + HASH_BYTES * (3*i + ch), sm, HASH_BYTES);
        sm += HASH_BYTES;
    }

    H(c, c, HASH_BYTES * ROUNDS * 3);
    if (memcmp(c, sigma0, HASH_BYTES)) {
        return 1;
    }

    *mlen = smlen - SIG_LEN;
    memcpy(m, sm, smlen - SIG_LEN);
    return 0;
}
