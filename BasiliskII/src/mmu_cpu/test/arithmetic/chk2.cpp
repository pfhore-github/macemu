#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(CHK2, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x40;
    raw_write16(0, 0000322);
    raw_write16(2, 0x3800);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x10;
    raw_write16(0, 0000322);
    raw_write16(2, 0x3800);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x50;
    raw_write16(0, 0000322);
    raw_write16(2, 0x3800);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x05;
    raw_write16(0, 0000322);
    raw_write16(2, 0x3800);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x55;
    raw_write16(0, 0000322);
    raw_write16(2, 0x3800);
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x4000;
    raw_write16(0, 0001322);
    raw_write16(2, 0x3800);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x1000;
    raw_write16(0, 0001322);
    raw_write16(2, 0x3800);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x5000;
    raw_write16(0, 0001322);
    raw_write16(2, 0x3800);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x0005;
    raw_write16(0, 0001322);
    raw_write16(2, 0x3800);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x5555;
    raw_write16(0, 0001322);
    raw_write16(2, 0x3800);
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x40000000;
    raw_write16(0, 0002322);
    raw_write16(2, 0x3800);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(low_eq) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x10000000;
    raw_write16(0, 0002322);
    raw_write16(2, 0x3800);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x10000000;
    raw_write16(0, 0002322);
    raw_write16(2, 0x3800);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x00050000;
    raw_write16(0, 0002322);
    raw_write16(2, 0x3800);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x55550000;
    raw_write16(0, 0002322);
    raw_write16(2, 0x3800);
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    exception_check(6);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
