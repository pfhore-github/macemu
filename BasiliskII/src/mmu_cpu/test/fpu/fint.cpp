#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "fpu/fpu_mpfr.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FINT, InitFix)
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -0.0 : 0.0;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -INFINITY : INFINITY;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(isinf(regs.fp[3]) );
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]) );
}

BOOST_AUTO_TEST_SUITE(TO_NEAREST)
BOOST_AUTO_TEST_CASE(positive_floor1) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = 2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_floor2) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = 2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling1) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = 3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 4.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling2) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = 2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(negative_floor1) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = -2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor2) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = -2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling1) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = -3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -4.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling2) {
    regs.FPCR.rnd = ROUND_RN;
    regs.fp[2] = -2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TO_ZERO)
BOOST_AUTO_TEST_CASE(positive_floor1) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = 2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_floor2) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = 2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling1) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = 3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling2) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = 2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor1) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = -2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor2) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = -2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling1) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = -3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling2) {
    regs.FPCR.rnd = ROUND_RZ;
    regs.fp[2] = -2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DOWNWARD)
BOOST_AUTO_TEST_CASE(positive_floor1) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = 2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_floor2) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = 2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling1) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = 3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling2) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = 2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor1) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = -2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}

BOOST_AUTO_TEST_CASE(negative_floor2) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = -2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling1) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = -3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -4.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling2) {
    regs.FPCR.rnd = ROUND_RM;
    regs.fp[2] = -2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UPWARD)
BOOST_AUTO_TEST_CASE(positive_floor1) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = 2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(positive_floor2) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = 2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling1) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = 3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 4.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling2) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = 2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(negative_floor1) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = -2.3;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor2) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = -2.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling1) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = -3.5;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling2) {
    regs.FPCR.rnd = ROUND_RP;
    regs.fp[2] = -2.7;
    asm_m68k("fint.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()