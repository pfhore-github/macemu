#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include "mmu/mmu_68040.h"
BOOST_FIXTURE_TEST_SUITE(MOVEC, InitFix)
BOOST_AUTO_TEST_SUITE(from)

BOOST_AUTO_TEST_CASE(priv_error) {
    regs.S = false;
    raw_write16(0, 0047172);
    exception_check(8);
}
BOOST_DATA_TEST_CASE(sfc, REG(), r) {
    regs.S = true;
    regs.sfc = 1;
    raw_write16(0, 0047172);
    raw_write16(2, r << 12 | 0x000);
    m68k_do_execute();
    BOOST_TEST(regs.d[r] == 1);
}

BOOST_AUTO_TEST_CASE(dfc) {
    regs.S = true;
    regs.dfc = 2;
    raw_write16(0, 0047172);
    raw_write16(2, 0x1001);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 2);
}

BOOST_AUTO_TEST_CASE(usp) {
    regs.S = true;
    regs.usp = 0x1000;
    raw_write16(0, 0047172);
    raw_write16(2, 0x2800);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x1000);
}

BOOST_AUTO_TEST_CASE(vbr) {
    regs.S = true;
    regs.vbr = 0x2000;
    raw_write16(0, 0047172);
    raw_write16(2, 0x3801);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x2000);
}

BOOST_AUTO_TEST_CASE(cacr) {
    regs.S = true;
    regs.cacr_de = true;
    regs.cacr_ie = true;
    raw_write16(0, 0047172);
    raw_write16(2, 0x4002);
    m68k_do_execute();
    BOOST_TEST(regs.d[4] == 0x80008000);
}

BOOST_AUTO_TEST_CASE(msp) {
    regs.S = true;
    regs.msp = 0x2000;
    raw_write16(0, 0047172);
    raw_write16(2, 0x5803);
    m68k_do_execute();
    BOOST_TEST(regs.d[5] == 0x2000);
}

BOOST_AUTO_TEST_CASE(isp) {
    regs.S = true;
    regs.isp = 0x3000;
    raw_write16(0, 0047172);
    raw_write16(2, 0x6804);
    m68k_do_execute();
    BOOST_TEST(regs.d[6] == 0x3000);
}

BOOST_AUTO_TEST_CASE(tc) {
    regs.S = true;
    mmu.tcr_p = true;
    raw_write16(0, 0047172);
    raw_write16(2, 0x7003);
    m68k_do_execute();
    BOOST_TEST(regs.d[7] == 0x4000);
}

BOOST_DATA_TEST_CASE(ittr, BIT, i) {
    regs.S = true;
    mmu_i.TTR[i].address_base = 0x20;
    mmu_i.TTR[i].address_mask = 0x30;
    mmu_i.TTR[i].S = true;
    raw_write16(0, 0047172);
    raw_write16(2, 0x0004 + i);
    m68k_do_execute();
    BOOST_TEST(regs.d[0] == 0x20302000);
}

BOOST_DATA_TEST_CASE(dttr, BIT, i) {
    regs.S = true;
    mmu_d.TTR[i].address_base = 0x20;
    mmu_d.TTR[i].address_mask = 0x30;
    mmu_d.TTR[i].S = true;
    raw_write16(0, 0047172);
    raw_write16(2, 0x1006 + i);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x20302000);
}

BOOST_AUTO_TEST_CASE(mmusr) {
    regs.S = true;
    mmu.MMUSR.PA = 0x64000000;
    mmu.MMUSR.G = true;
    mmu.MMUSR.S = true;
    mmu.MMUSR.R = true;
    raw_write16(0, 0047172);
    raw_write16(2, 0x2805);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x64000481);
}

BOOST_AUTO_TEST_CASE(urp) {
    regs.S = true;
    mmu.urp = 0x5000;
    raw_write16(0, 0047172);
    raw_write16(2, 0x3806);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x5000);
}

