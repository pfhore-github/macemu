#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(LEA, InitFix)

BOOST_AUTO_TEST_CASE(ill_an) {
    raw_write16(0, 0043712);
    exception_check(4);
}

BOOST_AUTO_TEST_CASE(an) {
    regs.a[2] = 0x1000;
    raw_write16(0, 0043722);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(ill_incr) {
    raw_write16(0, 0043732);
    exception_check(4);
}

BOOST_AUTO_TEST_CASE(ill_decr) {
    raw_write16(0, 0043742);
    exception_check(4);
}


BOOST_AUTO_TEST_CASE(d16) {
    regs.a[2] = 0x1000;
    raw_write16(0, 0043752);
    raw_write16(2, -100);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000 - 100);
}
BOOST_AUTO_TEST_CASE(xn_w) {
    regs.d[3] = (-6) & 0xffff;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | (-16 & 0xff));
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000 - 6 - 16);
}

BOOST_AUTO_TEST_CASE(xn_l) {
    regs.d[3] = 10;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000 + 10 + 4);
}

BOOST_DATA_TEST_CASE(xn_cc, bdata::xrange(4), c) {
    regs.d[3] = 8;
    regs.a[2] = 0x1000;
    raw_write16(0, 0044762);
    raw_write16(2, 3 << 12 | 1 << 11 | c << 9 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[4] == 0x1004 + (8 << c));
}

BOOST_AUTO_TEST_CASE(bd_err) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 0 << 4);
    exception_check(4);
}

BOOST_AUTO_TEST_CASE(bd_z) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 1 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000 + 4);
}

BOOST_AUTO_TEST_CASE(bd_w) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    raw_write16(4, -60);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000 + 4 - 60);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 3 << 4);
    raw_write32(4, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1000 + 4 + 1000);
}

#if 0





BOOST_DATA_TEST_CASE(ai, BIT, ai) {
    auto [ax, an] = rand_ar2();
    auto dn = rand_reg();
    uint32_t addr = get_v32();
    int32_t di = get_v32();

    regs.d[dn] = di;
    regs.a[ax] = addr;
    raw_write16(0, 0040760 | an << 9 | ax);
    raw_write16(2, dn << 12 | 1 << 11 | 1 << 8 | ai << 7 | 1 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == (ai ? 0 : addr) + di);
}

BOOST_DATA_TEST_CASE(ri, BIT, ri) {
    auto [ax, an] = rand_ar2();
    auto dn = rand_reg();
    uint32_t addr = get_v32();
    int32_t di = get_v32();
    regs.d[dn] = di;
    regs.a[ax] = addr;
    raw_write16(0, 0040760 | an << 9 | ax);
    raw_write16(2, dn << 12 | 1 << 11 | 1 << 8 | ri << 6 | 1 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == addr + (ri ? 0 : di));
}

BOOST_DATA_TEST_CASE(post_od, bdata::xrange(1, 4), od_sz) {
    auto [ax, an] = rand_ar2();
    auto dn = rand_reg();
    uint32_t addr = 0x1000;
    uint8_t di = get_v8();
    regs.d[dn] = di;
    regs.a[ax] = addr;
    raw_write16(0, 0040760 | an << 9 | ax);
    raw_write16(2, dn << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | od_sz);
    raw_write16(4, 0x100);
    int32_t od = 0;
    switch(od_sz) {
    case 1:
        od = 0;
        break;
    case 2:
        od = int16_t(get_v16());
        raw_write16(6, od);
        break;
    case 3:
        od = get_v32();
        raw_write32(6, od);
        break;
    }
    raw_write32(0x1000 + di * 4 + 0x100, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 0x2000 + od);
}

BOOST_DATA_TEST_CASE(pre_od, bdata::xrange(1, 4), od_sz) {
    auto [ax, an] = rand_ar2();
    auto dn = rand_reg();
    uint32_t addr = 0x1000;
    uint8_t di = get_v8();
    regs.d[dn] = di;
    regs.a[ax] = addr;
    raw_write16(0, 0040760 | an << 9 | ax);
    raw_write16(2, dn << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 1 << 2 | od_sz);
    raw_write16(4, 0x100);
    int32_t od = 0;
    switch(od_sz) {
    case 1:
        od = 0;
        break;
    case 2:
        od = int16_t(get_v16());
        raw_write16(6, od);
        break;
    case 3:
        od = get_v32();
        raw_write32(6, od);
        break;
    }
    raw_write32(0x1000 + 0x100, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 0x2000 + di * 4 + od);
}

BOOST_AUTO_TEST_CASE(imm_w) {
    auto an = rand_ar();
    uint16_t addr = get_v16();
    raw_write16(0, 0040770 | an << 9 );
    raw_write16(2, addr);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == addr);
}

BOOST_AUTO_TEST_CASE(imm_l) {
    auto an = rand_ar();
    uint32_t addr = get_v32();
    raw_write16(0, 0040771 | an << 9 );
    raw_write32(2, addr);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == addr);
}

BOOST_AUTO_TEST_CASE(pc_d16) {
    auto an = rand_ar();
    int16_t off = get_v16();
    raw_write16(0, 0040772 | an << 9 );
    raw_write16(2, off);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 2 + off);
}

BOOST_AUTO_TEST_CASE(pc_brief) {
    auto an = rand_ar();
    auto dn = rand_reg();
    int32_t di = get_v32();
    int8_t off = get_v8();
    regs.d[dn] = di;
    raw_write16(0, 0040773 | an << 9);
    raw_write16(2, dn << 12 | 1 << 11 | (off & 0xff));
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == (2 + off + di));
}

BOOST_AUTO_TEST_CASE(pc_extend) {
    auto an = rand_ar();
    auto dn = rand_reg();
    int32_t di = get_v32();
    uint16_t bd = get_v16() & 0x7fff;
    raw_write16(4, bd);
    regs.d[dn] = di;
    raw_write16(0, 0040773 | an << 9 );
    raw_write16(2, dn << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 2 + di + bd);
}

BOOST_AUTO_TEST_CASE(pc_post) {
    auto an = rand_ar();
    auto dn = rand_reg();
    uint8_t di = get_v8();
    regs.d[dn] = di;
    raw_write16(0, 0040773 | an << 9);
    raw_write16(2, dn << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 2);
    raw_write16(4, 0x100);
    int16_t od = get_v16();
    raw_write16(6, od);
    raw_write32(2 + di * 4 + 0x100, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 0x2000 + od);
}

BOOST_AUTO_TEST_CASE(pc_pre) {
    auto an = rand_ar();
    auto dn = rand_reg();
    uint8_t di = get_v8();
    regs.d[dn] = di;
    raw_write16(0, 0040773 | an << 9 );
    raw_write16(2, dn << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 1 << 2 | 2);
    raw_write16(4, 0x100);
    int16_t od = get_v16();
    raw_write16(6, od);
    raw_write32(2 + 0x100, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.a[an] == 0x2000 + di * 4 + od);
}
#endif
BOOST_AUTO_TEST_SUITE_END()