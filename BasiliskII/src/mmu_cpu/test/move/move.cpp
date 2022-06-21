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
    regs.d[1] = 0x01234567;
    regs.d[2] = 0x89ABCDEF;
    raw_write16(0, 0012001);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x89ABCD67);
}


BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0x12;
    regs.a[2] = 0x10;
    raw_write16(0, 0012301);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x12);
    BOOST_TEST(regs.a[2] == 0x11);
}

BOOST_AUTO_TEST_CASE(inc_a7) {
    regs.d[1] = 0x12;
    regs.a[7] = 0x10;
    raw_write16(0, 0017301);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x12);
    BOOST_TEST(regs.a[7] == 0x12);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0x12;
    regs.a[2] = 0x11;
    raw_write16(0, 0012401);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x12);
    BOOST_TEST(regs.a[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(dec_a7) {
    regs.d[1] = 0x12;
    regs.a[7] = 0x12;
    raw_write16(0, 0017401);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x12);
    BOOST_TEST(regs.a[7] == 0x10);
}




BOOST_AUTO_TEST_CASE(ext_pc1) {
    regs.d[1] = 0x12;
    raw_write16(0, 0012701);
    raw_write16(2, 0x1000);
     exception_check(4);  
}



BOOST_AUTO_TEST_CASE(ext_pc2) {
    regs.d[1] = 0x12;
    raw_write16(0, 0013701);
    raw_write16(2, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    raw_write16(4, 100);
     exception_check(4);  
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)
BOOST_AUTO_TEST_CASE(inc) {
    raw_write8(0x10, 0x23);
    regs.a[2] = 0x10;
    raw_write16(0, 0013032);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x11);
    BOOST_TEST(regs.d[3] == 0x23);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write8(0x10, 0x23);
    regs.a[2] = 0x11;
    raw_write16(0, 0013042);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[3] == 0x23);
}

BOOST_AUTO_TEST_CASE(imm_addr_w) {
    raw_write16(0, 0012070);
    raw_write16(2, 0x100);
    raw_write8(0x100, 0x12);
     m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12);
}

BOOST_AUTO_TEST_CASE(imm) {
    raw_write16(0, 0012074);
    raw_write16(2, 0x12);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12);
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
    regs.d[2] = 0x01234567;
    regs.d[3] = 0x89ABCDEF;
    raw_write16(0, 0033002);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x89AB4567);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[2] = 0x1234;
    regs.a[3] = 0x10;
    raw_write16(0, 0033302);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0x1234);
    BOOST_TEST(regs.a[3] == 0x12);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[2] = 0x5678;
    regs.a[3] = 0x12;
    raw_write16(0, 0033402);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0x5678);
    BOOST_TEST(regs.a[3] == 0x10);
}

BOOST_AUTO_TEST_CASE(ext_pc1) {
    regs.d[1] = 0x12;
    raw_write16(0, 0032701);
    raw_write16(2, 0x1000);
     exception_check(4);  
}



BOOST_AUTO_TEST_CASE(ext_pc2) {
    regs.d[1] = 0x12;
    raw_write16(0, 0033701);
    raw_write16(2, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    raw_write16(4, 100);
     exception_check(4);  
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
    raw_write16(0x10, 0x1234);
    regs.a[2] = 0x10;
    raw_write16(0, 0033032 );
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x12);
    BOOST_TEST(regs.d[3] == 0x1234);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write16(0x10, 0x1234);
    regs.a[2] = 0x12;
    raw_write16(0, 0033042);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[3] == 0x1234);
}


BOOST_AUTO_TEST_CASE(imm_addr_w) {
    raw_write16(0, 0032070);
    raw_write16(2, 0x100);
    raw_write16(0x100, 0x1234);
     m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x1234);
}

BOOST_AUTO_TEST_CASE(imm) {
    raw_write16(0, 0032074 );
    raw_write16(2, 0x1234);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x1234);
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
    regs.d[2] = 0x01234567;
    regs.d[3] = 0x89ABCDEF;
    raw_write16(0, 0023002);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x01234567);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[2] = 0x12345678;
    regs.a[3] = 0x10;
    raw_write16(0, 0023302);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0x12345678);
    BOOST_TEST(regs.a[3] == 0x14);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[2] = 0x12345678;
    regs.a[3] = 0x14;
    raw_write16(0, 0023402);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0x12345678);
    BOOST_TEST(regs.a[3] == 0x10);
}


BOOST_AUTO_TEST_CASE(ext_pc1) {
    regs.d[1] = 0x12;
    raw_write16(0, 0022701);
    raw_write16(2, 0x1000);
     exception_check(4);  
}



BOOST_AUTO_TEST_CASE(ext_pc2) {
    regs.d[1] = 0x12;
    raw_write16(0, 0023701);
    raw_write16(2, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    raw_write16(4, 100);
     exception_check(4);  
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
    raw_write32(0x10, 0x12345678);
    regs.a[2] = 0x10;
    raw_write16(0, 0023032);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x14);
    BOOST_TEST(regs.d[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write32(0x10, 0x12345678);
    regs.a[2] = 0x14;
    raw_write16(0, 0023042);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(imm_addr_w) {
    raw_write16(0, 0022070);
    raw_write16(2, 0x100);
    raw_write32(0x100, 0x12345678);
     m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(imm) {
    raw_write16(0, 0023074 );
    raw_write32(2, 0x12345678);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x12345678);
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
