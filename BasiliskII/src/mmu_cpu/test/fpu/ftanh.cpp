#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "fpu/fpu_mpfr.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FTANH, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x09, 1.0, tanh(1.0));
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x09, 0.1, tanh(0.1));
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x09, z, z);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x09, copysign(INFINITY, sg), copysign(1.0, sg));
}


BOOST_AUTO_TEST_SUITE_END()
