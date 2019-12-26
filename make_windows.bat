mkdir build_windows & pushd build_windows
cmake -G "Visual Studio 15 2017 Win64" ..
popd
cmake --build build_windows --config Release
md plugin\Plugins\Win\x86_64
copy /Y build_windows\Release\zstd.dll plugin\Plugins\Win\x86_64\zstd.dll
md Assets\Plugins\Win\x86_64
copy /Y build_windows\Release\zstd.dll Assets\Plugins\Win\x86_64\zstd.dll
pause

