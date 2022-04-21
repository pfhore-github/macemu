#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE( Unsigned, InitFix  )
BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( in_range ) {
    regs.d[1] = 1234;
    regs.d[2] = 3456;
    asm_m68k("mulu.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 1234 * 3456);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( ! regs.z);
}

BOOST_AUTO_TEST_CASE( zero ) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    asm_m68k("mulu.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_CASE( overflow ) {
    regs.d[1] = 0x10000;
    regs.d[2] = 0x10000;
    asm_m68k("mulu.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_CASE( high ) {
    regs.d[1] = 0x10000;
    regs.d[2] = 0x10000;
    asm_m68k("mulu.l %D1, %D3:%D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( regs.d[3] == 1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( in_range ) {
    regs.d[1] = 1234;
    regs.d[2] = 3456;
    asm_m68k("mulu.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 1234 * 3456);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( ! regs.z);
    BOOST_TEST( ! regs.c);
}

BOOST_AUTO_TEST_CASE( zero ) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    asm_m68k("mulu.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Signed, InitFix  )
BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( in_range ) {
    regs.d[1] = -1234;
    regs.d[2] = 3456;
    asm_m68k("muls.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == -1234 * 3456);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( ! regs.z);
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE( zero ) {
    regs.d[1] = -12345;
    regs.d[2] = 0;
    asm_m68k("muls.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( regs.z);
    BOOST_TEST(! regs.n);
}

BOOST_AUTO_TEST_CASE( overflow ) {
    regs.d[1] = -(0x10000);
    regs.d[2] = -(0x10000);
    asm_m68k("muls.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_CASE( high ) {
    regs.d[1] = -(0x10000);
    regs.d[2] = -(0x10000);
    asm_m68k("muls.l %D1, %D3:%D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( regs.d[3] == 1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( in_range ) {
    regs.d[1] = -1234;
    regs.d[2] = 3456;
    asm_m68k("muls.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( static_cast<int32_t>(regs.d[2]) == -1234 * 3456);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( ! regs.z);
    BOOST_TEST( ! regs.c);
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE( zero ) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    asm_m68k("muls.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()