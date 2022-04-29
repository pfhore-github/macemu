#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FSUB, InitFix)
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = 1.0;
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -1.0);
}

BOOST_DATA_TEST_CASE(infinity, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = INFINITY;
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = 0.0;
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[2] = 1.0;
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = 0.0;
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, -sg);
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(nan1) {
    regs.fp[2] = 1.0;
    regs.fp[3] = NAN;
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan2) {
    regs.fp[3] = 1.0;
    regs.fp[2] = NAN;
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(operr, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, sg);
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[3] = 10.0;
    regs.fp[2] = 2.0;
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 8.0);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    regs.fp[2] = -std::numeric_limits<double>::max();
    regs.fp[3] = std::numeric_limits<double>::max();
    asm_m68k("fsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FSSUB, InitFix)
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[3] = copysignf(0.0f, sg);
    regs.fp[2] = 1.0f;
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -1.0f);
}

BOOST_DATA_TEST_CASE(infinity, SIGN, sg) {
    regs.fp[3] = copysignf(0.0f, sg);
    regs.fp[2] = INFINITY;
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[3] = copysign(0.0f, sg);
    regs.fp[2] = 0.0f;
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.0f);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[2] = 1.0f;
    regs.fp[3] = copysignf(INFINITY, sg);
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = 0.0f;
    regs.fp[3] = copysignf(INFINITY, sg);
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysignf(INFINITY, -sg);
    regs.fp[3] = copysignf(INFINITY, sg);
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(nan1) {
    regs.fp[2] = 1.0f;
    regs.fp[3] = NAN;
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan2) {
    regs.fp[3] = 1.0f;
    regs.fp[2] = NAN;
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(operr, SIGN, sg) {
    regs.fp[2] = copysignf(INFINITY, sg);
    regs.fp[3] = copysignf(INFINITY, sg);
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[3] = 10.0f;
    regs.fp[2] = 2.0f;
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 8.0f);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    regs.fp[2] = -std::numeric_limits<float>::max();
    regs.fp[3] = std::numeric_limits<float>::max();
    asm_m68k("fssub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FDSUB, InitFix)
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = 1.0;
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -1.0);
}

BOOST_DATA_TEST_CASE(infinity, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = INFINITY;
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = 0.0;
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[2] = 1.0;
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = 0.0;
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, -sg);
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(nan1) {
    regs.fp[2] = 1.0;
    regs.fp[3] = NAN;
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan2) {
    regs.fp[3] = 1.0;
    regs.fp[2] = NAN;
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(operr, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, sg);
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[3] = 10.0;
    regs.fp[2] = 2.0;
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 8.0);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    regs.fp[2] = -std::numeric_limits<double>::max();
    regs.fp[3] = std::numeric_limits<double>::max();
    asm_m68k("fdsub.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
