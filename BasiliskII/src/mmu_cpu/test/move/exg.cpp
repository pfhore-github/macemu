#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(MOVE, InitFix)
BOOST_AUTO_TEST_CASE(DR2DR) {
    auto [rx, ry] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[rx] = v1;
    regs.d[ry] = v2;
    raw_write16(0, 0140500 | rx << 9 | ry);
    m68k_do_execute();
    BOOST_TEST(regs.d[rx] == v2);
    BOOST_TEST(regs.d[ry] == v1);
}

BOOST_AUTO_TEST_CASE(AR2AR) {
    auto [rx, ry] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.a[rx] = v1;
    regs.a[ry] = v2;
    raw_write16(0, 0140510 | rx << 9 | ry);
    m68k_do_execute();
    BOOST_TEST(regs.a[rx] == v2);
    BOOST_TEST(regs.a[ry] == v1);
}

BOOST_AUTO_TEST_CASE(AR2DR) {
    auto rx = rand_reg();
    auto ry = rand_ar();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[rx] = v1;
    regs.a[ry] = v2;
    raw_write16(0, 0140610 | rx << 9 | ry);
    m68k_do_execute();
    BOOST_TEST(regs.d[rx] == v2);
    BOOST_TEST(regs.a[ry] == v1);
}
BOOST_AUTO_TEST_SUITE_END()