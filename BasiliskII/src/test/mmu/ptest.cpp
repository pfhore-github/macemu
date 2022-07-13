#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
uint32_t do_op_movec_from(int o);
BOOST_FIXTURE_TEST_SUITE(PTEST, InitFix)
static const decltype(&mmu_d) mmu_p[] = {&mmu_d, &mmu_i};
BOOST_AUTO_TEST_SUITE(PTESTR)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0172550);
    exception_check(8);
}
BOOST_DATA_TEST_CASE(bus_error, BIT, s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    mmu_d.TTR[0].E = false;
    mmu_d.TTR[1].E = false;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x6000000;
    regs.a[2] = 0x2000000;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST(do_op_movec_from(0x805) == 1 << 11);
}

BOOST_DATA_TEST_CASE(ttr, BIT, s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    mmu_d.TTR[0].E = true;
    mmu_d.TTR[0].address_base = 0x1;
    mmu_d.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x6000000;
    regs.a[2] = 0x01000000;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST(do_op_movec_from(0x805) == 3);
}

BOOST_DATA_TEST_CASE(unexist, BIT, s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0);
    regs.a[2] = 0x2000000;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST(do_op_movec_from(0x805) == 0);
}

BOOST_DATA_TEST_CASE(PADDR, BIT, s) {
    regs.S = true;

    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003);
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) & 0xfffff000)== 0x5000);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(G, BIT*BIT, s,g) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (g << 10));
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) >> 10 & 1) == g);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(Ux, BIT*bdata::xrange(4), s,u) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (u << 8));
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) >> 8 & 3) == u);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(S, BIT*BIT, s,p_s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (p_s << 7));
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) >> 7 & 1) == p_s);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(CM, BIT*bdata::xrange(4), s,cm) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (cm << 5));
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) >> 5 & 3) == cm);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(M, BIT*BIT, s,m) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (m << 4));
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) >> 4 & 1) == m);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(W_1, BIT, s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002 | 1 << 2);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 );
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST(do_op_movec_from(0x805) & 1 << 2);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}


BOOST_DATA_TEST_CASE(W_2, BIT, s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002 );
    raw_write32(0x2008, 0x3002| 1 << 2);
    raw_write32(0x300C, 0x5003 );
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST(do_op_movec_from(0x805) & 1 << 2);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_DATA_TEST_CASE(W_3, BIT, s) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002 );
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | 1 << 2);
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172552);
    exception_check(0);
    BOOST_TEST(do_op_movec_from(0x805) & 1 << 2);
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PTESTW)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0172510);
    exception_check(8);
}

BOOST_DATA_TEST_CASE(M, BIT*BIT, s,m) {
    regs.S = true;
    mmu.tcr_e = true;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    regs.dfc = s << 2 | 1;
    mmu.pflush();
    mmu.urp = mmu.srp = 0x1000;
    raw_write32(0x1004, 0x2002);
    raw_write32(0x2008, 0x3002);
    raw_write32(0x300C, 0x5003 | (m << 4));
    regs.a[2] = 0x2083022;
    raw_write16(0, 0172512);
    exception_check(0);
    BOOST_TEST((do_op_movec_from(0x805) >> 4 & 1));
    BOOST_TEST(do_op_movec_from(0x805) & 1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()