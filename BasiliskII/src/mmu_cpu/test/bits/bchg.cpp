#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(BCHG, InitFix)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_AUTO_TEST_CASE(Byte) {
    auto xr = rand_reg();
    auto v1 = get_v8();
    int n = get_vn(0, 7);
    regs.a[xr] = 0x10;
    raw_write8(0x10, v1);
    raw_write16(0, 0004120 | xr);
    raw_write16(2, n);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
    BOOST_TEST(raw_read8(0x10) == (v1 ^ 1 << n));
}

BOOST_AUTO_TEST_CASE(Long) {
    auto xr = rand_reg();
    auto v1 = get_v32();
    int n = get_vn(0, 31);
    regs.d[xr] = v1;
    raw_write16(0, 0004100 | xr);
    raw_write16(2, n);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
    BOOST_TEST(regs.d[xr] == (v1 ^ 1 << n));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(Byte) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v8();
    int n = get_vn(0, 7);
    regs.a[yr] = 0x10;
    regs.d[xr] = n;
    raw_write16(0, 0000520 | xr << 9 | yr);
    raw_write8(0x10, v1);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
    BOOST_TEST(raw_read8(0x10) == (v1 ^ 1 << n));
}

BOOST_AUTO_TEST_CASE(Long) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v32();
    int n = get_vn(0, 31);
    regs.d[yr] = v1;
    regs.d[xr] = n;
    raw_write16(0, 0000500 | xr << 9 | yr);
    m68k_do_execute();
    BOOST_TEST(regs.z == !(v1 & 1 << n));
    BOOST_TEST(regs.d[yr] == (v1 ^ 1 << n));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
