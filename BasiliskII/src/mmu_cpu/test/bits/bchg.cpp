#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(BCHG, InitFix)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_DATA_TEST_CASE(Byte, BIT *bdata::xrange(8), value, pos) {
    regs.a[1] = 0x10;
    raw_write8(0x10, value << pos);
    raw_write16(0, 0004121);
    raw_write16(2, pos);
    m68k_do_execute();
    BOOST_TEST((raw_read8(0x10) >> pos & 1) != value);
}

BOOST_DATA_TEST_CASE(Long, BIT *bdata::xrange(32), value,
                     pos) {
    regs.d[1] = value << pos;
    raw_write16(0, 0004101);
    raw_write16(2, pos);
    m68k_do_execute();
    BOOST_TEST((regs.d[1] >> pos & 1) != value);
}

BOOST_DATA_TEST_CASE(opc_d, REG(), ea) {
    BOOST_TEST(opc_map[0004100 | ea] == opc_map[0004100]);
}

BOOST_DATA_TEST_CASE(opc_m, EA_M(), ea) {
    BOOST_TEST(opc_map[0004100 | ea] == opc_map[0004120]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(Byte, BIT*bdata::xrange(8), value, pos) {
    regs.d[2] = pos;
    regs.a[1] = 0x10;
    raw_write16(0, 0002521);
    raw_write8(0x10, value << pos);
    m68k_do_execute();
    BOOST_TEST((raw_read8(0x10) >> pos & 1) != value);
}

BOOST_DATA_TEST_CASE(Long, BIT*bdata::xrange(32), value, pos) {
    regs.d[2] = pos;
    regs.d[1] = value << pos;
    raw_write16(0, 0002501);
    m68k_do_execute();
    BOOST_TEST((regs.d[1] >> pos & 1) != value);
}
BOOST_DATA_TEST_CASE(opc_d, REG() * REG(), dn, ea) {
    BOOST_TEST(opc_map[0000500 | dn << 9| ea] == opc_map[0000500]);
}

BOOST_DATA_TEST_CASE(opc_m, REG() * EA_M(), dn, ea) {
    BOOST_TEST(opc_map[0000500 | dn << 9 | ea] == opc_map[0000520]);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
