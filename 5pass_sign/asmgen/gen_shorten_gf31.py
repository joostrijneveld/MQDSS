
p = print


def shorten_gf31(x, c=14):
    p("vpand %ymm{}, %ymm13, %ymm{}".format(x, c))
    p("vpsraw $5, %ymm{}, %ymm{}".format(x, x))
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(x, c, x))


def signed_shorten_gf31(x, c=14):
    p("vpaddw %ymm15, %ymm{}, %ymm{}".format(x, x))
    p("vpand %ymm{}, %ymm13, %ymm{}".format(x, c))
    p("vpsraw $5, %ymm{}, %ymm{}".format(x, x))
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(x, c, x))
    p("vpsubw %ymm15, %ymm{}, %ymm{}".format(x, x))

if __name__ == '__main__':
    p(".data")
    p("gf31:")
    for i in range(16):
        p(".word 31")

    p("all16:")
    for i in range(16):
        p(".word 16")

    p(".text")
    p(".global shorten_gf31_asm")
    p(".att_syntax prefix")
    p("")
    p("shorten_gf31_asm:")
    p("")

    p("vmovdqu gf31, %ymm13")
    p("vmovdqu all16, %ymm15")

    p("vmovdqu (%rsi), %ymm0")  # Store original x in ymm0.
    shorten_gf31(0)
    p("vmovdqu %ymm0, (%rdi)")  # Write result to out.

    p("ret")

    p(".global signed_shorten_gf31_asm")
    p(".att_syntax prefix")
    p("")
    p("signed_shorten_gf31_asm:")
    p("")

    p("vmovdqu gf31, %ymm13")
    p("vmovdqu all16, %ymm15")

    p("vmovdqu (%rsi), %ymm0")  # Store original x in ymm0.
    signed_shorten_gf31(0)
    p("vmovdqu %ymm0, (%rdi)")  # Write result to out.

    p("ret")
