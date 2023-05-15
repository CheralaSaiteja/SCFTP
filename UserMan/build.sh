#!/bin/sh

echo "Building project"
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=off -S . -B build/
cd build/
make

echo "Moving executable"
mkdir -p ../../bin/release/
mv UserMan ../../bin/release/
cd ..
