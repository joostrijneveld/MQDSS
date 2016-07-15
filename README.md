## Compilation instructions

To compile the code, we also need the keccak code package. To clone the KCP:

        `git clone https://github.com/gvanas/KeccakCodePackage`

    Note that this codebase was last tested against commit f40509b of the KCP,
    so in order to get to that specific state, execute in the KCP directory:

        `git reset --hard f40509b`

Or if you obtained this README via git, retrieve the KCP as a submodule:

        `git submodule update --init`

Then in order to build the Keccak library file, execute in the KCP directory:

        `make Haswell/libkeccak.a`

Finally, in the 3pass or 5pass directory, run:

        `make`

## Troubleshooting

Note that the Keccak code package depends on gcc, GNU Make and xsltproc.
These should be readily available through your package manager.

## Testing

To run the unit tests, simply call ./test.sh in the 3pass or 5pass directories.

## Benchmarking

To run measurements, execute any binaries produced in the measurements directories.
