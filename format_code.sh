#!/bin/bash

SUBDIRS="ciao ciao/* ciao/*/* ciao/*/*/* test test/*"
FILETYPES="*.hpp *.h *.cpp *.c *.cc"
CLANG_FORMAT="clang-format"

echo "start format..."

for d in ${SUBDIRS}
do
    echo "clang-format format subdir: $d "
    for t in ${FILETYPES}
    do
        for file in $d/$t
        do
            echo ">>>>>>>>>> format file: $file "
            if test -f $file
            then
               ${CLANG_FORMAT} -i $file
            fi
        done
    done
done

echo "format end."

