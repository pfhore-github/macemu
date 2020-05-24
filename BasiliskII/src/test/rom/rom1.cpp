#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "registers.hpp"
#include "exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"
#include "../../rom/prototype.hpp"
#include <unordered_map>
#include "../../new_cpu/inst_cmn.hpp"
#include "../../rom/data.hpp"
#include "machine.hpp"
namespace bdata = boost::unit_test::data;
extern uint8_t* ROMBaseHost;
extern uint32_t ROMSize;
BOOST_FIXTURE_TEST_SUITE(rom_check_glu, fixture);


BOOST_AUTO_TEST_CASE( t )  {
	set_sr( 0x2700 );
	stubs.clear();
	stubs[ 0x46AC] =
		[](CPU* cpu) {
			BOOST_TEST( cpu->R[10] == 0x50F01C00 );
			BOOST_TEST( cpu->R[2] == 0x20000 );
			cpu->Z = true;
		};	
	cpu.R[8] = 0x408032D4;
	ROM::check_glu();
	BOOST_TEST( cpu.PC == 0x2FB0 );
	
}

BOOST_AUTO_TEST_CASE( f )  {
	set_sr( 0x2700 );
	stubs.clear();
	stubs[ 0x46AC] =
		[](CPU* cpu) {
			BOOST_TEST( cpu->R[10] == 0x50F01C00 );
			BOOST_TEST( cpu->R[2] == 0x20000 );
			cpu->Z = false;
		};	
	cpu.R[8] = 0x408032D4;
	ROM::check_glu();
	BOOST_TEST( cpu.PC == 0x2FA4 );
	
}

BOOST_AUTO_TEST_SUITE_END();
