
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(SUBQ, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), im) {
    BOOST_TEST(opc_map[00504 | im << 3] == opc_map[00504]);
    BOOST_TEST(opc_map[00505 | im << 3] == opc_map[00505]);
    BOOST_TEST(opc_map[00506 | im << 3] == opc_map[00506]);
}


BOOST_DATA_TEST_CASE(Byte, REG(), im) {
    regs.d[1] = 33;
    raw_write16(0, 0050401 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 33 - (im ? im : 8));
}

BOOST_DATA_TEST_CASE(Word, REG(), im) {
    regs.d[1] = 3456;
    raw_write16(0, 0050501 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 3456 - (im ? im : 8));
}

BOOST_DATA_TEST_CASE(AWord, REG(), im) {
    regs.a[1] = 0x1234;
    raw_write16(0, 0050511 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1234 - (im ? im : 8));
}

BOOST_DATA_TEST_CASE(Long, REG(), im) {
    regs.d[1] = 123456789;
    raw_write16(0, 0050601 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456789- (im ? im : 8));
}

BOOST_DATA_TEST_CASE(ALong, REG(), im) {
    regs.a[1] = 0x20000002;
    raw_write16(0, 0050611 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000002- (im ? im : 8));
}

BOOST_AUTO_TEST_SUITE_END()
