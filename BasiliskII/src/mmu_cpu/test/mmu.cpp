#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
presult test_TTR(const uint32_t addr, bool is_code, bool rw);
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s);
presult ptest(const uint32_t addr, bool rw, bool s);
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
    regs.S = s;
    regs.tcr_e = false;
    regs.ITTR[0].E = false;
    regs.ITTR[1].E = false;
    uint32_t ret = do_mmu(0x1000, true, false, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_invalid, BIT, s) {
    regs.S = s;
    regs.tcr_e = false;
    regs.DTTR[0].E = true;
    regs.DTTR[1].E = true;
    uint32_t ret = do_mmu(0x1000, true, false, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_addr_mismatch, BIT, s) {
    regs.S = s;
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x2f;
    regs.ITTR[1].E = false;
    uint32_t ret = do_mmu(0x1000, true, false, s);
    BOOST_TEST(ret == 0x1000);
}

BOOST_DATA_TEST_CASE(ttr_mode_mismatch, BIT, s) {
    regs.S = s;
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x3f;
    regs.ITTR[0].address_mask = 0x1f;
    regs.ITTR[0].S = s ? 0 : 1;
    regs.ITTR[1].E = false;
    uint32_t ret = do_mmu(0x3f000000, true, false, s);
    BOOST_TEST(ret == 0x3f000000);
}
BOOST_DATA_TEST_CASE(ttr_write_protected, BIT, s) {
    regs.S = s;
    regs.tcr_e = false;
    regs.ITTR[0].E = true;
    regs.ITTR[0].address_base = 0x3f;
    regs.ITTR[0].address_mask = 0x1f;
    regs.ITTR[0].S = s ? 1 : 0;
    regs.ITTR[0].W = true;

    regs.ITTR[1].E = false;
    auto ret = test_TTR(0x3f000000, true, true);
    BOOST_TEST((ret.W == true));
}

BOOST_DATA_TEST_CASE(atc_match_l, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2000200, false, s);
    BOOST_TEST(!ret.G);
    BOOST_TEST((ret.addr + 0) == 1);
}

BOOST_DATA_TEST_CASE(atc_match_g, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    raw_write32(0x3000 + 2 * 4, 0x4000);
    auto ret = ptest(0x2000200, true, s);
    BOOST_TEST((ret.G == true));
    BOOST_TEST((ret.addr + 0) == 1);
}

BOOST_DATA_TEST_CASE(root_error, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0);
    auto ret = ptest(0x2000000, true, s);
    BOOST_TEST(!ret.R);
}

BOOST_DATA_TEST_CASE(lv2_error, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
    atc_gcache.clear();
    atc_lcache.clear();
    if(s) {
        regs.srp = 0x1000;
    } else {
        regs.urp = 0x1000;
    }
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0);
    auto ret = ptest(0x2080000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
}

BOOST_DATA_TEST_CASE(lv3_error, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_indirect_error, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_DATA_TEST_CASE(lv3_indirect_nest_error, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083000, true, s);
    BOOST_TEST(!ret.R);
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
}

BOOST_AUTO_TEST_CASE(lv3_priv_error) {
    regs.S = false;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
    atc_gcache.clear();
    atc_lcache.clear();
    regs.urp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5081);
    auto ret = ptest(0x2083000, true, false);
    BOOST_TEST((ret.S == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x5089);
}

BOOST_DATA_TEST_CASE(root_write_protect, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200E);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x5009);
}

BOOST_DATA_TEST_CASE(lv2_write_protect, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300E);
    BOOST_TEST(raw_read32(0x300C) == 0x5009);
}

BOOST_DATA_TEST_CASE(lv3_write_protect, BIT, s) {
    regs.S = s;
    regs.tcr_e = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083000, true, s);
    BOOST_TEST((ret.W == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == 0x500F);
}

BOOST_DATA_TEST_CASE(ok, BIT *BIT, s, w) {
    regs.S = s;
    regs.tcr_e = true;
    regs.tcr_p = false;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2083022, w, s);
    BOOST_TEST((ret.R == true));
    BOOST_TEST(raw_read32(0x1004) == 0x200A);
    BOOST_TEST(raw_read32(0x2008) == 0x300A);
    BOOST_TEST(raw_read32(0x300C) == (0x500B | w << 4));
    BOOST_TEST((ret.addr + 0) == 0x5);
}

BOOST_DATA_TEST_CASE(p_ok, BIT *BIT, s, w) {
    regs.S = s;
    regs.tcr_e = true;
    regs.tcr_p = true;
    regs.DTTR[0].E = false;
    regs.ITTR[0].E = false;
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
    auto ret = ptest(0x2086022, w, s);
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