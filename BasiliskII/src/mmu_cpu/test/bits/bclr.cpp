#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(BCLR, InitFix)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_DATA_TEST_CASE(Byte, bdata::xrange(8), pos) {
    regs.a[1] = 0x10;
    raw_write16(0, 0004221);
    raw_write16(2, pos);
    raw_write8(0x10, 0xff);
    m68k_do_execute();
    BOOST_TEST(!(raw_read8(0x10) & 1 << pos));
}

BOOST_DATA_TEST_CASE(Long, bdata::xrange(32), pos) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0004201);
    raw_write16(2, pos);
    m68k_do_execute();
    BOOST_TEST(!(regs.d[1] & 1 << pos));
}

BOOST_DATA_TEST_CASE(opc_d, REG(), ea) {
    BOOST_TEST(opc_map[0004200 | ea] == opc_map[0004200]);
}

BOOST_DATA_TEST_CASE(opc_m, EA_M(), ea) {
    BOOST_TEST(opc_map[0004200 | ea] == opc_map[0004220]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(Byte, bdata::xrange(8), pos) {
    regs.d[2] = pos;
    regs.a[1] = 0x10;
    raw_write16(0, 0002621);
    raw_write8(0x10, 0xff);
    m68k_do_execute();
    BOOST_TEST(!(raw_read8(0x10) >> pos & 1));
}

BOOST_DATA_TEST_CASE(Long, bdata::xrange(32),pos) {
    regs.d[2] = pos;
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0002601);
    m68k_do_execute();
    BOOST_TEST(!(regs.d[1] >> pos & 1));
}

BOOST_DATA_TEST_CASE(opc_d, REG() * REG(), dn, ea) {
    BOOST_TEST(opc_map[0000600 | dn << 9| ea] == opc_map[0000600]);
}

BOOST_DATA_TEST_CASE(opc_m, REG() * EA_M(), dn, ea) {
    BOOST_TEST(opc_map[0000600 | dn << 9 | ea] == opc_map[0000620]);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
