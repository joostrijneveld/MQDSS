## MQDSS

The code in this branch of the repository corresponds to the MQDSS submission to [NIST's Post-Quantum Cryptography project](https://csrc.nist.gov/Projects/Post-Quantum-Cryptography/Round-1-Submissions). It starts off from the code as submitted to NIST, and contains a few fixes for bugs that surfaced since then.

### Parameters

To switch between the MQDSS-48 and MQDSS-64 parameter sets, adjust the relevant parameters in `params.h` accordingly. In particular, this concerns `M = N` and `ROUNDS`, as well as `HASH_BYTES` and `SEED_BYTES` (which should be set to `48`, `269` and `32` versus `64`, `403` and `48` for the respective parameter sets). These parameters are motivated in detail in the specification document accompanying [the submission to NIST](https://csrc.nist.gov/CSRC/media/Projects/Post-Quantum-Cryptography/documents/round-1/submissions/MQDSS.zip).

### License

All included code is available under the CC0 1.0 Universal Public Domain Dedication.
