#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(BTST, InitFix)

BOOST_AUTO_TEST_SUITE(Imm)
BOOST_AUTO_TEST_CASE(Byte) {
    auto ea = rand_reg();
    auto v1 = get_v8();
    int n = get_vn(0, 7);
    regs.a[ea] = 0x10;
    raw_write8(0x10, v1);
    raw_write16(0, 0004020 | ea);
    raw_write16(2, n);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
}

BOOST_AUTO_TEST_CASE(Long) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    int n = get_vn(0, 31);
    regs.d[ea] = v1;
    raw_write16(0, 0004000 | ea);
    raw_write16(2, n);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(Byte) {
    auto [ea, dn] = rand_reg2();
    auto v1 = get_v8();
    int n = get_vn(0, 7);
    regs.a[ea] = 0x10;
    regs.d[dn] = n;
    raw_write16(0, 0000420 | dn << 9 | ea);
    raw_write8(0x10, v1);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
}

BOOST_AUTO_TEST_CASE(Long) {
    auto [ea, dn] = rand_reg2();
    auto v1 = get_v32();
    int n = get_vn(0, 31);
    regs.d[ea] = v1;
    regs.d[dn] = n;
    raw_write16(0, 0000400 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()