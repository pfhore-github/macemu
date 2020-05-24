#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"
namespace bdata = boost::unit_test::data;
using namespace ROM;
void prepare() {
	fixture f(MB_TYPE::SONORA);
	INIT_HW_FLG = 0;
	ROM_FLG = 0;
	HWCFG_FLAGS = 0;
	GID = 0;
	GEN = 0;
	INIT_FLAGS = 0;
	motherboard = nullptr;
	model = nullptr;
}
BOOST_DATA_TEST_CASE( ring, bdata::xrange(5), a ) {
	prepare();
	AR(5) = 0x50f00000;
	DR(0) = a;	
	AR(3) = 0x50F14000;
	TEST_ROM( 45C64 );
	machine->asc.reset();
}
