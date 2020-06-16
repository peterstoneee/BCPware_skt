@echo off
for /f "tokens=2* delims= " %%x in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\12.0" /v "MSBuildToolsPath"') do (
  set "msbuild_path=%%y"
  echo %%~y
)
echo %exe_path%
pause
::Build common.lib
if exist %exe_path% START /WAIT "" %msbuild_path% ..\common\common.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild

pause
