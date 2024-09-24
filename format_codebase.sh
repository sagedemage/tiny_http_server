#!/bin/sh
cpp_files=$(find src/ -name "*.c")
hpp_files=$(find src/ -name "*.h")
clang-format --style=file -i $cpp_files $hpp_files
