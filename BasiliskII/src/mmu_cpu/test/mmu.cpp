#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
presult test_TTR(const uint32_t addr, bool is_code, bool rw);
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s);
BOOST_FIXTURE_TEST_SUITE(MMU, InitFix)
BOOST_AUTO_TEST_SUITE(code)
BOOST_DATA_TEST_CASE(disabled, BIT, s) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu.ITTR[0].E = false;
    mmu.ITTR[1].E = false;
    uint32_t ret = do_mmu(0x1000, true, false, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_invalid, BIT, s) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu.DTTR[0].E = true;
    mmu.DTTR[1].E = true;
    uint32_t ret = do_mmu(0x1000, true, false, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_addr_mismatch, BIT, s) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu.ITTR[0].E = true;
    mmu.ITTR[0].address_base = 0x2f;
    mmu.ITTR[1].E = false;
    uint32_t ret = do_mmu(0x1000, true, false, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_mode_mismatch, BIT, s) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu.ITTR[0].E = true;
    mmu.ITTR[0].address_base = 0x3f;
    mmu.ITTR[0].address_mask = 0x1f;
    mmu.ITTR[0].S = s ? 0 : 1;
    mmu.ITTR[1].E = false;
    uint32_t ret = do_mmu(0x3f000000, true, false, s);
    BOOST_TEST(ret == 0x3f000000);
}
BOOST_DATA_TEST_CASE(ttr_write_protected, BIT, s) {
    regs.S = s;
    mmu.tcr_e = false;
    mmu.ITTR[0].E = true;
    mmu.ITTR[0].address_base = 0x3f;
    mmu.ITTR[0].address_mask = 0x1f;
    mmu.ITTR[0].S = s ? 1 : 0;
    mmu.ITTR[0].W = true;

    mmu.ITTR[1].E = false;
    auto ret = test_TTR(0x3f000000, true, true);
    BOOST_TEST((ret.W == true));
}

BOOST_DATA_TEST_CASE(atc_match_l, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    mmu.atc_lcache[(s << 31) | 0x2000] =
        atc_entry_t{1, true, false, false, 0, s, 0};
    raw_write32(0x1004, 0);
    auto ret = mmu.ptest(0x2000200, false, s);
    BOOST_TEST(!ret.G);
    BOOST_TEST((ret.addr + 0) == 1);
}

BOOST_DATA_TEST_CASE(atc_match_g, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    mmu.atc_gcache[(s << 31) | 0x2000] =
        atc_entry_t{1, true, false, false, 0, s, 0};
    raw_write32(0x1004, 0);
    raw_write32(0x3000 + 2 * 4, 0x4000);
    auto ret = mmu.ptest(0x2000200, true, s);
    BOOST_TEST((ret.G == true));
    BOOST_TEST((ret.addr + 0) == 1);
}

BOOST_DATA_TEST_CASE(root_error, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0);
    auto ret = mmu.ptest(0x2000000, true, s);
    BOOST_TEST(!ret.R);
}

BOOST_DATA_TEST_CASE(lv2_error, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0);
    auto ret = mmu.ptest(0x2080000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
}

BOOST_DATA_TEST_CASE(lv3_error, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0);
    auto ret = mmu.ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_indirect_error, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x4002);
    raw_write32(0x4000, 0x0);
    auto ret = mmu.ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_indirect_nest_error, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x4002);
    raw_write32(0x4000, 0x5002);
    auto ret = mmu.ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_AUTO_TEST_CASE(lv3_priv_error) {
    regs.S = false;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    mmu.urp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5081);
    auto ret = mmu.ptest(0x2083000, true, false);
    BOOST_TEST((ret.S == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x5089);
}

BOOST_DATA_TEST_CASE(root_write_protect, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2006);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5001);
    auto ret = mmu.ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200E);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x5009);
}

BOOST_DATA_TEST_CASE(lv2_write_protect, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3006);
    raw_write32(0x300C, 0x5001);
    auto ret = mmu.ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300E);
    BOOST_TEST(raw_read32(0x300C) == 0x5009);
}

BOOST_DATA_TEST_CASE(lv3_write_protect, BIT, s) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5007);
    auto ret = mmu.ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x500F);
}

BOOST_DATA_TEST_CASE(ok, BIT *BIT, s, w) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003);
    auto ret = mmu.ptest(0x2083022, w, s);
    BOOST_TEST((ret.R == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == (0x500B | w << 4));
    BOOST_TEST((ret.addr + 0) == 0x5);
}

BOOST_DATA_TEST_CASE(p_ok, BIT *BIT, s, w) {
    regs.S = s;
    mmu.tcr_p = true;
    mmu.tcr_e = true;
    mmu.DTTR[0].E = false;
    mmu.ITTR[0].E = false;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x6003);
    auto ret = mmu.ptest(0x2086022, w, s);
    BOOST_TEST((ret.R == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == (0x600B | w << 4));
    BOOST_TEST((ret.addr + 0) == 0x6);
}

BOOST_AUTO_TEST_SUITE_END()

// BOOST_AUTO_TEST_SUITE(data)

// BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()