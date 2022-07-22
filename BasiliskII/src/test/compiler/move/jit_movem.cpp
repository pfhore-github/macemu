#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(MOVEM)

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(To)
BOOST_AUTO_TEST_CASE(addr) {
    regs.i_ea = 0;
    regs.d[1] = 0x1000;
    regs.d[2] = 0x2000;
    regs.d[3] = 0x3000;
    regs.a[1] = 0x4000;
    regs.a[2] = 0x5000;
    regs.a[3] = 0x6000;
    raw_write16(0, 0044222);
    raw_write16(2, 0x0E0E);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x5000) == 0x1000);
    BOOST_TEST(raw_read16(0x5002) == 0x2000);
    BOOST_TEST(raw_read16(0x5004) == 0x3000);
    BOOST_TEST(raw_read16(0x5006) == 0x4000);
    BOOST_TEST(raw_read16(0x5008) == 0x5000);
    BOOST_TEST(raw_read16(0x500A) == 0x6000);
}

BOOST_AUTO_TEST_CASE(decr) {
    regs.i_ea = 0;
    regs.d[1] = 0x1000;
    regs.d[2] = 0x2000;
    regs.d[3] = 0x3000;
    regs.a[1] = 0x4000;
    regs.a[2] = 0x500C;
    regs.a[3] = 0x6000;
    raw_write16(0, 0044242);
    raw_write16(2, 0x7070);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x500A) == 0x6000);
    BOOST_TEST(raw_read16(0x5008) == 0x500A);
    BOOST_TEST(raw_read16(0x5006) == 0x4000);
    BOOST_TEST(raw_read16(0x5004) == 0x3000);
    BOOST_TEST(raw_read16(0x5002) == 0x2000);
    BOOST_TEST(raw_read16(0x5000) == 0x1000);
    BOOST_TEST(regs.a[2] == 0x5000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(From)
BOOST_AUTO_TEST_CASE(addr) {
    regs.i_ea = 0;
    regs.a[2] = 0x5000;
    raw_write16(0x5000, 0x1000);
    raw_write16(0x5002, 0x2000);
    raw_write16(0x5004, 0x3000);
    raw_write16(0x5006, 0x4000);
    raw_write16(0x5008, 0x5000);
    raw_write16(0x500A, 0x6000);
    raw_write16(0, 0046222);
    raw_write16(2, 0x0E0E);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == 0x1000);
    BOOST_TEST(regs.d[2] == 0x2000);
    BOOST_TEST(regs.d[3] == 0x3000);
    BOOST_TEST(regs.a[1] == 0x4000);
    BOOST_TEST(regs.a[2] == 0x5000);
    BOOST_TEST(regs.a[3] == 0x6000);
}

BOOST_AUTO_TEST_CASE(incr) {
    regs.i_ea = 0;
    regs.a[2] = 0x5000;
    raw_write16(0x5000, 0x1000);
    raw_write16(0x5002, 0x2000);
    raw_write16(0x5004, 0x3000);
    raw_write16(0x5006, 0x4000);
    raw_write16(0x5008, 0x5000);
    raw_write16(0x500A, 0x6000);
    raw_write16(0, 0046232);
    raw_write16(2, 0x0E0E);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == 0x1000);
    BOOST_TEST(regs.d[2] == 0x2000);
    BOOST_TEST(regs.d[3] == 0x3000);
    BOOST_TEST(regs.a[1] == 0x4000);
    BOOST_TEST(regs.a[3] == 0x6000);
    BOOST_TEST(regs.a[2] == 0x500c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(To)
BOOST_AUTO_TEST_CASE(addr) {
    regs.i_ea = 0;
    regs.d[1] = 0x100000;
    regs.d[2] = 0x200000;
    regs.d[3] = 0x300000;
    regs.a[1] = 0x400000;
    regs.a[2] = 0x5000;
    regs.a[3] = 0x600000;
    raw_write16(0, 0044322);
    raw_write16(2, 0x0E0E);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read32(0x5000) == 0x100000);
    BOOST_TEST(raw_read32(0x5004) == 0x200000);
    BOOST_TEST(raw_read32(0x5008) == 0x300000);
    BOOST_TEST(raw_read32(0x500C) == 0x400000);
    BOOST_TEST(raw_read32(0x5010) == 0x5000);
    BOOST_TEST(raw_read32(0x5014) == 0x600000);
}

BOOST_AUTO_TEST_CASE(decr) {
    regs.i_ea = 0;
    regs.d[1] = 0x100000;
    regs.d[2] = 0x200000;
    regs.d[3] = 0x300000;
    regs.a[1] = 0x400000;
    regs.a[2] = 0x5018;
    regs.a[3] = 0x600000;
    raw_write16(0, 0044342);
    raw_write16(2, 0x7070);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read32(0x5014) == 0x600000);
    BOOST_TEST(raw_read32(0x5010) == 0x5014);
    BOOST_TEST(raw_read32(0x500C) == 0x400000);
    BOOST_TEST(raw_read32(0x5008) == 0x300000);
    BOOST_TEST(raw_read32(0x5004) == 0x200000);
    BOOST_TEST(raw_read32(0x5000) == 0x100000);
    BOOST_TEST(regs.a[2] == 0x5000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(From)
BOOST_AUTO_TEST_CASE(addr) {
    regs.i_ea = 0;
    regs.a[2] = 0x5000;
    raw_write32(0x5000, 0x100000);
    raw_write32(0x5004, 0x200000);
    raw_write32(0x5008, 0x300000);
    raw_write32(0x500c, 0x400000);
    raw_write32(0x5010, 0x500000);
    raw_write32(0x5014, 0x600000);
    raw_write16(0, 0046322);
    raw_write16(2, 0x0E0E);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == 0x100000);
    BOOST_TEST(regs.d[2] == 0x200000);
    BOOST_TEST(regs.d[3] == 0x300000);
    BOOST_TEST(regs.a[1] == 0x400000);
    BOOST_TEST(regs.a[2] == 0x500000);
    BOOST_TEST(regs.a[3] == 0x600000);
}

BOOST_AUTO_TEST_CASE(incr) {
    regs.i_ea = 0;
    regs.a[2] = 0x5000;
    raw_write32(0x5000, 0x100000);
    raw_write32(0x5004, 0x200000);
    raw_write32(0x5008, 0x300000);
    raw_write32(0x500c, 0x400000);
    raw_write32(0x5010, 0x500000);
    raw_write32(0x5014, 0x600000);
    raw_write16(0, 0046332);
    raw_write16(2, 0x0E0E);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == 0x100000);
    BOOST_TEST(regs.d[2] == 0x200000);
    BOOST_TEST(regs.d[3] == 0x300000);
    BOOST_TEST(regs.a[1] == 0x400000);
    BOOST_TEST(regs.a[3] == 0x600000);
    BOOST_TEST(regs.a[2] == 0x5018);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
