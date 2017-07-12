#!/bin/bash

# make -C ref/c {test/sign,test/open,test/keypair}
# make -C avx2 {test/sign,test/open,test/keypair}
make {test/sign,test/open,test/keypair}

keyfile=$(mktemp)
mfile=$(mktemp)
smfile=$(mktemp)
# for keygen in {"ref/c","avx2"}; do
    # for signer in {"ref/c","avx2"}; do
        # for verifier in {"ref/c","avx2"}; do
for keygen in {".",}; do
    for signer in {".",}; do
        for verifier in {".",}; do
            echo -n "Testing with keygen: ${keygen}, signer: ${signer}, verifier: ${verifier}.. "
            ${keygen}/test/keypair > ${keyfile}
            head -c 32 /dev/urandom > ${mfile}
            tail -c 64 ${keyfile} | cat - ${mfile} | ${signer}/test/sign > ${smfile}
            head -c 72 ${keyfile} | cat - ${smfile} | ${verifier}/test/open
            if [ $? -eq 0 ]
            then
                echo "succeeded!"
            else
                echo "failed!"
            fi
        done
    done
done

rm ${keyfile}
rm ${mfile}
rm ${smfile}
