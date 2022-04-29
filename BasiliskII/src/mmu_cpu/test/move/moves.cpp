#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVES, InitFix)
BOOST_DATA_TEST_CASE(opc, EA_M(), ea) {
    BOOST_TEST(opc_map[0007000 | ea] == opc_map[0007020]);
    BOOST_TEST(opc_map[0007100 | ea] == opc_map[0007120]);
    BOOST_TEST(opc_map[0007200 | ea] == opc_map[0007220]);
}
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0007021);
    raw_write16(2, 0x4000);
     exception_check(8);
}

BOOST_AUTO_TEST_CASE(from) {
    regs.S = true;
    regs.a[1] = 0x1000;
    raw_write16(0, 0007021);
    raw_write16(2, 0x5000);
    raw_write8(0x1000, 0x23);
    exception_check(0);
    BOOST_TEST( regs.d[5] == 0x23);
}

BOOST_AUTO_TEST_CASE(to) {
    regs.S = true;
    regs.a[1] = 0x1000;
    regs.d[5] = 0x23;
    raw_write16(0, 0007021);
    raw_write16(2, 0x5800);
    exception_check(0);
    BOOST_TEST( raw_read8(0x1000) == 0x23);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0007121);
    raw_write16(2, 0x4000);
     exception_check(8);
}

BOOST_AUTO_TEST_CASE(from) {
    regs.S = true;
    regs.a[1] = 0x1000;
    raw_write16(0, 0007121);
    raw_write16(2, 0x5000);
    raw_write16(0x1000, 0x2345);
    exception_check(0);
    BOOST_TEST( regs.d[5] == 0x2345);
}

BOOST_AUTO_TEST_CASE(to) {
    regs.S = true;
    regs.a[1] = 0x1000;
    regs.d[5] = 0x2345;
    raw_write16(0, 0007121);
    raw_write16(2, 0x5800);
    exception_check(0);
    BOOST_TEST( raw_read16(0x1000) == 0x2345);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0007221);
    raw_write16(2, 0x4000);
     exception_check(8);
}

BOOST_AUTO_TEST_CASE(from) {
    regs.S = true;
    regs.a[1] = 0x1000;
    raw_write16(0, 0007221);
    raw_write16(2, 0x5000);
    raw_write32(0x1000, 0x23456789);
    exception_check(0);
    BOOST_TEST( regs.d[5] == 0x23456789);
}

BOOST_AUTO_TEST_CASE(to) {
    regs.S = true;
    regs.a[1] = 0x1000;
    regs.d[5] = 0x23456789;
    raw_write16(0, 0007221);
    raw_write16(2, 0x5800);
    exception_check(0);
    BOOST_TEST( raw_read32(0x1000) == 0x23456789);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()