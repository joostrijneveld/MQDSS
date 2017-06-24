## Compilation instructions

To compile the code, simply change into the 3pass or 5pass directory, and run:

    `make`

This also attempts to download and compile the Keccak Code Package if it is not
yet available. To avoid this, simply place it in the root directory first.

By default, the code relies on the availability of the AVX2 instruction set.
In particular, it assumes the Haswell platform. To switch to a different
platform, toggle line 4 and 5 of the respective Makefiles.

## Troubleshooting

Note that the Keccak Code Package depends on gcc, GNU Make and xsltproc. These
should be readily available through your package manager. Downloading the
KCP via git (as is done when calling `make`) requires git to be installed.

## Testing

To run the unit tests, simply call ./test.sh in the 3pass or 5pass directories.

## Benchmarking

To run measurements, execute any binaries produced in the measurements directories.
