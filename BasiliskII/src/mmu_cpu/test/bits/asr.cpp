#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ASR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto xr = rand_reg();
    auto v1 = get_v8();
    regs.d[xr] = v1;
    write16(0, 0160000 | n << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((int8_t(v1) >> (n ? n : 8) & 0xff)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v8();
    int n = get_vn(1, 7);
    regs.d[xr] = v1;
    regs.d[yr] = n;
    write16(0, 0160040 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((int8_t(v1) >> n) & 0xff));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0160040 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161001);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xff;
    regs.d[1] = 0;
    write16(0, 0161042);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfe;
    write16(0, 0161001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto xr = rand_reg();
    auto v1 = get_v16();
    regs.d[xr] = v1;
    write16(0, 0160100 | n << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((int16_t(v1) >> (n ? n : 8) & 0xffff)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v16();
    int n = get_vn(1, 15);
    regs.d[xr] = v1;
    regs.d[yr] = n;
    write16(0, 0160140 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((int16_t(v1) >> n) & 0xffff));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0160140 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161101);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xffff;
    regs.d[1] = 0;
    write16(0, 0161142);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffe;
    write16(0, 0161101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, -16);
    raw_write16(0, 0160322);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = static_cast<uint16_t>(-8));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto xr = rand_reg();
    auto v1 = get_v32();
    regs.d[xr] = v1;
    write16(0, 0160200 | n << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == (int32_t(v1) >> (n ? n : 8)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v32();
    int n = get_vn(1, 31);
    regs.d[xr] = v1;
    regs.d[yr] = n;
    write16(0, 0160240 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == (int32_t(v1) >> n));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0160240 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161201);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xffffffff;
    regs.d[1] = 0;
    write16(0, 0161242);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffffffe;
    write16(0, 0161201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161201);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()