#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FSQRT, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysign(0.0, sg);
    asm_m68k("fsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_AUTO_TEST_CASE(inf) {
    regs.fp[2] = INFINITY;
    asm_m68k("fsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[2] = 9.0;
    asm_m68k("fsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(operr) {
    regs.fp[2] = -9.0;
    asm_m68k("fsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FSSQRT, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysignf(0.0f, sg);
    asm_m68k("fssqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_AUTO_TEST_CASE(inf) {
    regs.fp[2] = INFINITY;
    asm_m68k("fssqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fssqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.0001)) {
    regs.fp[2] = 3.0f;
    asm_m68k("fssqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1.7320508f);
}

BOOST_AUTO_TEST_CASE(operr) {
    regs.fp[2] = -9.0f;
    asm_m68k("fssqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FDSQRT, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysignf(0.0f, sg);
    asm_m68k("fdsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_AUTO_TEST_CASE(inf) {
    regs.fp[2] = INFINITY;
    asm_m68k("fdsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fdsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.00001)) {
    regs.fp[2] = 3.0;
    asm_m68k("fdsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1.73205080756888);
}

BOOST_AUTO_TEST_CASE(operr) {
    regs.fp[2] = -9.0;
    asm_m68k("fdsqrt.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
