#!/usr/bin/env python3
""" This generates assembly code that implements the MQ function.

Input should be in the domain [-16, 15] for x, and [-15, 15] for F.
Output will be in [0, 31].
"""

from gen_ROL256 import ROL256, debug
from gen_shorten_gf31 import shorten_gf31, signed_shorten_gf31

p = print

p(".data")
p("gf31:")
for i in range(16):
    p(".word 31")

p("all16:")
for i in range(16):
    p(".word 16")

p(".text")
p(".global MQ_asm")
p(".att_syntax prefix")
p("")
p("MQ_asm:")
p("")

# we must preserve: %rbx, %rbp, r12-r15
# rdi contains param 0 (fx_asm)
# rsi contains param 1 (x)
# rdx contains param 2 (F)
F_reg = 'rdx'

p("mov %rsp, %r8")  # Use r8 to store the old stack pointer during execution.
p("andq $-32, %rsp")  # Align rsp to the next 32-byte value, for vmovdqa.
p("subq ${}, %rsp".format(64 * 32))  # Allocate 64 32-byte accumulators.

# Store original x in ymm0 to ymm3
# Leave ymm4-ymm7 empty (for symmetry with computation of G)
# Store rotated variant in ymm8
# Store multiplication result in ymm9
# Use ymm10 and ymm11 for intermediate results
# ymm12 is free for rotated variant of G
# Store 31 in ymm13 for easy shortening
# Store 15 in ymm15 for signed shortening
# ymm14 for reduction

p("vmovdqu gf31, %ymm13")
p("vmovdqu all16, %ymm15")

for i in range(4):
    p("vmovdqu {}(%rsi), %ymm{}".format(32 * i, i))

F_byte = 0

# Add the monomials.
for reg in range(4):
    for acc in range(64):
        if reg == 0:
            p("vpmullw {}(%{}), %ymm{}, %ymm10".format(F_byte, F_reg, reg))
            p("vmovdqa %ymm10, {}(%rsp)".format(acc * 32))
        else:
            p("vpmullw {}(%{}), %ymm{}, %ymm10".format(F_byte, F_reg, reg))
            p("vpaddw {}(%rsp), %ymm10, %ymm10".format(acc * 32))
            p("vmovdqa %ymm10, {}(%rsp)".format(acc * 32))
        F_byte += 32

# Add the rectangle of binomials
for reg in range(4):
    srcreg = reg
    for row in range(8):
        # For all registers.. (prevent symmetry for row=0 (i.e. no ROL) here)
        for reg2 in range(0 if row > 0 else reg, 4):
            p("vpmullw %ymm{}, %ymm{}, %ymm9".format(srcreg, reg2))
            shorten_gf31(9)
            signed_shorten_gf31(9)
            for acc in range(64):
                p("vpmullw {}(%{}), %ymm9, %ymm10".format(F_byte, F_reg))
                p("vpaddw {}(%rsp), %ymm10, %ymm10".format(acc * 32))  # ADD into accumulator.
                p("vmovdqa %ymm10, {}(%rsp)".format(acc * 32))
                F_byte += 32
        if row < 7:
            ROL256(srcreg, 10, 11, 8, 16)
        srcreg = 8

# This combines the last half-row with accumulator processing
for highreg in range(4):  # For all high parts of the registers
    p("vextracti128 $1, %ymm{}, %xmm8".format(highreg))
    for lowreg in range(4):
        p("vpmullw %xmm8, %xmm{}, %xmm9".format(lowreg))  # Zeros top half of ymm9
        shorten_gf31(9)
        signed_shorten_gf31(9)

        for acc in range(64):
            p("vpmullw {}(%{}), %xmm9, %xmm10".format(F_byte, F_reg))
            p("vpaddw {}(%rsp), %ymm10, %ymm10".format(acc * 32))  # ADD into accumulator.
            F_byte += 16
            if highreg < 3 or lowreg < 3:  # Accumulator is not yet complete
                p("vmovdqa %ymm10, {}(%rsp)".format(acc * 32))
            else:
                shorten_gf31(10)
                p("vextracti128 $1, %ymm10, %xmm11")
                p("vpaddw %xmm10, %xmm11, %xmm10")  # Add high 8 words to low

                p("vmovhlps %xmm10, %xmm0, %xmm11")  # Second input is ignored
                p("vpaddw %xmm10, %xmm11, %xmm10")  # Add high 4 words to low

                # TODO maybe this can be done faster using vector shifts more effectively
                p("vpsrldq $4, %xmm10, %xmm11")  # Truncate low 2 words
                p("vpaddw %xmm10, %xmm11, %xmm10")  # Add high 2 words to low

                p("vpsrldq $2, %xmm10, %xmm11")  # Truncate low word
                p("vpaddw %xmm10, %xmm11, %xmm10")  # Add high word to low
                shorten_gf31(10)
                shorten_gf31(10)
                shorten_gf31(10)

                if acc % 4 == 0:
                    p("vmovq %xmm10, %r9")
                    p("shl ${}, %r9".format(16 * 3))
                elif acc % 4 in [1, 2]:
                    p("vmovq %xmm10, %rax")
                    p("and $65535, %rax")
                    p("shl ${}, %rax".format(16 * (3 - (acc % 4))))
                    p("or %rax, %r9")
                else:
                    p("vmovq %xmm10, %rax")
                    p("and $65535, %rax")
                    p("or %rax, %r9")
                    p("mov %r9, {}(%rdi)".format(8 * (acc // 4)))

p("mov %r8, %rsp")
p("vzeroupper")
p("ret")
