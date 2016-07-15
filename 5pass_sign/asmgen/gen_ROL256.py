
p = print


def debug(n):
    p("vmovdqu %ymm{}, (%rdi)".format(n))  # Write result to out.
    p("mov %r8, %rsp")
    p("ret")
    import sys
    sys.exit()


def ROL256(reg, t0, t1, out, shl):
    if shl > 63:
        raise Exception("Cannot ROL256 more than 63 bits")
    p("vpsllq ${}, %ymm{}, %ymm{}".format(shl, reg, t0))
    p("vpsrlq ${}, %ymm{}, %ymm{}".format(64-shl, reg, t1))
    # TODO figure out if there is a cycle count difference with vpermpd.
    p("vpermq $147, %ymm{}, %ymm{}".format(t1, t1))
    # Maybe this OR can be combined with whatever comes after,
    # as the separate components could also be e.g. XORed into a destination.
    p("vpor %ymm{}, %ymm{}, %ymm{}".format(t0, t1, out))


if __name__ == '__main__':
    p(".global ROL256_asm")
    p(".att_syntax prefix")
    p("")
    p("ROL256_asm:")
    p("")

    p("vmovdqu (%rsi), %ymm0")  # Store original x in ymm0.
    ROL256(0, 1, 2, 3, 1)
    p("vmovdqu %ymm3, (%rdi)")  # Write result to out.

    p("ret")
