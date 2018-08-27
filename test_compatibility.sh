#!/bin/bash

make -C ref {test/sign,test/open,test/keypair,test/mq}
make -C avx2 {test/sign,test/open,test/keypair,test/mq}

N=48
SEED_BYTES=16

keyfile=$(mktemp)
mfile=$(mktemp)
smfile=$(mktemp)
for keygen in {"ref","avx2"}; do
    for signer in {"ref","avx2"}; do
        for verifier in {"ref","avx2"}; do
            echo -n "Testing with keygen: ${keygen}, signer: ${signer}, verifier: ${verifier}.. "
            ${keygen}/test/keypair > ${keyfile}
            head -c 32 /dev/urandom > ${mfile}
            tail -c ${SEED_BYTES} ${keyfile} | cat - ${mfile} | ${signer}/test/sign > ${smfile}
            head -c $((SEED_BYTES + N * 5 / 8)) ${keyfile} | cat - ${smfile} | ${verifier}/test/open
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

mqfile_in=$(mktemp)
mqfile_ref=$(mktemp)
mqfile_avx2=$(mktemp)

# input is 40 bytes of packed x, 32 bytes of Fseed
head -c $((SEED_BYTES + N * 5 / 8)) /dev/urandom > ${mqfile_in}
ref/test/mq < ${mqfile_in} > ${mqfile_ref}
avx2/test/mq < ${mqfile_in} > ${mqfile_avx2}
echo -n "Testing if ref/mq and avx2/mq are identical.. "
cmp --silent ${mqfile_ref} ${mqfile_avx2} && echo "succeeded!" || echo "failed!"

rm ${mqfile_in}
rm ${mqfile_ref}
rm ${mqfile_avx2}
