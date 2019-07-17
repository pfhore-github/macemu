#!/bin/sh
rm obj/*.gcda 
make
./test
lcov -c -d obj -o test.info 
genhtml -o cov test.info 
