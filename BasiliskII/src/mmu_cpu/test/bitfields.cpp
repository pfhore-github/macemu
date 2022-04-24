#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(BFTST, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x18045678;
    asm_m68k("bftst %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x10045678;
    asm_m68k("bftst %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(reg_off) {
    regs.d[1] = 0x10045678;
    regs.d[2] = 4;
    asm_m68k("bftst %D1{%D2:8}");
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(reg_imm) {
    regs.d[1] = 0x10045678;
    regs.d[2] = 8;
    asm_m68k("bftst %D1{4:%D2}");
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x18);
    asm_m68k("bftst (%A1){4:4}");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    asm_m68k("bftst (%A1){4:4}");
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(BFCHG, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x18045678;
    asm_m68k("bfchg %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x17f45678);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0x12345678;
    asm_m68k("bfchg %D1{28:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xe2345677);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    asm_m68k("bfchg (%A1){4:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1C);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    asm_m68k("bfchg (%A1){4:8}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xD3);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    asm_m68k("bfchg (%A1){4:16}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xDc);
    BOOST_TEST(raw_read8(0x12) == 0xb5);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    asm_m68k("bfchg (%A1){4:24}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xDc);
    BOOST_TEST(raw_read8(0x12) == 0xba);
    BOOST_TEST(raw_read8(0x13) == 0x97);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write8(0x14, 0x89);
    asm_m68k("bfchg (%A1){4:32}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xDc);
    BOOST_TEST(raw_read8(0x12) == 0xba);
    BOOST_TEST(raw_read8(0x13) == 0x98);
    BOOST_TEST(raw_read8(0x14) == 0x79);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x13);
    asm_m68k("bfchg (%A1){%D2:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1C);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    asm_m68k("bfchg (%A1){12:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x1c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(BFCLR, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x18045678;
    asm_m68k("bfclr %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10045678);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0x12345678;
    asm_m68k("bfclr %D1{28:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x02345670);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    asm_m68k("bfclr (%A1){4:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x10);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    asm_m68k("bfclr (%A1){4:8}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x10);
    BOOST_TEST(raw_read8(0x11) == 0x03);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    asm_m68k("bfclr (%A1){4:16}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x10);
    BOOST_TEST(raw_read8(0x11) == 0x00);
    BOOST_TEST(raw_read8(0x12) == 0x05);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    asm_m68k("bfclr (%A1){4:24}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x10);
    BOOST_TEST(raw_read8(0x11) == 0x00);
    BOOST_TEST(raw_read8(0x12) == 0x00);
    BOOST_TEST(raw_read8(0x13) == 0x07);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write8(0x14, 0x89);
    asm_m68k("bfclr (%A1){4:32}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x10);
    BOOST_TEST(raw_read8(0x11) == 0x00);
    BOOST_TEST(raw_read8(0x12) == 0x00);
    BOOST_TEST(raw_read8(0x13) == 0x00);
    BOOST_TEST(raw_read8(0x14) == 0x09);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x13);
    asm_m68k("bfclr (%A1){%D2:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x10);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    asm_m68k("bfclr (%A1){12:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x10);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(BFSET, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x18045678;
    asm_m68k("bfset %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1ff45678);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0x12345678;
    asm_m68k("bfset %D1{28:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xf234567f);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    asm_m68k("bfset (%A1){4:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    asm_m68k("bfset (%A1){4:8}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xf3);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    asm_m68k("bfset (%A1){4:16}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xff);
    BOOST_TEST(raw_read8(0x12) == 0xf5);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    asm_m68k("bfset (%A1){4:24}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xff);
    BOOST_TEST(raw_read8(0x12) == 0xff);
    BOOST_TEST(raw_read8(0x13) == 0xf7);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write8(0x14, 0x89);
    asm_m68k("bfset (%A1){4:32}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xff);
    BOOST_TEST(raw_read8(0x12) == 0xff);
    BOOST_TEST(raw_read8(0x13) == 0xff);
    BOOST_TEST(raw_read8(0x14) == 0xf9);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x13);
    asm_m68k("bfset (%A1){%D2:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    asm_m68k("bfset (%A1){12:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x1f);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(BFEXTU, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x18045678;
    asm_m68k("bfextu %D1{4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x80);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0x12345678;
    asm_m68k("bfextu %D1{28:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x81);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    asm_m68k("bfextu (%A1){4:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 0x3);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    asm_m68k("bfextu (%A1){4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x12 );
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    asm_m68k("bfextu (%A1){4:16}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x1234);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    asm_m68k("bfextu (%A1){4:24}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x123456);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write8(0x14, 0x89);
    asm_m68k("bfextu (%A1){4:32}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 0x12345678);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x10;
    regs.d[2] = -4;
    raw_write8(0x10, 0x13);
    asm_m68k("bfextu (%A1){%D2:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 0x3);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    asm_m68k("bfextu (%A1){12:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x3);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(BFEXTS, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x1ff45678;
    asm_m68k("bfexts %D1{4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0xF234567f;
    asm_m68k("bfexts %D1{28:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    asm_m68k("bfexts (%A1){4:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = -1);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xF3);
    asm_m68k("bfexts (%A1){4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1 );
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xF5);
    asm_m68k("bfexts (%A1){4:16}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xFF);
    raw_write8(0x13, 0xF7);
    asm_m68k("bfexts (%A1){4:24}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xFF);
    raw_write8(0x13, 0xFF);
    raw_write8(0x14, 0xF9);
    asm_m68k("bfexts (%A1){4:32}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = -1);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x10;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    asm_m68k("bfexts (%A1){%D2:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = -1);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x1F);
    asm_m68k("bfexts (%A1){12:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(BFFFO, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x12f45678;
    asm_m68k("bfffo %D1{4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 6);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0x10045678;
    asm_m68k("bfffo %D1{4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 12);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0x02345670;
    asm_m68k("bfffo %D1{28:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 36);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    asm_m68k("bfffo (%A1){4:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 7);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x13);
    asm_m68k("bfffo (%A1){4:8}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 11 );
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x00);
    raw_write8(0x12, 0x15);
    asm_m68k("bfffo (%A1){4:16}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 19);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x00);
    raw_write8(0x12, 0x00);
    raw_write8(0x13, 0x17);
    asm_m68k("bfffo (%A1){4:24}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 27);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x00);
    raw_write8(0x12, 0x00);
    raw_write8(0x13, 0x00);
    raw_write8(0x14, 0x19);
    asm_m68k("bfffo (%A1){4:32}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 35);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    asm_m68k("bfffo (%A1){%D2:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = -1);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x11);
    asm_m68k("bfffo (%A1){12:4}, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 15);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(BFINS, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0x80;
    asm_m68k("bfins %D3, %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0;
    asm_m68k("bfins %D3, %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0x9a;
    asm_m68k("bfins %D3, %D1{4:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x19a45678);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0xab;
    asm_m68k("bfins %D3, %D1{28:8}");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xb234567a);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    regs.d[3] = 0x07;
    raw_write8(0x10, 0x11);
    asm_m68k("bfins %D3, (%A1){4:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) = 0x17);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    regs.d[3] = 0xab;
    asm_m68k("bfins %D3, (%A1){4:8}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1a );
    BOOST_TEST(raw_read8(0x11) == 0xb4 );
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    regs.d[3] = 0xabcd;
    asm_m68k("bfins %D3, (%A1){4:16}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1a );
    BOOST_TEST(raw_read8(0x11) == 0xbc );
    BOOST_TEST(raw_read8(0x12) == 0xd6 );
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    raw_write8(0x13, 0x78);
    regs.d[3] = 0xabcdef;
    asm_m68k("bfins %D3, (%A1){4:24}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1a );
    BOOST_TEST(raw_read8(0x11) == 0xbc );
    BOOST_TEST(raw_read8(0x12) == 0xde );
    BOOST_TEST(raw_read8(0x13) == 0xf8 );
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    raw_write8(0x13, 0x78);
    raw_write8(0x14, 0x9a);
    regs.d[3] = 0xabcdef01;
    asm_m68k("bfins %D3, (%A1){4:32}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1a );
    BOOST_TEST(raw_read8(0x11) == 0xbc );
    BOOST_TEST(raw_read8(0x12) == 0xde );
    BOOST_TEST(raw_read8(0x13) == 0xf0 );
    BOOST_TEST(raw_read8(0x14) == 0x1a );
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    regs.d[3] = 0x4;
    asm_m68k("bfins %D3, (%A1){%D2:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) = 0x14);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x19);
    regs.d[3] = 0x7;
    asm_m68k("bfins %D3, (%A1){12:4}");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x17);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()