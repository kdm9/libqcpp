#########
# SETUP #
#########
set -e
source /hbb_exe_gc_hardened/activate
set -x

export QCPP_VER=$1

cd $(mktemp -d)

########
# DEPS #
########

# Install libbz2
tar xf /io/deps/libbz2.tar*
pushd bzip2*/
make CFLAGS=-fPIC
make install
popd
rm -rf boost*/

# Install boost headers only
tar xf /io/deps/boost.tar* boost_1_60_0/boost/
mv boost_1_*/boost /usr/local/include
rm -rf boost*/

# Compile & install yaml-cpp 
tar xf /io/deps/yaml-cpp.tar*
pushd yaml-cpp*/
mkdir build && cd build
cmake .. -DYAML_CPP_BUILD_TOOLS=Off -DYAML_CPP_BUILD_CONTRIB=Off
make -j4
make install
popd
rm -rf yaml-cpp*/


#########
# BUILD #
#########

prefix=/io/libqcpp_${QCPP_VER}_amd64
mkdir -p $prefix

builddir=$(mktemp -d)
pushd $builddir

cmake /io -DCMAKE_INSTALL_PREFIX=$prefix
make -j4
make test
make install
popd

chown 1000:1000 -R $prefix
