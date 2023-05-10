#!/bin/sh

echo "Building project"
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/
cd build/
make

echo "Moving executable to root"
mv Cipher ../
cd ..
