#!/usr/bin/env python3
from gen_ROL256 import ROL256, debug
from gen_shorten_gf31 import shorten_gf31, signed_shorten_gf31
""" This generates assembly code that implements the G polar form function.

Input should be in the domain [-16, 15] for x and y, and [-15, 15] for F.
Output will be in [0, 31].
"""

p = print

p(".data")
p("gf31:")
for i in range(16):
    p(".word 31")

p("all16:")
for i in range(16):
    p(".word 16")

p(".text")
p(".global G_asm")
p(".att_syntax prefix")
p("")
p("G_asm:")
p("")

# we must preserve: %rbx, %rbp, r12-r15
# rdi contains param 0 (fx_asm)
# rsi contains param 1 (x)
# rdx contains param 2 (y)
# rcx contains param 3 (F)
F_reg = 'rcx'

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

for i in range(4):
    p("vmovdqu {}(%rdx), %ymm{}".format(32 * i, i + 4))

F_byte = 4 * 64 * 32  # Skip the monomials, as they cancel in G.

# Add the rectangle of binomials
for reg in range(4):
    srcreg_x = reg
    srcreg_y = reg+4
    for row in range(8):
        # For all registers.. (prevent symmetry for row=0 (i.e. no ROL) here)
        for reg2 in range(0 if row > 0 else reg, 4):
            p("vpmullw %ymm{}, %ymm{}, %ymm9".format(srcreg_x, reg2+4))
            p("vpmullw %ymm{}, %ymm{}, %ymm11".format(srcreg_y, reg2))
            p("vpaddw %ymm9, %ymm11, %ymm11")
            shorten_gf31(11)
            signed_shorten_gf31(11)
            for acc in range(64):
                p("vpmullw {}(%{}), %ymm11, %ymm10".format(F_byte, F_reg))
                # TODO see if we can combine the above using VPMACSSWW
                if reg + row + reg2 > 0:
                    p("vpaddw {}(%rsp), %ymm10, %ymm10".format(acc * 32))  # ADD into accumulator.
                # very aggressive reducing (i.e. can be less frequent)
                p("vmovdqa %ymm10, {}(%rsp)".format(acc * 32))
                F_byte += 32
        if row < 7:
            ROL256(srcreg_x, 10, 11, 8, 16)
            ROL256(srcreg_y, 10, 11, 12, 16)
        srcreg_x = 8
        srcreg_y = 12
p("vmovdqa {}(%rsp), %ymm10".format(acc * 32))

# This combines the last half-row with accumulator processing
for highreg in range(4):  # For all high parts of the registers
    p("vextracti128 $1, %ymm{}, %xmm8".format(highreg))
    p("vextracti128 $1, %ymm{}, %xmm12".format(highreg+4))
    for lowreg in range(4):
        p("vpmullw %xmm8, %xmm{}, %xmm9".format(lowreg+4))
        p("vpmullw %xmm12, %xmm{}, %xmm10".format(lowreg))
        p("vpaddw %xmm9, %xmm10, %xmm9".format(lowreg))
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
