#!/bin/bash

wdir=`pwd`

rm -rf build
mkdir build && cd build

cmake $wdir
make
