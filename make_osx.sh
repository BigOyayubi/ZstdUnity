#!/bin/sh

mkdir -p build_osx && cd build_osx
cmake -GXcode ../
cd ..
cmake --build build_osx --config Release
mkdir -p plugin/Plugins/OSX/zstd.bundle/Contents/MacOS/
cp build_osx/Release/zstd.bundle/Contents/MacOS/zstd plugin/Plugins/OSX/zstd.bundle/Contents/MacOS/zstd
mkdir -p Assets/Plugins/OSX/zstd.bundle/Contents/MacOS/
cp build_osx/Release/zstd.bundle/Contents/MacOS/zstd Assets/Plugins/OSX/zstd.bundle/Contents/MacOS/zstd
