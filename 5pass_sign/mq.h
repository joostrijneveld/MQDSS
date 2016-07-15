#ifndef MQ_H
#define MQ_H

#include "gf31.h"

extern void MQ_asm(gf31 *fx, const gf31 *x, const gf31 *F);

extern void G_asm(gf31 *fx, const gf31 *x, const gf31 *y, const gf31 *F);

#endif