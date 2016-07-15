#ifndef PARAMS_H
#define PARAMS_H

#define N 256
#define M 256
#define F_LEN (M * ((N * (N + 1)) >> 1))

#define ROUNDS 438

#define N_BYTES (N >> 3)
#define M_BYTES (M >> 3)
#define F_BYTES (F_LEN >> 3)

#define HASH_BYTES 32
#define SEED_BYTES 32
#define PK_BYTES (HASH_BYTES + M_BYTES)
#define SK_BYTES SEED_BYTES

#define SIG_LEN (2 * HASH_BYTES + ROUNDS * (2*N_BYTES + M_BYTES + HASH_BYTES))

#endif
