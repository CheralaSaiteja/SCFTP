#!/bin/sh
echo "Building project"
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/
cd build/
make

echo "Moving Executable"
mkdir -p ../../bin/debug/
mv UserMan ../../bin/debug/