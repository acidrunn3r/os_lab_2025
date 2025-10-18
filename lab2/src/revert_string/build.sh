#!/bin/bash

echo "=== Building Static Library ==="
gcc -c revert_string.c -o revert_string.o
ar rcs librevert_string.a revert_string.o
gcc main.c -L. -lrevert_string -static -o program_static

echo -e "=== Building Dynamic Library ==="
gcc -c -fPIC revert_string.c -o revert_string_dynamic.o
gcc -shared -o librevert_string.so revert_string_dynamic.o
gcc main.c -L. -lrevert_string -Wl,-rpath,. -o program_dynamic

echo -e "\n=== Dependencies ==="
echo "Static:"
ldd program_static | grep revert

echo -e "Dynamic:"
ldd program_dynamic | grep revert