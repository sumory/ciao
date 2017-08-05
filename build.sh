#!/bin/bash

SHELL=/bin/bash

RED='\033[0;31m' # Red
BB='\033[0;34m'  # Blue
NC='\033[0m'     # No Color
BG='\033[0;32m'  # Green

line() { echo ""; }
error() { >&2 echo -e "${RED}$1${NC}"; }
showinfo() { echo -e "${BG}$1${NC}"; }
tip() { echo -e "${BB}$1${NC}"; }

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8

if [ $? -ne 0 ]; then
    error "Error: there are compile errors!"
    exit 3
fi

showinfo "Running tests ..."
./bin/ciao_unittest
if [ $? -ne 0 ]; then
    error "Error: there are failed tests!"
    exit 4
fi

line ""
tip "[Result]:"
showinfo "All tests compile and pass."