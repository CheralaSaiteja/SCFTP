#!/bin/sh

echo "Building project"
cmake -S . -B build/
cd build/
make

echo "Moving executable to root"
mv ConfigTool ../
cd ..
