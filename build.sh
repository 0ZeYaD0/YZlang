#!/bin/bash

outputAssembly="yzlang"
compiler="g++"

echo "------------------------------"
echo "C++ build script for $outputAssembly"
echo "by Yassin Ibrahim/Zeyad Emad (DeltaY)(0ZeYaD0)"
echo "------------------------------"
echo

if [ ! -d "./bin/" ]; then
    mkdir bin
fi

if [ ! -d "./obj/" ]; then
    mkdir obj
fi

# Add include directories
includeDirs="-I./src -I./lib/tomlplusplus/include"

# List of source files
cppFiles=$(find src -name "*.cpp")

# Compiler flags
compilerFlags="-std=c++17"

# Defines (if any)
defines=""

echo "compiling..."

# Compile and link
$compiler $cppFiles $compilerFlags $includeDirs $defines -o bin/$outputAssembly

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "compilation/linking successful."
else
    echo "compilation/linking failed."
    exit 1
fi

exit 0