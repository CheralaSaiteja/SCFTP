#!/bin/sh

echo "Building Project"
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/

cd build/
echo "Cmpiling Project"
make

echo "Moving Executable to Root"
mv SCFTPClient ../
cd ..
