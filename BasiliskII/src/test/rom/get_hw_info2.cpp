#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
namespace ROM {
void getHWInfo(uint8_t v) { MOCK::invoke<void>("getHWInfo", v); }
}
void prepare() {
	fixture f;
	MOCK::make<void(uint8_t)>("getHWInfo")
		->always_do( [](uint8_t v) {
						 BOOST_CHECK_EQUAL(v, 0);
						 motherboard = &motherboards[0];
						 model = &old_machines[0];
					 } );
	DEFINE_ROM( 02F64 );
}
BOOST_AUTO_TEST_CASE( exec ) {
	prepare();
	TEST_ROM( 47752 );
	BOOST_CHECK_EQUAL( cpu.intmask, 7 );
	MOCK::verify("getHWInfo");
}



