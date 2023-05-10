#!/bin/sh

echo "Building project"
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=off -S . -B build/
cd build/
make

echo "Moving executable"
mv ConfigTool ../../bin/release/
cd ..
