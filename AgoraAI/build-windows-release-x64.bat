call VsDevCmd.bat
cd build\build_vs2017_agora_ai_x64
call msbuild agora_ai.sln /t:Rebuild /p:Platform=x64 /p:Configuration=Release
cd ..\..\