#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_CASE( dx, InitFix  ) {
    regs.d[1] = 0x1234;
    asm_m68k("pack %D1, %D2, #0x100");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x34 );
}

BOOST_FIXTURE_TEST_CASE( ax, InitFix  ) {
    raw_write16(0x20, 0x1234);
    regs.a[1] = 0x22;
    regs.a[2] = 0x11;
    asm_m68k("pack -(%A1), -(%A2), #0x100");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 0x34 );
    BOOST_TEST( regs.a[1] == 0x20 );
    BOOST_TEST( regs.a[2] == 0x10 );
}
