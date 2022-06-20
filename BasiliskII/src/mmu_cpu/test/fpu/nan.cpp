#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(nan_, InitFix)
BOOST_AUTO_TEST_SUITE(single)
BOOST_AUTO_TEST_CASE(qnan) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 0x7FC12345);

    raw_write16(0, 0171021);
    raw_write16(2, 1 << 14 | 1 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[1].get_payload() == 0x4123450000000000LLU);
}

BOOST_AUTO_TEST_CASE(snan) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 0x7F812345);

    raw_write16(0, 0171021);
    raw_write16(2, 1 << 14 | 1 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST( fpu.fp[1].get_payload() == 0x4123450000000000LLU);
    BOOST_TEST( fpu.FPSR.snan );
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
