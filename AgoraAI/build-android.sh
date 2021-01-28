#!/bin/bash

arch=armeabi-v7a
ANDROID_NDK_ROOT=/Users/chenfeifei/Android/android-ndk-r16b

while [[  $# -gt 0 ]]
do
key="$1"
case $key in
--arch )
arch="$2"
shift #past argumemt
;;
*)
# unknown option
;;
esac
shift # past argument or value
done

if [[ $arch =~ armeabi-v7a ]]; then
abi="armeabi-v7a"
toolchain=arm-linux-androideabi-4.9
elif [[ $arch =~ arm64-v8a ]]; then
abi="arm64-v8a"
toolchain=aarch64-linux-android-4.9
elif [[ $arch =~ x86 ]]; then
abi="x86"
toolchain=x86-4.9
else
echo "arch must be armeabi-v7a, arm64-v8a, or x86"
exit 1
fi

build_path=build/android/$arch

rm -rf $build_path || exit 1
mkdir -p $build_path || exit 1
cd $build_path       || exit 1

echo Build Target=$abi

#/Users/chenfeifei/Library/Android/sdk/cmake/3.6.4111459/bin/cmake
cmake \
../../../proj.cmake \
-DANDROID_ABI=$abi \
-DANDROID_PLATFORM=android-16 \
-DANDROID_TOOLCHAIN_NAME=$toolchain \
-DANDROID_STL=c++_static \
-DCMAKE_CXX_FLAGS="-std=c++11 -O2 -fno-exceptions -fno-rtti -fPIE \
                  -fno-bounds-check -funroll-loops -funswitch-loops \
                  -fvisibility=hidden -finline-functions -funsafe-loop-optimizations" \
-DCMAKE_BUILD_TYPE=Debug \
-DANDROID_NDK=$ANDROID_NDK_ROOT \
-DANDROID_ARM_NEON=TRUE \
-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake \
-DANDROID_TOOLCHAIN=gcc || exit 1

make -j4 || exit 1
cd ../../.. || exit 1

exit 0
