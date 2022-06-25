#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s);
BOOST_FIXTURE_TEST_SUITE(MMU, InitFix)
const auto mmu_p = { &mmu_i, &mmu_d};


BOOST_DATA_TEST_CASE(invalid, mmu_p, mmu_r) {
    regs.S = false;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x1000);
    BOOST_TEST(!ret.R);
}

BOOST_DATA_TEST_CASE(addr_mismatch, mmu_p, mmu_r) {
    regs.S = false;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x2f;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x1000);
    BOOST_TEST(!ret.R);
}

BOOST_DATA_TEST_CASE(mode_mismatch, BIT * mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x3f;
    mmu_r->TTR[0].address_mask = 0x1f;
    mmu_r->TTR[0].S = !s;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x3f000000);
    BOOST_TEST(!ret.R);
}
BOOST_DATA_TEST_CASE(ok_s, BIT* mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x3f;
    mmu_r->TTR[0].address_mask = 0x1f;
    mmu_r->TTR[0].S = s;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x3f000000);
    BOOST_TEST(static_cast<bool>(ret.R));
}

BOOST_DATA_TEST_CASE(ok_2, mmu_p, mmu_r) {
    regs.S = true;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x3f;
    mmu_r->TTR[0].address_mask = 0x1f;
    mmu_r->TTR[0].S = false;
    mmu_r->TTR[1].E = true;
    mmu_r->TTR[1].address_base = 0x3f;
    mmu_r->TTR[1].address_mask = 0x1f;
    mmu_r->TTR[1].S = true;
    auto ret = mmu_r->test_TTR(0x3f000000);
    BOOST_TEST(static_cast<bool>(ret.R));
}

BOOST_DATA_TEST_CASE(ok_u, bdata::xrange(4) * mmu_p, u, mmu_r) {
    regs.S = true;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x3f;
    mmu_r->TTR[0].address_mask = 0x1f;
    mmu_r->TTR[0].S = true;
    mmu_r->TTR[0].U = u;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x3f000000);
    BOOST_TEST(static_cast<int>(ret.U) == u);
}

BOOST_DATA_TEST_CASE(ok_cm, bdata::xrange(4)* mmu_p, cm, mmu_r) {
    regs.S = true;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x3f;
    mmu_r->TTR[0].address_mask = 0x1f;
    mmu_r->TTR[0].S = true;
    mmu_r->TTR[0].CM = cm;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x3f000000);
    BOOST_TEST(static_cast<int>(ret.CM) == cm);
}

BOOST_DATA_TEST_CASE(ok_w, BIT* mmu_p, w, mmu_r) {
    regs.S = true;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = true;
    mmu_r->TTR[0].address_base = 0x3f;
    mmu_r->TTR[0].address_mask = 0x1f;
    mmu_r->TTR[0].S = true;
    mmu_r->TTR[0].W = w;
    mmu_r->TTR[1].E = false;
    auto ret = mmu_r->test_TTR(0x3f000000);
    BOOST_TEST(static_cast<bool>(ret.W) == w);
}
BOOST_AUTO_TEST_SUITE_END()
