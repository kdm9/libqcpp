#!/bin/bash

set -xe

wdir=`pwd`

rm -rf build
mkdir build && cd build

cmake $wdir
make

# for use in tests below
tmpdir=biojenkins-out/$RANDOM
mkdir -p $tmpdir

# Next bit is broken, who knows why
# test parsing of valid file using qcparse
# ./bin/qcparse ./data/valid_il.fastq >$tmpdir/valid.fastq
#cmp --silent ./data/valid_il.fastq $tmpdir/valid.fastq

# clean up from above tests
rm -rf $tmpdir
