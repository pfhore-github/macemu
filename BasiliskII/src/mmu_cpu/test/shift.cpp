#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(ASR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffffff00 | 32;
    asm_m68k("asrb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff00 | 8 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = -128;
    asm_m68k("asrb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -1 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 32;
    regs.d[2] = 2;
    asm_m68k("asrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("asrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffff0000 | 32;
    asm_m68k("asrw #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff0000 | 8 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x1000;
    asm_m68k("asrw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 16 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = -1024;
    regs.d[2] = 2;
    asm_m68k("asrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -256 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = -32;
    regs.d[2] = 0;
    asm_m68k("asrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, -16);
    asm_m68k("asrw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = static_cast<uint16_t>(-8));
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x10000;
    asm_m68k("asrl #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4000 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10000;
    asm_m68k("asrl #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = -40000;
    regs.d[2] = 2;
    asm_m68k("asrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -10000 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = -32000;
    regs.d[2] = 0;
    asm_m68k("asrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -32000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(LSR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffffff00 | 32;
    asm_m68k("lsrb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff00 | 8 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0xff;
    asm_m68k("lsrb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 32;
    regs.d[2] = 2;
    asm_m68k("lsrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("lsrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lsrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffff0000 | 32;
    asm_m68k("lsrw #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff0000 | 8 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x1000;
    asm_m68k("lsrw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 16 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x8000;
    regs.d[2] = 2;
    asm_m68k("lsrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2000 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("lsrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lsrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}


BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x8000);
    asm_m68k("lsrw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x4000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x10000;
    asm_m68k("lsrl #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4000 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10000;
    asm_m68k("lsrl #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 2;
    asm_m68k("lsrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x20000000 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("lsrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lsrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ROXR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xffffff39;
    asm_m68k("roxrb #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff67 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x40;
    asm_m68k("roxrb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x39;
    regs.d[2] = 3;
    asm_m68k("roxrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x47 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("roxrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 67;
    asm_m68k("roxrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x47 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xffff3009;
    asm_m68k("roxrw #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff6601 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x1000;
    asm_m68k("roxrw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x3009;
    regs.d[2] = 3;
    asm_m68k("roxrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4601 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("roxrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3009;
    regs.d[2] = 67;
    asm_m68k("roxrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4601 );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x8000);
    asm_m68k("roxrw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x4000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0x30000009;
    asm_m68k("roxrl #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x66000001 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10000;
    asm_m68k("roxrl #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x30000009;
    regs.d[2] = 3;
    asm_m68k("roxrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x46000001 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("roxrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("roxrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ROR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffffff39;
    asm_m68k("rorb #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff93 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x40;
    asm_m68k("rorb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x39;
    regs.d[2] = 4;
    asm_m68k("rorb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("rorb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 68;
    asm_m68k("rorb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffff1234;
    asm_m68k("rorw #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff4123 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x1000;
    asm_m68k("rorw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x3009;
    regs.d[2] = 4;
    asm_m68k("rorw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x9300 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("rorw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x1234;
    regs.d[2] = 68;
    asm_m68k("rorw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4123 );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x8000);
    asm_m68k("rorw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x4000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0x12345678;
    asm_m68k("rorl #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x81234567 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10000;
    asm_m68k("rorl #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 4;
    asm_m68k("rorl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x81234567 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("rorl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("rorl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(ASL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffffff00 | 32;
    asm_m68k("aslb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff00 | 128 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0xff;
    asm_m68k("aslb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 32;
    regs.d[2] = 2;
    asm_m68k("aslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("aslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("aslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffff0000 | 32;
    asm_m68k("aslw #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff0000 | 128 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 16;
    asm_m68k("aslw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = -1024;
    regs.d[2] = 2;
    asm_m68k("aslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -4096 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = -32;
    regs.d[2] = 0;
    asm_m68k("aslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("aslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, -16);
    asm_m68k("aslw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = static_cast<uint16_t>(-64));
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x10000;
    asm_m68k("asll #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40000 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("asll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = -10000;
    regs.d[2] = 2;
    asm_m68k("asll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -40000 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = -32000;
    regs.d[2] = 0;
    asm_m68k("asll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -32000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(LSL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffffff00 | 32;
    asm_m68k("lslb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff00 | 128 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0xff;
    asm_m68k("lslb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 32;
    regs.d[2] = 2;
    asm_m68k("lslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("lslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffff0000 | 32;
    asm_m68k("lslw #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff0000 | 128 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 16;
    asm_m68k("lslw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x2000;
    regs.d[2] = 2;
    asm_m68k("lslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("lslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}


BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    asm_m68k("lslw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x10000;
    asm_m68k("lsll #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40000 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("lsll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x20000000;
    regs.d[2] = 2;
    asm_m68k("lsll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("lsll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lsll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(ROXL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xffffffc3;
    asm_m68k("roxlb #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff1f );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x40;
    asm_m68k("roxlb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x20 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0xc3;
    regs.d[2] = 3;
    asm_m68k("roxlb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("roxlb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc3;
    regs.d[2] = 67;
    asm_m68k("roxlb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xffffc003;
    asm_m68k("roxlw #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff001F );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10;
    asm_m68k("roxlw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0xc003;
    regs.d[2] = 3;
    asm_m68k("roxlw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("roxlw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc003;
    regs.d[2] = 67;
    asm_m68k("roxlw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    asm_m68k("roxlw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xc0000003;
    asm_m68k("roxll #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1f );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("roxll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0xc0000003;
    regs.d[2] = 3;
    asm_m68k("roxll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("roxll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("roxll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ROL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffffff39;
    asm_m68k("rolb #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffff93 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x40;
    asm_m68k("rolb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x39;
    regs.d[2] = 4;
    asm_m68k("rolb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93 );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("rolb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 68;
    asm_m68k("rolb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0xffff1234;
    asm_m68k("rolw #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff2341 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10;
    asm_m68k("rolw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x1234;
    regs.d[2] = 4;
    asm_m68k("rolw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2341 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("rolw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x1234;
    regs.d[2] = 68;
    asm_m68k("rolw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2341 );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    asm_m68k("rolw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0x12345678;
    asm_m68k("roll #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x23456781 );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("roll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 4;
    asm_m68k("roll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x23456781 );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("roll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("roll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
