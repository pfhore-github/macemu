#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_CASE( dx, InitFix  ) {
    regs.d[1] = 0x1234;
    asm_m68k("unpk %D1, %D2, #0x10");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x314 );
}

BOOST_FIXTURE_TEST_CASE( ax, InitFix  ) {
    raw_write8(0x20, 0x12);
    regs.a[1] = 0x21;
    regs.a[2] = 0x12;
    asm_m68k("unpk -(%A1), -(%A2), #0x10");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 0x0112);
    BOOST_TEST( regs.a[1] == 0x20 );
    BOOST_TEST( regs.a[2] == 0x10 );
}
