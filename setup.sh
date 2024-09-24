#!/bin/sh
rm -rf build
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D_CMAKE_TOOLCHAIN_PREFIX=llvm- -B build
