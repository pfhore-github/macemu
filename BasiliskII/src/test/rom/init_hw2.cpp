#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace ROM {
void reset_via() { MOCK::invoke<void>("reset_via"); }
void getHWInfo(uint8_t v) { return MOCK::invoke<void>("getHWInfo", v); }
}
using namespace ROM;
void prepare() {
	clear_global();
	fixture f;
	DEFINE_ROM(02E8C);
	DEFINE_ROM(02F64);
	MOCK::reset_all();
	MOCK::make<void()>("reset_via")->always_do( []() {});
	MOCK::make<void(uint8_t)>("getHWInfo")
		->always_do( [](uint8_t d) {
						 BOOST_CHECK_EQUAL( d, 0 );
						 motherboard = &motherboards[0];
						 model = &old_machines[0];
						 INIT_HW_FLG.set( INIT_HW_FLG_T::MASTER );
					 });
}
BOOST_AUTO_TEST_CASE( exec ) {
	prepare();
	DR(3) = 0x40800000;
	AR(7) = 0x1f00;
	TEST_ROM2( 04A5E, 02E46 );
	BOOST_TEST( AR(7) == 0x40801F00 );
	BOOST_TEST( cpu.VBR = 0x40803DD8 );
	MOCK::verify( "reset_via");
}



