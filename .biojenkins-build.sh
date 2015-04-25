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

# test parsing of valid file using qcparse
./bin/qcparse ./data/valid.fastq >$tmpdir/valid.fastq
cmp --silent ./data/valid.fastq $tmpdir/valid.fastq

# clean up from above tests
rm -rf $tmpdir
