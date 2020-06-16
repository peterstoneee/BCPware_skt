
::Build common.lib
START /WAIT "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\common\common.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild

::Build Bcpware.exe, edit_test.dll, filter_box_packing.dll, io_base.dll, io_x3d.dll
START  "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\bcpwarePlugins\edit_test\edit_test.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild /m

START  "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\bcpwarePlugins\filter_box_packing\filter_box_packing.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild /m

START  "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\bcpwarePlugins\io_base\io_base.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild /m:2

START  "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\bcpwarePlugins\io_x3d\io_x3d.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild /m

START  "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\bcpware\picasso.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild /m

START /WAIT "" "C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe" ..\bcpwarePlugins\generate_print_func\generate_print_func.vcxproj /p:Configuration=Release,PlatformToolset=v120,Platform=x64 -t:rebuild /m

::wait the time of the longest project end, and do the next step;

copy  ..\distrib\BCPware.exe C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\BCPware.exe 
copy  ..\distrib\plugins\edit_test.dll C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\plugins\edit_test.dll 
copy  ..\distrib\plugins\io_x3d.dll C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\plugins\io_x3d.dll
copy  ..\distrib\plugins\io_base.dll C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\plugins\io_base.dll
copy ..\distrib\plugins\filter_box_packing.dll C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\plugins\filter_box_packing.dll
copy ..\distrib\plugins\generate_print_func_re.dll C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\plugins\generate_print_func_re.dll
copy  C:\Users\TB495076\Documents\BCPware\parameter_setting.zxb C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\recordDoc\parameter_setting.zxb
copy  C:\Users\TB495076\Documents\BCPware\parameter_setting.zxb C:\Users\TB495076\Desktop\ReleasePlace\3DP_rock_setup\windows\Release4\parameter_setting.zxb