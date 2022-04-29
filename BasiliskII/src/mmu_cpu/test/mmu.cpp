#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
uint32_t do_mmu(const paddr &vaddr);
extern jmp_buf ex_jmp;
struct atc_entry_t {
    uint32_t addr;
    uint8_t u;
    bool s;
    uint8_t cm;
    bool m;
    bool w;
    bool r;
};
extern std::unordered_map<uint32_t, atc_entry_t> atc_gcache, atc_lcache;
BOOST_FIXTURE_TEST_SUITE(MMU, InitFix)
BOOST_AUTO_TEST_SUITE(code)
BOOST_DATA_TEST_CASE(disabled, BIT, s) {
    regs.tcr_e = false;
    regs.ITTR[0].E = false;
    regs.ITTR[1].E = false;
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    paddr pe{0x1000, 0, SZ::WORD, TT::NORMAL, tm, false};
    uint32_t ret = do_mmu(pe);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_invalid, BIT, s) {
    regs.tcr_e = false;
    regs.DTTR[0].E = true;
    regs.DTTR[1].E = true;
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    paddr pe{0x1000, 0, SZ::WORD, TT::NORMAL, tm, false};
    uint32_t ret = do_mmu(pe);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_addr_mismatch, BIT, s) {
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x2f;
    regs.ITTR[1].E = false;
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    paddr pe{0x1000, 0, SZ::WORD, TT::NORMAL, tm, false};
    uint32_t ret = do_mmu(pe);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_mode_mismatch, BIT, s) {
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x3f;
    regs.ITTR[0].address_mask = 0x1f;
    regs.ITTR[0].S = s ? 0 : 1;
    regs.ITTR[1].E = false;
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    paddr pe{0x3f000000, 0, SZ::WORD, TT::NORMAL, tm, false};
    uint32_t ret = do_mmu(pe);
    BOOST_TEST(ret == 0x3f000000);
}
BOOST_DATA_TEST_CASE(ttr_write_protected, BIT, s) {
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x3f;
    regs.ITTR[0].address_mask = 0x1f;
    regs.ITTR[0].S = s ? 1 : 0;
    regs.ITTR[0].W = true;

    regs.ITTR[1].E = false;

    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0x1000;
    regs.isp = regs.a[7] = 0x3000;

    raw_write32(0x1000 + 2 * 4, 0x2000);
    paddr pe{0x3f000000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(regs.pc == 0x2000);
    }
}

BOOST_DATA_TEST_CASE(ttr_ok, BIT, s) {
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x3f;
    regs.ITTR[0].address_mask = 0x1f;
    regs.ITTR[0].S = 2;
    regs.ITTR[0].W = false;
    regs.ITTR[1].E = false;
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0x1000;
    raw_write32(0x1000 + 2 * 4, 0x2000);
    paddr pe{0x3f000000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_TEST(ret == 0x20000000);
    } else {
        BOOST_FAIL("exception occuered.");
    }
}

BOOST_DATA_TEST_CASE(atc_match_l, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.ITTR[1].E = false;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    atc_lcache[(s << 31) | 0x2000] =
        atc_entry_t{1, true, false, false, 0, s, 0};
    raw_write32(0x1004, 0);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2000200, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_TEST(ret == (1 << 12 | 0x200));
    } else {
        BOOST_FAIL("exception occuered.");
    }
}

BOOST_DATA_TEST_CASE(atc_match_g, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.ITTR[1].E = false;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    atc_gcache[(s << 31) | 0x2000] =
        atc_entry_t{1, true, false, false, 0, s, 0};
    raw_write32(0x1004, 0);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2000200, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_TEST(ret == (1 << 12 | 0x200));
    } else {
        BOOST_FAIL("exception occuered.");
    }
}

BOOST_DATA_TEST_CASE(root_error, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.ITTR[1].E = false;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2000000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_DATA_TEST_CASE(lv2_error, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2080000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_DATA_TEST_CASE(lv3_error, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_DATA_TEST_CASE(lv3_indirect_error, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x4002);
    raw_write32(0x4000, 0x0);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_DATA_TEST_CASE(lv3_indirect_nest_error, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x4002);
    raw_write32(0x4000, 0x5002);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_AUTO_TEST_CASE(lv3_priv_error) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    regs.urp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5081);
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, TM::USER_CODE, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(raw_read32(0x300C) == 0x5089);
        BOOST_TEST(regs.pc == 0x4000);
    }
}


BOOST_DATA_TEST_CASE(root_write_protect, BIT, s) {
    regs.tcr_e = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2006);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5001);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200E);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(raw_read32(0x300C) == 0x5009);
        BOOST_TEST(regs.pc == 0x4000);
    }
}


BOOST_DATA_TEST_CASE(lv2_write_protect, BIT, s) {
    regs.tcr_e = true;
    regs.tcr_p = false;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3006);
    raw_write32(0x300C, 0x5001);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300E);
        BOOST_TEST(raw_read32(0x300C) == 0x5009);
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_DATA_TEST_CASE(lv3_write_protect, BIT, s) {
    regs.tcr_e = true;
    regs.tcr_p = false;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5007);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083000, 0, SZ::WORD, TT::NORMAL, tm, true};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_FAIL("exception didn't occuer.");
    } else {
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(raw_read32(0x300C) == 0x500F);
        BOOST_TEST(regs.pc == 0x4000);
    }
}

BOOST_DATA_TEST_CASE(ok, BIT * BIT, s, w) {
    regs.tcr_e = true;
    regs.tcr_p = false;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2083022, 0, SZ::WORD, TT::NORMAL, tm, w};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(raw_read32(0x300C) == (0x500B | w << 4));
        BOOST_TEST(ret == 0x5022);
     } else {
        BOOST_FAIL("exception occuered.");
    }
}

BOOST_DATA_TEST_CASE(p_ok, BIT * BIT, s, w) {
    regs.tcr_e = true;
    regs.tcr_p = true;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].address_base = 0xff;
    regs.DTTR[0].address_mask = 0xff;
    regs.isp = 0xff001000;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x6003);
    TM tm = s ? TM::SUPER_CODE : TM::USER_CODE;
    regs.vbr = 0xff003000;
    raw_write32(0x3000 + 2 * 4, 0x4000);
    paddr pe{0x2086022, 0, SZ::WORD, TT::NORMAL, tm, w};
    if(setjmp(ex_jmp) == 0) {
        uint32_t ret = do_mmu(pe);
        BOOST_TEST(raw_read32(0x1004) == 0x200A);
        BOOST_TEST(raw_read32(0x2008) == 0x300A);
        BOOST_TEST(raw_read32(0x300C) == (0x600B | w << 4));
        BOOST_TEST(ret == 0x6022);
     } else {
        BOOST_FAIL("exception occuered.");
    }
}

BOOST_AUTO_TEST_SUITE_END()

// BOOST_AUTO_TEST_SUITE(data)

// BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()