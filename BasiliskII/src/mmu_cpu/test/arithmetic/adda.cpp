#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_SUITE(ADDA, InitFix)

BOOST_AUTO_TEST_CASE(Word) {
    int xr = rand_reg();
    int yr = rand_reg();
    regs.a[xr] = 3456;
    regs.d[yr] = 0xffff;
    raw_write16(0, 0150300 | xr << 9 | yr);
    m68k_do_execute();
    BOOST_TEST(regs.a[xr] == 3455);
}

BOOST_AUTO_TEST_CASE(Long) {
    int xr = rand_reg();
    int yr = rand_reg();
    regs.a[xr] = 123456789;
    regs.d[yr] = 100000000;
    raw_write16(0, 0150700 | xr << 9 | yr);
    m68k_do_execute();
    BOOST_TEST(regs.a[xr] == 223456789);
}

BOOST_AUTO_TEST_SUITE_END()

