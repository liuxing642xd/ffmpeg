echo "#######################################"
echo "#   build mac jnd static lib"
echo "#######################################"
proj_config="Release"

eval "sh gen-mac.sh" || exit 1

cd build/mac
xcodebuild ARCHS="x86_64" ONLY_ACTIVE_ARCH=NO -UseModernBuildSystem=NO -project agora_ai.xcodeproj -configuration Release -sdk macosx clean build &&
rm -rf ../release/mac &&
mkdir -p ../release/mac &&
cp -r lib/${proj_config}/libJnd.a ../release/mac || exit 1
cd ../..

xcrun -sdk macosx metal -c src/libJnd/ios/jnd_kernels.metal -o build/mac/jnd_kernels.air
xcrun -sdk macosx metallib build/mac/jnd_kernels.air -o build/mac/pvc_kernels.metallib
 
 cp -r build/mac/pvc_kernels.metallib build/release/mac/pvc_kernels.metallib || exit 1
  
 cp -f src/libJnd/ios/jnd_filter_metal.h build/release/mac/
