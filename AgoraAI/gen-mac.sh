#!/bin/bash
binary_dir=build/mac

rm -rf $binary_dir
mkdir -p $binary_dir
cd $binary_dir

cmake \
    -DCMAKE_TOOLCHAIN_FILE=../../proj.cmake/mac.toolchain.cmake \
    -DCMAKE_CXX_FLAGS="-std=c++11 -O2 -fno-exceptions -fno-rtti -fPIE \
                      -fno-bounds-check -funroll-loops -funswitch-loops \
                      -fvisibility=hidden -finline-functions -funsafe-loop-optimizations" \
    -DX86_64:BOOL=true  -G Xcode ../../proj.cmake || exit 1

cd ../..