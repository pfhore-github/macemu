#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(BKPT, InitFix)

BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[0044110 | dn] == opc_map[0044110]);
}

BOOST_AUTO_TEST_CASE( run )
{    
    raw_write16(0, 0044110);
    exception_check(4);
}

BOOST_AUTO_TEST_SUITE_END()