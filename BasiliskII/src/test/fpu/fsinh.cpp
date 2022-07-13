#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FSINH, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x02, 0.1, sinh(0.1));
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x02, 0.1, sinh(0.1));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double d = copysign(0.0, sg);
    fpu_test<double>(0x02, d, d);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double d = copysign(INFINITY, sg);
    fpu_test<double>(0x02, d, d);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xv{false, 1, 200};
    fpu_test(0x02, xv, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
