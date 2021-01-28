call VsDevCmd.bat
cd build\build_vs2017_agora_ai_x86
call msbuild agora_ai.sln /t:Rebuild /p:Configuration=Release /p:SubsystemVersion=5.1
cd ..\..\