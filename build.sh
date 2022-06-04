#!/bin/sh

rm -rf ./build
mkdir ./build
cd build

export PICO_SDK_PATH=../../pico-sdk

cmake ..

cd dotstar_strip
make -j8
