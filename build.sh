#!/bin/sh

set -e

echo "Building libtins"

pushd libtins
mkdir build
cd build
cmake .. -DLIBTINS_BUILD_SHARED=0
make
popd

echo "Building data-injector"

mkdir build
cd build
cmake ..
make
