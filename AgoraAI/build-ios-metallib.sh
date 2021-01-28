xcrun -sdk iphoneos metal -c src/libJnd/ios/jnd_kernels.metal -o build/ios/jnd_kernels.air
xcrun -sdk iphoneos metallib build/ios/jnd_kernels.air -o build/ios/pvc_kernels.metallib