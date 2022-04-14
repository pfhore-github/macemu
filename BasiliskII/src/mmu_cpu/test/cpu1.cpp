#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "newcpu.h"
#include <vector>
void m68k_do_execute ();
extern std::vector<uint8_t> RAM;
BOOST_AUTO_TEST_CASE( run_empty )
{
    regs.pc = 0;
    regs.vbr = 0;
    
    m68k_do_execute();
    BOOST_TEST( true /* test assertion */ );
}
