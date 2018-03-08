#ifndef MQDSS_RANDOMBYTES_H
#define MQDSS_RANDOMBYTES_H

/* Reads xlen random bytes from /dev/urandom and writes them to x */
extern void randombytes(unsigned char *, unsigned long long);

#endif
