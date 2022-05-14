#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(NEGX, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(cx, BIT, value) {
    regs.d[1] = 44;
    regs.x = value;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 - 44 - value);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 10;
    regs.x = false;
    regs.z = true;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(cx, bdata::xrange(1), value) {
    regs.d[1] = 440;
    regs.x = value;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 65536 - 440 - value);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    regs.x = false;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 10;
    regs.x = false;
    regs.z = true;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    regs.x = false;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(cx, bdata::xrange(1), value) {
    regs.d[1] = 440000;
    regs.x = value;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffffff - 440000 - value + 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    regs.x = false;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 10;
    regs.x = false;
    regs.z = true;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    regs.x = false;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()