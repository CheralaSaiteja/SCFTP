#!/bin/sh

echo "Building Project"
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/
cd build/
make

echo "Moving Executable"
mv SCFTPServer ../../bin/debug/