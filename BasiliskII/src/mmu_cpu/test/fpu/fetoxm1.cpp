#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FETOXM1, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x08, in, 0.0, exp(in) - 1.0);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x08, v, 0.0, v);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x08, INFINITY, 0.0, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) { fpu_test<double>(0x08, -INFINITY, 0.0, -1.0); }

BOOST_AUTO_TEST_CASE(nan_) {fpu_test<double>(0x08, NAN, 0.0, NAN); } 


BOOST_AUTO_TEST_CASE(ovfl) {
    xval v{1, 0x3ffe};
    fpu_test(0x08, v, v, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}


BOOST_AUTO_TEST_SUITE_END()