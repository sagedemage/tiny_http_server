#!/bin/sh
cpp_files=$(find ../src/ -name "*.c")
hpp_files=$(find ../src/ -name "*.h")
clang-tidy --format-style=file $cpp_files $hpp_files
