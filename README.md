## MQDSS

The code in this branch of the repository corresponds to the [MQDSS submission](http://mqdss.org) to [NIST's Post-Quantum Cryptography project](https://csrc.nist.gov/Projects/Post-Quantum-Cryptography/Round-1-Submissions). It starts off from the code as submitted to NIST, and contains a few fixes for bugs that surfaced since then.

### Parameters

To switch between the MQDSS-48 and MQDSS-64 parameter sets, we adjust the relevant parameters in `params.h` accordingly. In particular, this concerns `M = N` and `ROUNDS`, as well as `SEED_BYTES` and `HASH_BYTES` (which should be set to `48`, `135`, `16` and `32` versus `64`, `202`, `24` and `48` for the respective parameter sets). The recommended parameter sets are predefined in the `params/` directory. These parameters are motivated in detail in [the specification document](http://mqdss.org/specification.html).

### License

All included code is available under the CC0 1.0 Universal Public Domain Dedication.
