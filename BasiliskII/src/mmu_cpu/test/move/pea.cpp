#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(PEA, InitFix)
BOOST_DATA_TEST_CASE(opc, EA_M(), dn) {
    BOOST_TEST(opc_map[0044100 | dn] == opc_map[0044120]);
}

BOOST_AUTO_TEST_CASE( run )
{    
    regs.a[7] = 0x104;
    regs.a[2] = 0x10;
    raw_write16(0, 0044122);
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x10 );
    BOOST_TEST( regs.a[7] == 0x100 );
}

BOOST_AUTO_TEST_SUITE_END()