#!/bin/bash
set -e

echo "Building EcoRecycle Server..."
mkdir -p build
cd build
cmake ..
make

echo "Starting Server..."
./server
