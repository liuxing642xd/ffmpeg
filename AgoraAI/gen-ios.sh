#!/bin/bash
build_path=build/ios
TARGET_ARCH="ARMEABI_V7"

while [[ $# -gt 0 ]]
do
key="$1"
case $key in
    sim)
    echo "set target arch x86_64"
    TARGET_ARCH="X86_64"
    ;;
    *)
    #unknow options
    ;;
esac
shift
done

rm -rf $build_path
mkdir -p $build_path || exit 1
cd $build_path || exit 1

cmake -D${TARGET_ARCH}:BOOL=true \
      -DCMAKE_TOOLCHAIN_FILE=../../proj.cmake/ios.toolchain.cmake \
      -DCMAKE_CXX_FLAGS="-std=c++11 -O2 -fno-exceptions -fno-rtti -fPIE \
                      -fno-bounds-check -funroll-loops -funswitch-loops \
                      -fvisibility=hidden -finline-functions -funsafe-loop-optimizations" \
      -G Xcode ../../proj.cmake || exit 1

cd ../..
