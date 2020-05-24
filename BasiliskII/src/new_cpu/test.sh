#!/bin/sh
rm obj/*.gcda 
make test
./test_cpu
./test_fpu
lcov -c -d obj -o test.info 
genhtml -o cov test.info 
