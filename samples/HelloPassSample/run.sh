#!/bin/bash

echo $1

INTERMEDIATE_DIRECTORY=$1
PLUGIN=$2

mkdir -p "$INTERMEDIATE_DIRECTORY"

clang -O1 -emit-llvm -c main.cpp -o "$INTERMEDIATE_DIRECTORY/main.bc"
opt "$INTERMEDIATE_DIRECTORY/main.bc" -o /dev/null -load-pass-plugin "$PLUGIN" -passes=HelloPass
