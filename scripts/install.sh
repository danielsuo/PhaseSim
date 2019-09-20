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

# Install spec
git clone https://github.com/danielsuo/spec

pushd spec
git lfs pull
./install.sh
ln -s $(pwd)/../ref/spec/gcc-mixed_asm_C.cfg config/default.cfg
./bin/runcpu -a build all
popd

