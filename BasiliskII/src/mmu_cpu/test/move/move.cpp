#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVE, InitFix)
BOOST_DATA_TEST_CASE(opc, EA_D() * EA_D_R(), ea_dst, ea_src) {
    int dn = ea_dst & 7;
    int type = (ea_dst >> 3) & 7;
    BOOST_TEST(opc_map[0010000 | type << 6 | dn << 9 | ea_src] ==
               opc_map[0010000]);
    BOOST_TEST(opc_map[0020000 | type << 6 | dn << 9 | ea_src] ==
               opc_map[0020000]);
    BOOST_TEST(opc_map[0030000 | type << 6 | dn << 9 | ea_src] ==
               opc_map[0030000]);
}

BOOST_DATA_TEST_CASE(opc2, EA_D(), ea) {
    BOOST_TEST(opc_map[0040300 | ea] == opc_map[0040300]);
    BOOST_TEST(opc_map[0041300 | ea] == opc_map[0041300]);
    BOOST_TEST(opc_map[0042300 | ea] == opc_map[0042300]);
    BOOST_TEST(opc_map[0043300 | ea] == opc_map[0043300]);
}

BOOST_DATA_TEST_CASE(opc3, REG(), an) {
    BOOST_TEST(opc_map[0047140 | an] == opc_map[0047140]);
    BOOST_TEST(opc_map[0047150 | an] == opc_map[0047150]);
}


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(n, bdata::xrange(1), value) {
    regs.d[1] = value << 7;
    raw_write16(0, 0012001);
    m68k_do_execute();
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(1), value) {
    regs.d[1] = !value;
    raw_write16(0, 0012001);
    m68k_do_execute();
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x30;
    regs.d[2] = 0x12345678;
    raw_write16(0, 0012001);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12345630);
}

BOOST_AUTO_TEST_CASE(address) {
    regs.d[1] = 0xff;
    regs.a[2] = 0x10;
    raw_write16(0, 0012201);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0xff);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0xff;
    regs.a[2] = 0x10;
    raw_write16(0, 0012301);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0xff);
    BOOST_TEST(regs.a[2] == 0x11);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0xff;
    regs.a[2] = 0x11;
    raw_write16(0, 0012401);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0xff);
    BOOST_TEST(regs.a[2] == 0x10);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)
BOOST_AUTO_TEST_CASE(address) {
    regs.a[2] = 0x10;
    raw_write8(0x10, 0xff);
    raw_write16(0, 0011022);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(inc) {
    raw_write8(0x10, 0xff);
    regs.a[2] = 0x10;
    raw_write16(0, 0011032);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x11);
    BOOST_TEST(regs.d[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write8(0x10, 0xff);
    regs.a[2] = 0x11;
    raw_write16(0, 0011042);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(imm) {
    raw_write16(0, 0011074);
    raw_write16(2, 200);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 200);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(n, bdata::xrange(1), value) {
    regs.d[1] = value << 15;
    raw_write16(0, 0032001);
    m68k_do_execute();
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(1), value) {
    regs.d[1] = !value;
    raw_write16(0, 0032001);
    m68k_do_execute();
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x3040;
    regs.d[2] = 0x12345678;
    raw_write16(0, 0032001);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12343040);
}

BOOST_AUTO_TEST_CASE(address) {
    regs.d[1] = 0xffff;
    regs.a[2] = 0x10;
    raw_write16(0, 0032201);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xffff);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0xffff;
    regs.a[2] = 0x10;
    raw_write16(0, 0032301);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xffff);
    BOOST_TEST(regs.a[2] == 0x12);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0xffff;
    regs.a[2] = 0x12;
    raw_write16(0, 0032401);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xffff);
    BOOST_TEST(regs.a[2] == 0x10);
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
BOOST_AUTO_TEST_CASE(address) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0xffff);
    raw_write16(0, 0031022);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_CASE(inc) {
    raw_write16(0x10, 0xffff);
    regs.a[2] = 0x10;
    raw_write16(0, 0031032);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x12);
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write16(0x10, 0xffff);
    regs.a[2] = 0x12;
    raw_write16(0, 0031042);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_CASE(imm) {
    raw_write16(0, 0031074);
    raw_write16(2, 2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 2000);
}

BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    regs.x = x1;
    regs.v = v1;
    regs.c = c1;
    regs.n = n1;
    regs.z = z1;
    raw_write16(0, 0041301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1);
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

BOOST_DATA_TEST_CASE(n, bdata::xrange(1), value) {
    regs.d[1] = value << 31;
    raw_write16(0, 0022001);
    m68k_do_execute();
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(1), value) {
    regs.d[1] = !value;
    raw_write16(0, 0022001);
    m68k_do_execute();
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0022001);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(address) {
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x10;
    raw_write16(0, 0022201);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xffffffff);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x10;
    raw_write16(0, 0022301);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xffffffff);
    BOOST_TEST(regs.a[2] == 0x14);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x14;
    raw_write16(0, 0022401);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xffffffff);
    BOOST_TEST(regs.a[2] == 0x10);
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
BOOST_AUTO_TEST_CASE(address) {
    regs.a[2] = 0x10;
    raw_write32(0x10, 0xffffffff);
    raw_write16(0, 0021022);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffffff);
}

BOOST_AUTO_TEST_CASE(inc) {
    raw_write32(0x10, 0xffffffff);
    regs.a[2] = 0x10;
    raw_write16(0, 0021032);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x14);
    BOOST_TEST(regs.d[1] == 0xffffffff);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write32(0x10, 0xffffffff);
    regs.a[2] = 0x14;
    raw_write16(0, 0021042);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[1] == 0xffffffff);
}

BOOST_AUTO_TEST_CASE(imm) {
    raw_write16(0, 0021074);
    raw_write32(2, 0x10000);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000);
}

BOOST_AUTO_TEST_CASE(usp) {
    regs.usp = 0x1234;
    regs.S = true;
    asm_m68k("move.l %USP, %A2");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x1234);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(MOVEQ, InitFix)
BOOST_AUTO_TEST_CASE(n) {
    asm_m68k("moveq #-4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == -4);
}

BOOST_AUTO_TEST_CASE(z) {
    asm_m68k("moveq #0, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()