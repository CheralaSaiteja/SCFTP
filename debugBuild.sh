#!/bin/sh
echo "Building ConfigTool"
cd ConfigurationTool
./debugBuild.sh

echo "Building Server"
cd ../Server/
./debugBuild.sh

