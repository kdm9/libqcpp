#!/bin/bash
/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#########
# SETUP #
#########
set -e
source /hbb_exe/activate
set -x

QCPP_VERSION=$1

YAMLCPP_VER=0.5.3

cd $(mktemp -d)
export  CMAKE_PREFIX_PATH=/hbb_exe

########
# DEPS #
########

# Compile & install yaml-cpp
curl -LSO https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-${YAMLCPP_VER}.tar.gz
tar xf yaml-cpp*.tar.gz
pushd yaml-cpp*/
mkdir build && cd build
cmake ..                       \
    -DYAML_CPP_BUILD_TOOLS=Off \
    -DYAML_CPP_BUILD_CONTRIB=Off
make -j4
make install
popd
rm -rf yaml-cpp*

#########
# BUILD #
#########


prefix=/io/static_build
rm -rf $prefix
mkdir -p $prefix

# Clone to builddir
builddir=$(mktemp -d)
pushd $builddir
tar xvf /io/libqcpp_${QCPP_VERSION}.tar

mkdir build
cd build

cmake $builddir                    \
    -DCMAKE_INSTALL_PREFIX=$prefix \
    -DSTATIC_BINARIES=On           \
    -DQCPP_VERSION=$QCPP_VERSION
make -j4 VERBOSE=1
make test
make install
