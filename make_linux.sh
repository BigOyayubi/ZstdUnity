#!/bin/sh

mkdir -p build_linux && cd build_linux
cmake ../
cd ..
cmake --build build_linux --config Release
mkdir -p plugin/Plugins/Linux/
cp build_linux/libzstd.so plugin/Plugins/Linux/
mkdir -p Assets/Plugins/Linux/
cp build_linux/libzstd.so Assets/Plugins/Linux/

