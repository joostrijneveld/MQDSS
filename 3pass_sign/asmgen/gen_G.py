#!/usr/bin/env python3
from gen_ROL256 import ROL256

p = print

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

p("mov %rsp, %r8")  # Use r8 to store the old stack pointer during execution.
p("andq $-32, %rsp")  # Align rsp to the next 32-byte value, for vmovdqa.
p("subq ${}, %rsp".format(256 * 32))  # Allocate 256 32-byte accumulators.
p("vmovdqu (%rsi), %ymm0")  # Store original x in ymm0.
p("vmovdqu (%rdx), %ymm4")  # Store original y in ymm4.

for i in range(1, 128):  # We completely skip i == 0, since xiyi ^ xiyi == 0
    ROL256(0 if i == 1 else 1, 2, 3, 1, 1)
    ROL256(4 if i == 1 else 5, 6, 7, 5, 1)
    p("vpand %ymm{}, %ymm{}, %ymm{}".format(4, 1, 2))
    p("vpand %ymm{}, %ymm{}, %ymm{}".format(0, 5, 6))
    p("vpxor %ymm{}, %ymm{}, %ymm{}".format(6, 2, 2))
    for j in range(256):
        p("vpand {}(%rcx), %ymm2, %ymm3".format((i * 256 + j) * 32))  # AND with F.
        if i > 1:
            p("vpxor {}(%rsp), %ymm3, %ymm3".format(j * 32))  # XOR into accumulator.
        p("vmovdqa %ymm3, {}(%rsp)".format(j * 32))
# The last row is only half-full, so that's a special case.
p("vextracti128 $1, %ymm0, %xmm1")  # This conveniently clears the top half of ymm1.
p("vextracti128 $1, %ymm4, %xmm5")  # This conveniently clears the top half of ymm5.
p("vpand %ymm{}, %ymm{}, %ymm{}".format(4, 1, 1))
p("vpand %ymm{}, %ymm{}, %ymm{}".format(0, 5, 5))
p("vpxor %ymm{}, %ymm{}, %ymm{}".format(1, 5, 1))

for i in range(256):
    p("vpand {}(%rcx), %xmm1, %xmm0".format((128 * 256 * 32) + i * 16))  # AND with F.
    p("vpxor {}(%rsp), %ymm0, %ymm0".format(i * 32))  # XOR into accumulator.
    # Now we need to compute the parity of each entry of the F-results.
    # Compute parity by doubling up on itself until quadword, which we can popcnt.
    p("vextracti128 $1, %ymm0, %xmm2")
    p("vpxor %xmm0, %xmm2, %xmm0")  # XOR low 128 with high 128.
    p("vmovhlps %xmm0, %xmm2, %xmm2")
    p("vpxor %xmm0, %xmm2, %xmm0")  # XOR high 64 with low 64.
    # Store in memory so we can continue doing VEX instructions
    p("vmovq %xmm0, %rax")
    p("vzeroupper")

    if i % 64 == 0:
        p("popcnt %rax, %r9")
        p("and $1, %r9")
    else:
        p("popcnt %rax, %rax")
        p("and $1, %rax")  # Parity is now the lowest bit, so mask the rest.
        # Now insert it into fx in the right position.
        p("shl ${}, %rax".format(i % 64))
        p("or %rax, %r9")
    if i % 64 == 63:
        p("mov %r9, {}(%rdi)".format(8 * (i // 64)))

p("mov %r8, %rsp")
p("ret")
