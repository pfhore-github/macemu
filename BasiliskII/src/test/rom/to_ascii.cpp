#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
namespace bdata = boost::unit_test::data;
void prepare() {
	fixture f;
	INIT_HW_FLG = 0;
	rom_base = 0x40800000;
}	
BOOST_DATA_TEST_CASE( digit, bdata::xrange(10), c ) {
	prepare();
	DR(0) = c;
	if( rom ) {
		test_rom( 0x408BA174 );
	} else {
		ROMWrapper::run_BA174();
	}
	BOOST_TEST( DR(0) == '0' + c );
}

BOOST_DATA_TEST_CASE( hex, bdata::xrange(6), c ) {
	prepare();
	DR(0) = 10 + c;
	if( rom ) {
		test_rom( 0x408BA174 );
	} else {
		ROMWrapper::run_BA174();
	}
	BOOST_TEST( DR(0) == 'A' + c );
}


