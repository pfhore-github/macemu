#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(CMP2, InitFix)
BOOST_AUTO_TEST_SUITE(WithDn)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x40;
    raw_write16(0, 0000323);
    raw_write16(2, 0x4000 );
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x10;
    raw_write16(0, 0000323);
    raw_write16(2, 0x4000 );
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high_eq) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x50;
    raw_write16(0, 0000323);
    raw_write16(2, 0x4000 );
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x05;
    raw_write16(0, 0000323);
    raw_write16(2, 0x4000 );
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x55;
    raw_write16(0, 0000323);
    raw_write16(2, 0x4000);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x4000;
    raw_write16(0, 0001323);
    raw_write16(2, 0x4000 );
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x1000;
    raw_write16(0, 0001323);
    raw_write16(2, 0x4000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high_eq) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x5000;
    raw_write16(0, 0001323);
    raw_write16(2, 0x4000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x0005;
    raw_write16(0, 0001323);
    raw_write16(2, 0x4000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x5555;
    raw_write16(0, 0001323);
    raw_write16(2, 0x4000);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x40000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x4000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x10000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x4000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high_eq) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x10000000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x4000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x00050000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x4000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.d[4] = 0x55550000;
    raw_write16(0, 0002323);
    raw_write16(2, 0x4000);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(WithAn)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.a[4] = -20;
    raw_write16(0, 0000323);
    raw_write16(2, 0xC000 );
    raw_write8(0x10, -50);
    raw_write8(0x11, -10);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.a[4] = -50;
    raw_write16(0, 0000323);
    raw_write16(2, 0xC000 );
    raw_write8(0x10, -50);
    raw_write8(0x11, -10);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high_eq) {
    regs.a[3] = 0x10;
    regs.a[4] = -10;
    raw_write16(0, 0000323);
    raw_write16(2, 0xC000 );
    raw_write8(0x10, -50);
    raw_write8(0x11, -10);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.a[4] = -70;
    raw_write16(0, 0000323);
    raw_write16(2, 0xC000 );
    raw_write8(0x10, -50);
    raw_write8(0x11, -10);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.a[4] = 40;
    raw_write16(0, 0000323);
    raw_write16(2, 0xC000);
    raw_write8(0x10, -50);
    raw_write8(0x11, -10);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.a[4] = -2000;
    raw_write16(0, 0001323);
    raw_write16(2, 0xC000 );
    raw_write16(0x10, -3000);
    raw_write16(0x12, -1000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.a[4] = -3000;
    raw_write16(0, 0001323);
    raw_write16(2, 0xC000);
    raw_write16(0x10, -3000);
    raw_write16(0x12, -1000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high_eq) {
    regs.a[3] = 0x10;
    regs.a[4] = -1000;
    raw_write16(0, 0001323);
    raw_write16(2, 0xC000);
    raw_write16(0x10, -3000);
    raw_write16(0x12, -1000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.a[4] = -4000;
    raw_write16(0, 0001323);
    raw_write16(2, 0xC000);
    raw_write16(0x10, -3000);
    raw_write16(0x12, -1000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.a[4] = 1000;
    raw_write16(0, 0001323);
    raw_write16(2, 0xC000);
    raw_write16(0x10, -3000);
    raw_write16(0x12, -1000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[3] = 0x10;
    regs.a[4] = -50000;
    raw_write16(0, 0002323);
    raw_write16(2, 0xC000);
    raw_write32(0x10, -70000);
    raw_write32(0x14, -30000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[3] = 0x10;
    regs.a[4] = -70000;
    raw_write16(0, 0002323);
    raw_write16(2, 0xC000);
    raw_write32(0x10, -70000);
    raw_write32(0x14, -30000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high_eq) {
    regs.a[3] = 0x10;
    regs.a[4] = -30000;
    raw_write16(0, 0002323);
    raw_write16(2, 0xC000);
    raw_write32(0x10, -70000);
    raw_write32(0x14, -30000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[3] = 0x10;
    regs.a[4] = -90000;
    raw_write16(0, 0002323);
    raw_write16(2, 0xC000);
    raw_write32(0x10, -70000);
    raw_write32(0x14, -30000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[3] = 0x10;
    regs.a[4] = -10000;
    raw_write16(0, 0002323);
    raw_write16(2, 0xC000);
    raw_write32(0x10, -70000);
    raw_write32(0x14, -30000);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
