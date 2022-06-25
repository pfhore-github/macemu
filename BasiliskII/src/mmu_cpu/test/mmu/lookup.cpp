#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MMU, InitFix)
static const auto mmu_p = {&mmu_i, &mmu_d};
BOOST_DATA_TEST_CASE(bus_error, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x60000000;
    } else {
        mmu.urp = 0x60000000;
    }
    auto ret = mmu_r->ptest(0x2000000, true, s);
    BOOST_TEST(static_cast<bool>(ret.B));
}

BOOST_DATA_TEST_CASE(root_error, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0);
    auto ret = mmu_r->ptest(0x2000000, true, s);
    BOOST_TEST(!ret.R);
}

BOOST_DATA_TEST_CASE(lv2_error, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0);
    auto ret = mmu_r->ptest(0x2080000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
}

BOOST_DATA_TEST_CASE(lv3_error, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0);
    auto ret = mmu_r->ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_indirect_error, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
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
    auto ret = mmu_r->ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_indirect_nest_error, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
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
    auto ret = mmu_r->ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_priv_error, mmu_p, mmu_r) {
    regs.S = false;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = 1;
    mmu.pflush();
    mmu.urp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5081);
    auto ret = mmu_r->ptest(0x2083000, true, false);
    BOOST_TEST(static_cast<bool>(ret.S));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x5089);
}

BOOST_DATA_TEST_CASE(root_write_protect, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2006);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5001);
    auto ret = mmu_r->ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200E);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x5009);
}

BOOST_DATA_TEST_CASE(lv2_write_protect, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3006);
    raw_write32(0x300C, 0x5001);
    auto ret = mmu_r->ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300E);
    BOOST_TEST(raw_read32(0x300C) == 0x5009);
}

BOOST_DATA_TEST_CASE(lv3_write_protect, BIT *mmu_p, s, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5007);
    auto ret = mmu_r->ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x500F);
}

BOOST_DATA_TEST_CASE(ok, BIT *BIT *BIT *mmu_p, s, w, g, mmu_r) {
    regs.S = s;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (g << 10));
    auto ret = mmu_r->ptest(0x2083022, w, s);
    BOOST_TEST((ret.R == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == (0x500B | (g << 10) | w << 4));
    BOOST_TEST((ret.addr + 0) == 0x5);
}

BOOST_DATA_TEST_CASE(p_ok, BIT *BIT *mmu_p, s, w, mmu_r) {
    regs.S = s;
    mmu.tcr_p = true;
    mmu.tcr_e = true;
    mmu_r->TTR[0].E = false;
    mmu_r->TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    if(s) {
        mmu.srp = 0x1000;
    } else {
        mmu.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x6003);
    auto ret = mmu_r->ptest(0x2086022, w, s);
    BOOST_TEST((ret.R == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == (0x600B | w << 4));
    BOOST_TEST((ret.addr + 0) == 0x6);
}
BOOST_AUTO_TEST_SUITE_END()