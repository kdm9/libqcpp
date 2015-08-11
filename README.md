libqc++
=======


A C++11 library for sequence quality control.

Design features:

- Modularity
- Parallelisation
- Pipelined
- Simple API

Installation
------------

On Debian-like systems:

    sudo apt-get install libyaml-cpp-dev cmake build-essential
    git clone https://github.com/kdmurray91/libqcpp.git
    cd libqcpp
    mkdir build && cd build
    cmake ..
    make
    make install

License
-------

All code is licensed under the GNU Lesser General Public License, version 3 or
at your option any later version.

Catch.hpp, seqan and libssw are licensed under their respective licenses, see
files for more information
