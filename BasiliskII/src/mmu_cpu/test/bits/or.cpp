#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(OR, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[01000 | dn << 3] == opc_map[01000]);
    BOOST_TEST(opc_map[01001 | dn << 3] == opc_map[01001]);
    BOOST_TEST(opc_map[01002 | dn << 3] == opc_map[01002]);
    BOOST_TEST(opc_map[01004 | dn << 3] == opc_map[01004]);
    BOOST_TEST(opc_map[01005 | dn << 3] == opc_map[01005]);
    BOOST_TEST(opc_map[01006 | dn << 3] == opc_map[01006]);
}

BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0x78;
    regs.d[2] = 0x2a;
    raw_write16(0, 0101002);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x78 | 0x2a));
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.d[1] = 0x78;
    regs.a[1] = 0x10;
    raw_write16(0, 0101421);
    raw_write8(0x10, 0x2a);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (0x78 | 0x2a));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0x5678;
    regs.d[2] = 0xdead;
    raw_write16(0, 0101102);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 | 0xdead));
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.d[1] = 0x5678;
    regs.a[1] = 0x10;
    raw_write16(0, 0101521);
    raw_write16(0x10, 0x2a2b);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (0x5678 | 0x2a2b));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 0xdeadbeaf;
    raw_write16(0, 0101202);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdeadbeaf));
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write32(0x10, 0x2a2b2c2d);
    raw_write16(0, 0101621);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (0x12345678 | 0x2a2b2c2d));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()