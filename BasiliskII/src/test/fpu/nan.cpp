#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(nan_, InitFix)
BOOST_AUTO_TEST_SUITE(sop)
BOOST_AUTO_TEST_CASE(qnan) {
    fpu.fp[1].set_nan(0x7F00000000000000LLU);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x18);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[2].is_nan());
    BOOST_TEST( fpu.fp[2].get_payload() == 0x7F00000000000000LLU);
}
BOOST_AUTO_TEST_CASE(snan) {
    fpu.FPCR.snan = false;
    fpu.fp[1].set_nan(0x3F00000000000000LLU);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x18);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[2].is_nan());
    BOOST_TEST( fpu.fp[2].get_payload() == 0x7F00000000000000LLU);
    BOOST_TEST( fpu.FPSR.snan );
}

BOOST_AUTO_TEST_CASE(snan_sig) {
    fpu.FPCR.snan = true;
    fpu.fp[1].set_nan(0x3F00000000000000LLU);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x18);
    exception_check(54);
    BOOST_TEST( fpu.fp[2].is_nan());
    BOOST_TEST( fpu.fp[2].get_payload() == 0x3F00000000000000LLU);
    BOOST_TEST( fpu.FPSR.snan );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(dop)
BOOST_AUTO_TEST_CASE(src) {
    fpu.fp[1].set_nan(0x7F00000000000000LLU);
    fpu.fp[2] = 2.0;
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x22);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[2].is_nan());
    BOOST_TEST( fpu.fp[2].get_payload() == 0x7F00000000000000LLU);
}

BOOST_AUTO_TEST_CASE(dst) {
    fpu.fp[2].set_nan(0x7F00000000000000LLU);
    fpu.fp[1] = 2.0;
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x22);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[2].is_nan());
    BOOST_TEST( fpu.fp[2].get_payload() == 0x7F00000000000000LLU);
}

BOOST_AUTO_TEST_CASE(both) {
    fpu.fp[1].set_nan(0x7F00000000000000LLU);
    fpu.fp[2].set_nan(0x7FF0000000000000LLU);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x22);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[2].is_nan());
    BOOST_TEST( fpu.fp[2].get_payload() == 0x7FF0000000000000LLU);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
