#!/bin/bash

make -C ref/c {test/sign,test/open,test/keypair,test/mq}
make -C avx2 {test/sign,test/open,test/keypair,test/mq}

keyfile=$(mktemp)
mfile=$(mktemp)
smfile=$(mktemp)
for keygen in {"ref/c","avx2"}; do
    for signer in {"ref/c","avx2"}; do
        for verifier in {"ref/c","avx2"}; do
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

mqfile_in=$(mktemp)
mqfile_refc=$(mktemp)
mqfile_avx2=$(mktemp)

# input is 40 bytes of packed x, 32 bytes of Fseed
head -c 72 /dev/urandom > ${mqfile_in}
ref/c/test/mq < ${mqfile_in} > ${mqfile_refc}
avx2/test/mq < ${mqfile_in} > ${mqfile_avx2}
echo -n "Testing if ref/c/mq and avx2/mq are identical.. "
cmp --silent ${mqfile_refc} ${mqfile_avx2} && echo "succeeded!" || echo "failed!"

rm ${mqfile_in}
rm ${mqfile_refc}
rm ${mqfile_avx2}
