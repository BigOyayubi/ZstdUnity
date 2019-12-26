#!/bin/sh

mkdir -p build_ios && cd build_ios
# from https://github.com/leetal/ios-cmake. thanks
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=OS64 -GXcode ../
cd ..
cmake --build build_ios --config Release
mkdir -p plugin/Plugins/iOS/
cp build_ios/Release-iphoneos/libzstd.a plugin/Plugins/iOS/libzstd.a
mkdir -p Assets/Plugins/iOS/
cp build_ios/Release-iphoneos/libzstd.a Assets/Plugins/iOS/libzstd.a

