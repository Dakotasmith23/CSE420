#!/bin/bash

./clean_os.sh
rm *.txt
rm *~
make clean
make

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sed 's,replace,'"$DIR"',' inputFile1.dat > inputFile1.txt
sed 's,replace,'"$DIR"',' inputFile2.dat > inputFile2.txt

echo
echo Testing will take around 10 seconds...
./run1
sort output.txt > student_out1.txt

if diff -w student_out1.txt correct_output/correct_output1.txt; then
    echo Success---------------------Test 1--------------------------Success
else
    echo Fail------------------------Test 1--------------------------Fail
fi

./clean_os.sh
rm output.txt

./run2
sort output.txt > student_out2.txt

if diff -w student_out2.txt correct_output/correct_output2.txt; then
    echo Success---------------------Test 2--------------------------Success
else
    echo Fail------------------------Test 2--------------------------Fail
fi
