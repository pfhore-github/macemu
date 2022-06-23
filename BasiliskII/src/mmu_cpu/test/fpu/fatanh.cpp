#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FATANH, InitFix)
BOOST_AUTO_TEST_CASE(operand, *boost::unit_test::tolerance(1e-12)) {
    fpu_test2(0x0D, 0.1, atanh(0.1));
}

BOOST_AUTO_TEST_CASE(normal, *boost::unit_test::tolerance(1e-12)) {
    fpu_test(0x0D, 0.3, atanh(0.3));
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double in = copysign(0.0, sg);
    fpu_test(0x0D, in, in);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x0D, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(dz, SIGN, sg) {
    fpu_test<double>(0x0D, copysign(1.0, sg), copysign(INFINITY, sg));
    BOOST_TEST(fpu.FPSR.dz);
}

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test<double>(0x0D, 2.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