BOOST_AUTO_TEST_CASE(srp) {
    regs.S = true;
    mmu.srp = 0x6000;
    raw_write16(0, 0047172);
    raw_write16(2, 0x4807);
    m68k_do_execute();
    BOOST_TEST(regs.d[4] == 0x6000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(priv_error) {
    regs.S = false;
    raw_write16(0, 0047173);
    exception_check(8);
}
BOOST_DATA_TEST_CASE(sfc, REG(), r) {
    regs.S = true;
    regs.d[r] = 1;
    raw_write16(0, 0047173);
    raw_write16(2, r << 12 | 0x000);
    m68k_do_execute();
    BOOST_TEST(regs.sfc == 1);
}

BOOST_AUTO_TEST_CASE(dfc) {
    regs.S = true;
    regs.d[1] = 2;
    raw_write16(0, 0047173);
    raw_write16(2, 0x1001);
    m68k_do_execute();
    BOOST_TEST(regs.dfc == 2);
}

BOOST_AUTO_TEST_CASE(usp) {
    regs.S = true;
    regs.d[2] = 0x1000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x2800);
    m68k_do_execute();
    BOOST_TEST(regs.usp == 0x1000);
}

BOOST_AUTO_TEST_CASE(vbr) {
    regs.S = true;
    regs.d[3] = 0x2000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x3801);
    m68k_do_execute();
    BOOST_TEST(regs.vbr == 0x2000);
}

BOOST_AUTO_TEST_CASE(cacr) {
    regs.S = true;
    regs.d[4] = 0x80008000;
    regs.cacr_de = true;
    regs.cacr_ie = true;
    raw_write16(0, 0047173);
    raw_write16(2, 0x4002);
    m68k_do_execute();
    BOOST_TEST(regs.cacr_de);
    BOOST_TEST(regs.cacr_ie);
}

BOOST_AUTO_TEST_CASE(msp) {
    regs.S = true;
    regs.d[5] = 0x2000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x5803);
    m68k_do_execute();
    BOOST_TEST(regs.msp == 0x2000);
}

BOOST_AUTO_TEST_CASE(isp) {
    regs.S = true;
    regs.d[6] = 0x3000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x6804);
    m68k_do_execute();
    BOOST_TEST(regs.isp == 0x3000);
}

BOOST_AUTO_TEST_CASE(tc) {
    regs.S = true;
    regs.d[7] = 0x4000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x7003);
    m68k_do_execute();
    BOOST_TEST(mmu.tcr_p);
}

BOOST_DATA_TEST_CASE(ittr, BIT, i) {
    regs.S = true;
    regs.d[0] = 0x20302000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x0004 + i);
    m68k_do_execute();
    BOOST_TEST(mmu_i.TTR[i].address_base == 0x20);
    BOOST_TEST(mmu_i.TTR[i].address_mask == 0x30);
    BOOST_TEST(mmu_i.TTR[i].S);
}

BOOST_DATA_TEST_CASE(dttr, BIT, i) {
    regs.S = true;
    regs.d[1] = 0x20302000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x1006 + i);
    m68k_do_execute();
    BOOST_TEST(mmu_d.TTR[i].address_base == 0x20);
    BOOST_TEST(mmu_d.TTR[i].address_mask == 0x30);
    BOOST_TEST(mmu_d.TTR[i].S);
}

BOOST_AUTO_TEST_CASE(mmusr) {
    regs.S = true;
    regs.d[2] = 0x64000481;
    raw_write16(0, 0047173);
    raw_write16(2, 0x2805);
    m68k_do_execute();
    BOOST_TEST( mmu.MMUSR.PA == 0x64000000);
    BOOST_TEST( mmu.MMUSR.G );
    BOOST_TEST( mmu.MMUSR.S );
    BOOST_TEST( mmu.MMUSR.R );
}

BOOST_AUTO_TEST_CASE(urp) {
    regs.S = true;
    regs.d[3] = 0x5000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x3806);
    m68k_do_execute();
    BOOST_TEST(mmu.urp == 0x5000);
}

BOOST_AUTO_TEST_CASE(srp) {
    regs.S = true;
    regs.d[4] = 0x6000;
    raw_write16(0, 0047173);
    raw_write16(2, 0x4807);
    m68k_do_execute();
    BOOST_TEST(mmu.srp == 0x6000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()