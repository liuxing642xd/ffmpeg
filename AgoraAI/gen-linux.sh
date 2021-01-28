#! /bin/bash -e
rm -rf build/linux
mkdir -p build/linux
cd build/linux
if [ $CC ] && [ $CXX ]; then
  if [ $(basename $CC) == gcc ] || [ $(basename $CXX) == g++ ]; then
    ARCH_MACRO=-DX86_64:BOOL=true
  elif [ "$(echo ${CFLAGS} | grep "armv7-a")" ]; then
    ARCH_MACRO=-DARMEABI_V7A:BOOL=true
  elif [ "$(echo ${CFLAGS} | grep "armv8-a")" ]; then
    ARCH_MACRO=-DARM64_V8A:BOOL=true
  fi
  echo "gcc path:$CC exist.."
  echo "gcc path:$CXX exist.."
  cmake \
    ../../proj.cmake \
    -DCMAKE_C_COMPILER=$CC \
    -DCMAKE_CXX_COMPILER=$CXX \
    -DCMAKE_CXX_FLAGS="-std=c++11 -O2 -fno-exceptions -fno-rtti -fPIE \
                      -fno-bounds-check -funroll-loops -funswitch-loops \
                      -fvisibility=hidden -finline-functions -funsafe-loop-optimizations -march=native -fopenmp -ffast-math" \
    ${ARCH_MACRO} -G "Unix Makefiles"
else
  echo "gcc path:$CC default.."
  echo "gcc path:$CXX default.."
  cmake \
    -DCMAKE_CXX_FLAGS="-std=c++11 -O2 -fno-exceptions -fno-rtti -fPIE \
                      -fno-bounds-check -funroll-loops -funswitch-loops \
                      -fvisibility=hidden -finline-functions -funsafe-loop-optimizations -march=native -fopenmp -ffast-math" \
    -DX86_64:BOOL=true -G "Unix Makefiles" ../../proj.cmake
fi
make -j4
cd ../..

rm -rf build/release/linux
mkdir -p build/release/linux
cp -f build/linux/lib/libJnd.a build/release/linux/libJnd.a || exit 1
cp -f src/libJnd/jnd_filter.h build/release/linux/
cp -f src/libJnd/jnd_filter_cl.h build/release/linux/

