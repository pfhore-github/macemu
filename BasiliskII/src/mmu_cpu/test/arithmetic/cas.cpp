#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(CMP2, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(eq) {
    auto v1 = get_v8();
    auto v2 = get_v8();
    auto [ur, cr] = rand_reg2();
    auto ea = rand_reg();
    regs.d[ur] = v1;
    regs.d[cr] = v2;
    regs.a[ea] = 0x10;
    raw_write16(0, 0005320 | ea);
    raw_write16(2, ur << 6 | cr);
    raw_write8(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == v1);
    BOOST_TEST(regs.d[cr] == v2);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ne) {
    auto v1 = get_v8();
    auto v2 = get_v8();
    auto v3 = get_v8();
    if(v1 == v3) {
        ++v3;
    }
    auto [ur, cr] = rand_reg2();
    auto ea = rand_reg();
    regs.d[ur] = v1;
    regs.d[cr] = v2;
    regs.a[ea] = 0x10;
    raw_write8(0x10, v3);
    raw_write16(0, 0005320 | ea);
    raw_write16(2, ur << 6 | cr);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == v3);
    BOOST_TEST(regs.d[cr] == v3);
    BOOST_TEST(!regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(eq) {
    auto v1 = get_v16();
    auto v2 = get_v16();
    auto [ur, cr] = rand_reg2();
    auto ea = rand_reg();
    regs.d[ur] = v1;
    regs.d[cr] = v2;
    regs.a[ea] = 0x10;
    raw_write16(0, 0006320 | ea);
    raw_write16(2, ur << 6 | cr);
    raw_write16(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == v1);
    BOOST_TEST(regs.d[cr] == v2);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ne) {
    auto v1 = get_v16();
    auto v2 = get_v16();
    auto v3 = get_v16();
    if(v1 == v3) {
        ++v3;
    }
    auto [ur, cr] = rand_reg2();
    auto ea = rand_reg();
    regs.d[ur] = v1;
    regs.d[cr] = v2;
    regs.a[ea] = 0x10;
    raw_write16(0x10, v3);
    raw_write16(0, 0006320 | ea);
    raw_write16(2, ur << 6 | cr);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == v3);
    BOOST_TEST(regs.d[cr] == v3);
    BOOST_TEST(!regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(eq) {
    auto v1 = get_v32();
    auto v2 = get_v32();
    auto [ur, cr] = rand_reg2();
    auto ea = rand_reg();
    regs.d[ur] = v1;
    regs.d[cr] = v2;
    regs.a[ea] = 0x10;
    raw_write16(0, 0007320 | ea);
    raw_write16(2, ur << 6 | cr);
    raw_write32(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == v1);
    BOOST_TEST(regs.d[cr] == v2);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ne) {
    auto v1 = get_v32();
    auto v2 = get_v32();
    auto v3 = get_v32();
    if(v1 == v3) {
        ++v3;
    }
    auto [ur, cr] = rand_reg2();
    auto ea = rand_reg();
    regs.d[ur] = v1;
    regs.d[cr] = v2;
    regs.a[ea] = 0x10;
    raw_write32(0x10, v3);
    raw_write16(0, 0007320 | ea);
    raw_write16(2, ur << 6 | cr);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == v3);
    BOOST_TEST(regs.d[cr] == v3);
    BOOST_TEST(!regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()