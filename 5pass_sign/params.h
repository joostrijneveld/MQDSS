#ifndef PARAMS_H
#define PARAMS_H

#define N 64
#define M 64
#define F_LEN (M * (((N * (N + 1)) >> 1) + N))

#define ROUNDS 269

#define NPACKED_BYTES ((N * 5) >> 3)
#define MPACKED_BYTES ((M * 5) >> 3)
#define FPACKED_BYTES ((F_LEN * 5) >> 3)

#define HASH_BYTES 32
#define SEED_BYTES 32
#define PK_BYTES (HASH_BYTES + MPACKED_BYTES)
#define SK_BYTES SEED_BYTES

// R, sigma_0, ROUNDS * (t1, r{0,1}, e1, c)
#define SIG_LEN (2 * HASH_BYTES + ROUNDS * (2*NPACKED_BYTES + MPACKED_BYTES + HASH_BYTES))

#endif
