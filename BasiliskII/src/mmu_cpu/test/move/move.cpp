#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVE, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    raw_write16(0, 0012001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0012001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    auto [src, dst] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v8();
    regs.d[src] = v2;
    regs.d[dst] = v1;
    raw_write16(0, 0010000 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.d[dst] == ((v1 & 0xffffff00) | v2));
}

BOOST_AUTO_TEST_CASE(inc) {
    auto src = rand_reg();
    auto dst = rand_ar();
    auto v1 = get_v8();
    regs.d[src] = v1;
    regs.a[dst] = 0x10;
    raw_write16(0, 0010300 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == v1);
    BOOST_TEST(regs.a[dst] == 0x11);
}

BOOST_AUTO_TEST_CASE(dec) {
    auto src = rand_reg();
    auto dst = rand_ar();
    auto v1 = get_v8();
    regs.d[src] = v1;
    regs.a[dst] = 0x11;
    raw_write16(0, 0010400 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == v1);
    BOOST_TEST(regs.a[dst] == 0x10);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)

BOOST_AUTO_TEST_CASE(inc) {
    auto dst = rand_reg();
    auto src = rand_ar();
    auto v1 = get_v8();
    raw_write8(0x10, v1);
    regs.a[src] = 0x10;
    raw_write16(0, 0010030 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[src] == 0x11);
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_CASE(dec) {
    auto dst = rand_reg();
    auto src = rand_ar();
    auto v1 = get_v8();
    raw_write8(0x10, v1);
    regs.a[src] = 0x11;
    raw_write16(0, 0010040 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[src] == 0x10);
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_CASE(imm) {
    auto dst = rand_reg();
    auto v1 = get_v8();
    raw_write16(0, 0010074 | dst << 9);
    raw_write16(2, v1);
    m68k_do_execute();
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0032001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0032001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    auto [src, dst] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v16();
    regs.d[src] = v2;
    regs.d[dst] = v1;
    raw_write16(0, 0030000 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.d[dst] == ((v1 & 0xffff0000) | v2));
}

BOOST_AUTO_TEST_CASE(inc) {
    auto src = rand_reg();
    auto dst = rand_ar();
    auto v1 = get_v16();
    regs.d[src] = v1;
    regs.a[dst] = 0x10;
    raw_write16(0, 0030300 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == v1);
    BOOST_TEST(regs.a[dst] == 0x12);
}

BOOST_AUTO_TEST_CASE(dec) {
    auto src = rand_reg();
    auto dst = rand_ar();
    auto v1 = get_v16();
    regs.d[src] = v1;
    regs.a[dst] = 0x12;
    raw_write16(0, 0030400 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == v1);
    BOOST_TEST(regs.a[dst] == 0x10);
}

BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    regs.x = regs.v = regs.c = regs.n = regs.z = false;
    regs.d[1] = x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1;
    raw_write16(0, 0042301);
    m68k_do_execute();
    BOOST_TEST(regs.x == x1);
    BOOST_TEST(regs.n == n1);
    BOOST_TEST(regs.z == z1);
    BOOST_TEST(regs.v == v1);
    BOOST_TEST(regs.c == c1);
}

BOOST_AUTO_TEST_CASE(sr_invalid) {
    regs.S = false;
    raw_write16(0, 0043301);
    exception_check(8);
}

BOOST_DATA_TEST_CASE(sr_valid, bdata::xrange(8), i) {
    regs.S = true;
    regs.M = false;
    regs.d[1] = 0x3000;
    raw_write16(0, 0043301);
    m68k_do_execute();
    BOOST_TEST(regs.M);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)

BOOST_AUTO_TEST_CASE(inc) {
    auto dst = rand_reg();
    auto src = rand_ar();
    auto v1 = get_v16();
    raw_write16(0x10, v1);
    regs.a[src] = 0x10;
    raw_write16(0, 0030030 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[src] == 0x12);
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_CASE(dec) {
    auto dst = rand_reg();
    auto src = rand_ar();
    auto v1 = get_v16();
    raw_write16(0x10, v1);
    regs.a[src] = 0x12;
    raw_write16(0, 0030040 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[src] == 0x10);
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_CASE(imm) {
    auto dst = rand_reg();
    auto v1 = get_v16();
    raw_write16(0, 0030074 | dst << 9);
    raw_write16(2, v1);
    m68k_do_execute();
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    regs.x = x1;
    regs.v = v1;
    regs.c = c1;
    regs.n = n1;
    regs.z = z1;
    raw_write16(0, 0041301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1));
}

BOOST_AUTO_TEST_CASE(sr_invalid) {
    regs.S = false;
    raw_write16(0, 0040301);
    exception_check(8);
}

BOOST_DATA_TEST_CASE(sr_valid, bdata::xrange(8), i) {
    regs.S = true;
    regs.M = true;
    raw_write16(0, 0040301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] & 0x3000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0022001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0022001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    auto [src, dst] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[src] = v2;
    regs.d[dst] = v1;
    raw_write16(0, 0020000 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.d[dst] == v2);
}

BOOST_AUTO_TEST_CASE(inc) {
    auto src = rand_reg();
    auto dst = rand_ar();
    auto v1 = get_v16();
    regs.d[src] = v1;
    regs.a[dst] = 0x10;
    raw_write16(0, 0020300 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == v1);
    BOOST_TEST(regs.a[dst] == 0x14);
}

BOOST_AUTO_TEST_CASE(dec) {
    auto src = rand_reg();
    auto dst = rand_ar();
    auto v1 = get_v32();
    regs.d[src] = v1;
    regs.a[dst] = 0x14;
    raw_write16(0, 0020400 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == v1);
    BOOST_TEST(regs.a[dst] == 0x10);
}

BOOST_AUTO_TEST_CASE(usp_invalid) {
    regs.S = false;
    raw_write16(0, 0047140);
    exception_check(8);
}

BOOST_AUTO_TEST_CASE(usp) {
    regs.a[2] = 0x14;
    regs.S = true;
    raw_write16(0, 0047142);
    m68k_do_execute();
    BOOST_TEST(regs.usp == 0x14);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)
BOOST_AUTO_TEST_CASE(inc) {
    auto dst = rand_reg();
    auto src = rand_ar();
    auto v1 = get_v32();
    raw_write32(0x10, v1);
    regs.a[src] = 0x10;
    raw_write16(0, 0020030 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[src] == 0x14);
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_CASE(dec) {
    auto dst = rand_reg();
    auto src = rand_ar();
    auto v1 = get_v32();
    raw_write32(0x10, v1);
    regs.a[src] = 0x14;
    raw_write16(0, 0020040 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[src] == 0x10);
    BOOST_TEST(regs.d[dst] == v1);
}

BOOST_AUTO_TEST_CASE(imm) {
    auto dst = rand_reg();
    auto v1 = get_v32();
    raw_write16(0, 0020074 | dst << 9);
    raw_write32(2, v1);
    m68k_do_execute();
    BOOST_TEST(regs.d[dst] == v1);
}
BOOST_AUTO_TEST_CASE(usp) {
    regs.usp = 0x1234;
    regs.S = true;
    raw_write16(0, 0047152);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x1234);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
