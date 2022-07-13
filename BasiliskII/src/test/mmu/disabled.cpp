#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MMU, InitFix)
const auto mmu_p = { &mmu_i, &mmu_d};
BOOST_DATA_TEST_CASE(disabled, BIT * BIT * mmu_p, s, w, mmu_r) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    uint32_t ret = mmu_r->do_mmu(0x1000, w, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_AUTO_TEST_SUITE_END()