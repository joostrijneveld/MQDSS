#ifndef MQ_H
#define MQ_H

#ifdef REFERENCE
extern void MQ_asm(unsigned char *fx, const unsigned char *x,
                   const unsigned char *F);

extern void G_asm(unsigned char *fx,
                  const unsigned char *x, const unsigned char *y,
                  const unsigned char *F);
#else
void MQ_asm(unsigned char *fx, const unsigned char *x, const unsigned char *F);

void G_asm(unsigned char *fx, const unsigned char *x, const unsigned char *y,
                  const unsigned char *F);
#endif

void ROL_NBYTES(unsigned char *out, const unsigned char *in);

void MQ(unsigned char *fx, const unsigned char *x, const unsigned char *F);

void G(unsigned char *gx, const unsigned char *x, const unsigned char *y,
       const unsigned char *F);

#endif