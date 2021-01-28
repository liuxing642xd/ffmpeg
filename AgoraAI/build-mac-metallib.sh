xcrun -sdk macosx metal -c src/libJnd/ios/jnd_kernels.metal -o jnd_kernels.air
xcrun -sdk macosx metallib jnd_kernels.air -o pvc_kernels.metallib
