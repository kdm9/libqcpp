libqc++
=======

[![Build
Status](https://travis-ci.org/kdmurray91/libqcpp.svg?branch=master)](https://travis-ci.org/kdmurray91/libqcpp)
[![Documentation
Status](https://readthedocs.org/projects/qcpp/badge/?version=latest)](http://qcpp.readthedocs.org/en/latest/?badge=latest)

A C++11 library for sequence quality control.

Design features:

- Modularity
- Parallelisation
- Pipelined
- Simple API

Installation
------------

Statically compiled libraries and binaries are available from the
[GitHub releases page](https://github.com/kdmurray91/libqcpp/releases).

These can be uncompressed then copied to, for instance, `/usr/local/` or `~/`.

### From Source:

On Debian-like systems:

    sudo apt-get install git libboost-dev libyaml-cpp-dev zlib1g-dev \
                         libbz2-dev cmake build-essential
    git clone https://github.com/kdmurray91/libqcpp.git
    cd libqcpp
    mkdir build && cd build
    cmake ..
    make
    make install

License
-------

All libqcpp code is licensed under the Mozilla Public Licence, version 2.0.

Catch.hpp, seqan and libssw are licensed under their respective licenses, see
their LICENSE files for more information.
