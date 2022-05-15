#define BOOST_TEST_DYN_LINK
#include "exception.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
void m68k_do_execute();

BOOST_FIXTURE_TEST_SUITE(exception, InitFix)
int szz[] = {3, 1, 0, 2};
BOOST_DATA_TEST_CASE(ACCESS_FAULT_R, bdata::xrange(1, 4) * BIT, sz, s) {
    regs.S = s;
    regs.a[2] = 0x60000000;
    raw_write16(0, 0001022 | sz << 12);
    exception_check(2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) ==
               (1 << 8 | szz[sz] << 5 | s << 2 | 1));
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}

BOOST_DATA_TEST_CASE(ACCESS_FAULT_W, bdata::xrange(1, 4) * BIT, sz, s) {
    regs.S = s;
    regs.a[2] = 0x60000000;
    raw_write16(0, 0002201 | sz << 12);
    exception_check(2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) == (szz[sz] << 5 | s << 2 | 1));
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}

BOOST_AUTO_TEST_CASE(ACCESS_FAULT_MOVEM) {
    regs.a[2] = 0x60000000;
    raw_write16(0, 0044222);
    raw_write16(2, 0x0E0E);
    exception_check(2);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 12);
} 

BOOST_AUTO_TEST_CASE(ADDRESS_ERROR) {
    regs.pc = 0x1001;
    exception_check(3);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0x1001);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0x1000);
}

BOOST_AUTO_TEST_CASE(ILLEGAL) {
    raw_write16(0, 0045374);
    exception_check(4);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
}

BOOST_AUTO_TEST_CASE(Div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2002);
    exception_check(5);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 4);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
}
BOOST_AUTO_TEST_SUITE_END()