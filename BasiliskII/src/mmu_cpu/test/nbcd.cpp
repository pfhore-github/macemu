#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( cx, InitFix  ) {
    regs.d[1] = 0x27;
    regs.z = true;
    regs.x = true;
    asm_m68k("nbcd %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0x72 );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.x );
    BOOST_TEST( ! regs.z );
}

BOOST_FIXTURE_TEST_CASE( z, InitFix  ) {
    regs.d[1] = 0;
    regs.z = true;
    regs.x = false;
    asm_m68k("nbcd %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.x );
}

