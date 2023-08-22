#! /bin/bash

rm -rf build
mkdir build
cd build

args=""
if [ $1 == "debug" ] || [ $2 == "debug" ]; then
    args+="-DCMAKE_BUILD_TYPE=Debug "
fi


if [ $1 == "12" ] || [ $2 == "12" ]; then
    args+="-DCMAKE_CXX_COMPILER=clang-12"
else
    args+="-DCMAKE_CXX_COMPILER=clang-14"
fi

cmake $args ..



