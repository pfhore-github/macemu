#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "fpu/fpu_ieee.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FATAN, InitFix)
BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), sg) {
    regs.fp[2] = sg ? -0.0 : 0.0;
    asm_m68k("fatan.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(regs.fp[3] == 0.0);
}
BOOST_TEST_DECORATOR(* boost::unit_test::tolerance(0.000001))
BOOST_DATA_TEST_CASE(inf,  boost::unit_test::data::xrange(2), sg) {
    regs.fp[2] = sg ? -INFINITY : INFINITY;
    asm_m68k("fatan.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(fabs(regs.fp[3]) == 1.5707963267949 );
}

BOOST_DATA_TEST_CASE(nan_, boost::unit_test::data::xrange(2), sg) {
    regs.fp[2] = sg ? -NAN : NAN;
    asm_m68k("fatan.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(isnan(regs.fp[3]) );
}

BOOST_AUTO_TEST_CASE(value, * boost::unit_test::tolerance(0.000001)) {
    regs.fp[2] = 0.5;
    asm_m68k("fatan.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.463647609000806);
}

BOOST_AUTO_TEST_SUITE_END()
