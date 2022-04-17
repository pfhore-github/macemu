#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( n, InitFix  ) {
    regs.d[1] = 0x12348678;
    regs.z = true;
    regs.c = true;
    asm_m68k("swap %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0x86781234 );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( ! regs.v );
    BOOST_TEST( regs.n );
}

BOOST_FIXTURE_TEST_CASE( z, InitFix  ) {
    regs.d[1] = 0;
    asm_m68k("swap %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

