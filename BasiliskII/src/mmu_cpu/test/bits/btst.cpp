#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(BTST, InitFix)

BOOST_AUTO_TEST_SUITE(Imm)
BOOST_DATA_TEST_CASE(Byte, BIT *bdata::xrange(8), value, pos) {
    regs.a[1] = 0x10;
    raw_write16(0, 0004021);
    raw_write16(2, pos);
    raw_write8(0x10, value << pos);
    m68k_do_execute();
    BOOST_TEST(regs.z != value);
}

BOOST_DATA_TEST_CASE(Long, BIT *bdata::xrange(8), value, pos) {
    regs.d[1] = value << pos;
    raw_write16(0, 0004001);
    raw_write16(2, pos);
    m68k_do_execute();
    BOOST_TEST(regs.z != value);
}

BOOST_DATA_TEST_CASE(opc_d, REG(), ea) {
    BOOST_TEST(opc_map[0004000 | ea] == opc_map[0004000]);
}

BOOST_DATA_TEST_CASE(opc_m, EA_M_R(), ea) {
    BOOST_TEST(opc_map[0004000 | ea] == opc_map[0004020]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(result, BIT *bdata::xrange(8), value, pos) {
    regs.a[1] = 0x10;
    regs.d[2] = pos;
    raw_write16(0, 0002421); 
    raw_write8(0x10, value << pos);
    m68k_do_execute();
    BOOST_TEST(regs.z != value);
}

BOOST_DATA_TEST_CASE(opc_m, REG() * EA_M_R(), dn, ea) {
    BOOST_TEST(opc_map[0000400 | dn << 9 | ea] == opc_map[0000420]);
}

BOOST_DATA_TEST_CASE(Long, BIT *bdata::xrange(32), value, pos) {
    regs.d[2] = 13;
    regs.d[1] = value << 13;
    asm_m68k("btst %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.z != value);
}

BOOST_DATA_TEST_CASE(opc_d, REG() * REG(), dn, ea) {
    BOOST_TEST(opc_map[0000400 | dn << 9 | ea] == opc_map[0000400]);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()