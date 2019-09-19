#!/usr/bin/env bash

# Assuming you're in a python virtual environment
pushd python
pip install -e .
popd

mkdir build
pushd build
cmake ..
make -j
popd

ln -s $(pwd)/build/compile_commands.json .
