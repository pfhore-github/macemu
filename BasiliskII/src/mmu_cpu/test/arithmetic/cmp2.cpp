#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(CMP2, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x40;
    raw_write16(0, 0000322);
    raw_write16(2, 0x3000);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x10;

    raw_write16(0, 0000322);
    raw_write16(2, 0x3000);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x50;

    raw_write16(0, 0000323);
    raw_write16(2, 0x4000);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[4] = 0x10;
    regs.d[5] = 0x05;

    raw_write16(0, 0000324);
    raw_write16(2, 0x5000);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[5] = 0x10;
    regs.d[6] = 0x55;

    raw_write16(0, 0000325);
    raw_write16(2, 0x6000);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_DATA_TEST_CASE(opc, EA_M_R(), ea) {
    BOOST_TEST(opc_map[0000300 | ea] == opc_map[0000320]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x4000;
    raw_write16(0, 0001323);
    raw_write16(2, 0x2000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(in_rangeA) {
    regs.a[3] = 0x10;
    regs.a[2] = 0x4000;
    raw_write16(0, 0001323);
    raw_write16(2, 0xa000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x1000;
    raw_write16(0, 0001323);
    raw_write16(2, 0x2000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x5000;
    raw_write16(0, 0001323);
    raw_write16(2, 0x2000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x0005;
    raw_write16(0, 0001323);
    raw_write16(2, 0x2000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x5500;
    raw_write16(0, 0001323);
    raw_write16(2, 0x2000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_DATA_TEST_CASE(opc, EA_M_R(), ea) {
    BOOST_TEST(opc_map[0001300 | ea] == opc_map[0001320]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x40000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x2000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x10000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x2000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x50000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x2000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x00050000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x2000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.d[2] = 0x55000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x2000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_DATA_TEST_CASE(opc, EA_M_R(), ea) {
    BOOST_TEST(opc_map[0002300 | ea] == opc_map[0002320]);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()