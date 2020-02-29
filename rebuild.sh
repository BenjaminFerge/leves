#!/bin/bash -xe
rm -rf build
mkdir build
pushd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args" ..
cmake --build .
popd
ln -sf build/compile_commands.json .
