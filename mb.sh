#!/bin/bash
echo "Removing old build directory"
rm -r build >/dev/null
echo "Creating new build directory"
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
make install
