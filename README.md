## MQDSS

The code in this repository is part of the work presented in the paper _"From
5-pass MQ-based identification to MQ-based signatures"_. When referring to this
implementation, please refer to the original publication:

> Ming-Shing Chen, Andreas Hülsing, Joost Rijneveld, Simona Samardjiska, and Peter Schwabe. From 5-pass MQ-based identification to MQ-based signatures. _Advances in Cryptology – ASIACRYPT 2016_, LNCS 10032, pp. 135-165, Springer, 2016. https://joostrijneveld.nl/papers/mqdss

Note that the scheme implemented in this repository differs slightly from the MQDSS scheme that was submitted to [NIST's Post-Quantum Cryptography project](https://csrc.nist.gov/Projects/Post-Quantum-Cryptography/Round-1-Submissions) in November 2017, making the two subtly incompatible. The basis for the code submitted to NIST can be found in the `NIST` branch of this repository.

## Compilation instructions

To compile the code, change into the `3pass`, `5pass/ref/c` or `5pass/avx2` directory, and run `make`.

## Testing

To run the unit tests, call `./test.sh` in the 3pass or 5pass directories.

## Benchmarking

To run measurements, execute any binaries produced in the measurements directories.
