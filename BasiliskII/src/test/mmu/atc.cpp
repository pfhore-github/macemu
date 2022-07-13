#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s);
BOOST_FIXTURE_TEST_SUITE(MMU, InitFix)
const auto mmu_p = { &mmu_i, &mmu_d};

BOOST_DATA_TEST_CASE(local, BIT * mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_p = false;
    mmu.tcr_e = true;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
        mmu_r->atc_cache_l_s[0x2000] =
            atc_entry_t{1, true, false, false, 0, s, 0};
    } else {
        mmu.urp = 0x1000;
        mmu_r->atc_cache_l_u[0x2000] =
            atc_entry_t{1, true, false, false, 0, s, 0};
    }
    raw_write32(0x1004, 0);
    auto ret = mmu_r->ptest(0x2000, false, s);
    BOOST_TEST(!ret.G);
    BOOST_TEST((ret.addr + 0) == 1);
}

BOOST_DATA_TEST_CASE(global, BIT * mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_p = false;
    mmu.tcr_e = true;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
        mmu_r->atc_cache_g_s[0x2000] =
            atc_entry_t{1, true, false, false, 0, true, 0};
    } else {
        mmu.urp = 0x1000;
        mmu_r->atc_cache_g_u[0x2000] =
            atc_entry_t{1, true, false, false, 0, false, 0};
    }
    raw_write32(0x1004, 0);
    raw_write32(0x3000 + 2 * 4, 0x4000);
    auto ret = mmu_r->ptest(0x2000, true, s);
    BOOST_TEST((ret.G == true));
    BOOST_TEST((ret.addr + 0) == 1);
}

BOOST_AUTO_TEST_SUITE_END()
