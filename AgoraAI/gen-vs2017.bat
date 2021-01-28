@echo off

set arch=x64

set build_path=build\build_vs2017_agora_ai_%arch%
echo Build Target=%arch%

if not exist %build_path% (
  mkdir %build_path%
)
cd %build_path%
echo %build_path%

cmake ..\..\proj.cmake ^
    -DX86:BOOL=false ^
    -DX86_64:BOOL=true ^
    -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ^
    -G "Visual Studio 15 2017 Win64"
cd ..\..\

set build_path_x86=build\build_vs2017_agora_ai_x86
echo Build Target=%arch%

if not exist %build_path_x86% (
  mkdir %build_path_x86%
)
cd %build_path_x86%
echo %build_path_x86%

cmake ..\..\proj.cmake ^
    -DX86:BOOL=true ^
    -DX86_64:BOOL=false ^
    -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ^
    -T v141_xp -G "Visual Studio 15 2017"

cd ..\..\