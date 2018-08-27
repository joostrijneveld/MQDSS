#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../params.h"
#include "../randombytes.h"
#include "../api.h"

#define NTESTS 1000
#define MLEN 32

static unsigned long long cpucycles(void)
{
  unsigned long long result;
  __asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
    : "=a" (result) ::  "%rdx");
  return result;
}

static int cmp_llu(const void *a, const void*b)
{
    if (*(unsigned long long *)a < *(unsigned long long *)b) return -1;
    if (*(unsigned long long *)a > *(unsigned long long *)b) return 1;
    return 0;
}

static unsigned long long median(unsigned long long *l, size_t llen)
{
    qsort(l, llen, sizeof(unsigned long long), cmp_llu);

    if (llen % 2) return l[llen / 2];
    else return (l[llen/2 - 1] + l[llen/2]) / 2;
}

static unsigned long long average(unsigned long long *t, size_t tlen)
{
    unsigned long long acc=0;
    size_t i;
    for(i = 0; i < tlen; i++) {
        acc += t[i];
    }
    return acc/(tlen);
}

static void print_results(const char *s, unsigned long long *t, size_t tlen, int mult)
{
  size_t i;
  printf("%s", s);
  for (i = 0; i < tlen-1; i++) {
    t[i] = t[i+1] - t[i];
  }
  printf("\n");
  printf("median        : %llu\n", median(t, tlen));
  printf("average       : %llu\n", average(t, tlen-1));
  if (mult > 1) {
    printf("median  (%3dx): %llu\n", mult, mult*median(t, tlen));
    printf("average (%3dx): %llu\n", mult, mult*average(t, tlen-1));
  }
  printf("\n");
}

int main()
{
    unsigned long long t[NTESTS];

    unsigned char sk[CRYPTO_SECRETKEYBYTES] __attribute__ ((aligned (32)));
    unsigned char pk[CRYPTO_PUBLICKEYBYTES] __attribute__ ((aligned (32)));

    unsigned char m[MLEN];
    unsigned char sm[MLEN+CRYPTO_BYTES];
    unsigned char m_out[MLEN+CRYPTO_BYTES];
    unsigned long long mlen;
    unsigned long long smlen;

    int i;

    randombytes(m, MLEN);

    printf("-- api --\n\n");

    for(i=0; i<NTESTS; i++) {
        t[i] = cpucycles();
        crypto_sign_keypair(pk, sk);
    }
    print_results("crypto_sign_keypair: ", t, NTESTS, 1);

    for(i=0; i<NTESTS; i++) {
        t[i] = cpucycles();
        crypto_sign(sm, &smlen, m, MLEN, sk);
    }
    print_results("crypto_sign: ", t, NTESTS, 1);

    for(i=0; i<NTESTS; i++) {
        t[i] = cpucycles();
        crypto_sign_open(m_out, &mlen, sm, smlen, pk);
    }
    print_results("crypto_sign_open: ", t, NTESTS, 1);

    return 0;
}
