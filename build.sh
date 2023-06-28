#!/bin/sh


echo "Building Project"

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=off -S . -B build/
cd build/
make

echo "Moving Executables"

mkdir -p ../bin/Debug ../bin/Release
mv ConfigTool UserMan Server Client ../bin/Release/
