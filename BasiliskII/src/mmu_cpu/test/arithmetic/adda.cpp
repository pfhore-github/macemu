#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_SUITE(ADDA, InitFix)

BOOST_AUTO_TEST_CASE(Word) {
    int an = rand_reg();
    int ea = rand_reg();
    regs.a[an] = 3456;
    regs.d[ea] = 0xffff;
    raw_write16(0, 0150300 | an << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 3455);
}

BOOST_AUTO_TEST_CASE(Long) {
    int an = rand_reg();
    int ea = rand_reg();
    regs.a[an] = 123456789;
    regs.d[ea] = 100000000;
    raw_write16(0, 0150700 | an << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 223456789);
}

BOOST_AUTO_TEST_SUITE_END()

