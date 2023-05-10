#!/bin/sh
echo "Building ConfigTool"
cd ConfigurationTool
./build.sh

echo "Building Server"
cd ../Server/
./build.sh
