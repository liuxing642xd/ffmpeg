echo "#######################################"
echo "#   build jnd static lib"
echo "#######################################"
proj_config="Release"

eval "sh gen-ios.sh" || exit 1

cd build/ios/
xcodebuild ARCHS="arm64 armv7" BITCODE_GENERATION_MODE=bitcode OTHER_CFLAGS="-fembed-bitcode" ONLY_ACTIVE_ARCH=NO -UseModernBuildSystem=NO -project agora_ai.xcodeproj -configuration ${proj_config} -sdk iphoneos clean build &&
rm -rf ../release/ios/${proj_config}-os
mkdir -p ../release/ios/${proj_config}-os || exit 1
cp -r lib/${proj_config} ../release/ios/${proj_config}-os || exit 1
cd ../..

eval "sh gen-ios.sh sim" || exit 1
cd build/ios/
xcodebuild ARCHS="i386 x86_64" BITCODE_GENERATION_MODE=bitcode OTHER_CFLAGS="-fembed-bitcode" ONLY_ACTIVE_ARCH=NO -UseModernBuildSystem=NO -project agora_ai.xcodeproj -configuration ${proj_config} -sdk iphonesimulator clean build &&
rm -rf ../release/ios/${proj_config}-simulator
mkdir -p ../release/ios/${proj_config}-simulator || exit 1
cp -r lib/${proj_config} ../release/ios/${proj_config}-simulator || exit 1
cd ../..

lipo -create build/release/ios/${proj_config}-simulator/Release/libJnd.a build/release/ios/${proj_config}-os/Release/libJnd.a -output build/release/ios/libJnd.a || exit 1

xcrun -sdk iphoneos metal -c src/libJnd/ios/jnd_kernels.metal -o build/ios/jnd_kernels.air
xcrun -sdk iphoneos metallib build/ios/jnd_kernels.air -o build/ios/pvc_kernels.metallib

cp -r build/ios/pvc_kernels.metallib build/release/ios/pvc_kernels.metallib || exit 1
 
cp -f src/libJnd/ios/jnd_filter_metal.h build/release/ios/
rm -rf build/release/ios/Release-os
rm -rf build/release/ios/Release-simulator
