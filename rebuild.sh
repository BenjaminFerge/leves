#!/bin/bash -xe
rm -rf build
rm -rf inc_duktape
mkdir inc_duktape
python duktape/tools/configure.py --output-directory=inc_duktape -DDUK_USE_DEBUGGER_SUPPORT -DDUK_USE_INTERRUPT_COUNTER -DDUK_USE_DEBUGGER_DUMPHEAP
mkdir build
pushd build
conan install ..
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args" ..
cmake --build .
popd
ln -sf build/compile_commands.json .
