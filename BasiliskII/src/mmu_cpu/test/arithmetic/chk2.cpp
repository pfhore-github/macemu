#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(CHK2, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(in_range) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x40;
    raw_write16(0, 0000320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x10;
    raw_write16(0, 0000320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x50;
    raw_write16(0, 0000320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x05;
    raw_write16(0, 0000320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x55;
    raw_write16(0, 0000320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x4000;
    raw_write16(0, 0001320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x1000;
    raw_write16(0, 0001320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x5000;
    raw_write16(0, 0001320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x0005;
    raw_write16(0, 0001320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x5555;
    raw_write16(0, 0001320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x40000000;
    raw_write16(0, 0002320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x10000000;
    raw_write16(0, 0002320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x10000000;
    raw_write16(0, 0002320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x00050000;
    raw_write16(0, 0002320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    auto rn = rand_reg();
    int ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.d[rn] = 0x55550000;
    raw_write16(0, 0002320 | ea);
    raw_write16(2, 0x0800 | rn << 12);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
