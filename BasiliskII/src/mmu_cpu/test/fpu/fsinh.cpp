#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FSINH, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x02, in, 0.0, sinh(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double d = copysign(0.0, sg);
    fpu_test<double>(0x02, d, 0.0, d);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double d = copysign(INFINITY, sg);
    fpu_test<double>(0x02, d, 0.0, d);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x02, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xv{1, 200};
    fpu_test(0x02, xv, xv, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
