#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

using namespace ROM;
namespace bdata = boost::unit_test::data;

namespace ROM {
bool has_fpu() { return MOCK::invoke<bool>("has_fpu"); }
bool has_ether(uint32_t a) { return MOCK::invoke<bool>("has_ether", a); }
bool check_LMG1(uint32_t a) { return MOCK::invoke<bool>("check_LMG1", a); }
}
void prepare() {
	fixture f;
	set_sr( 0x2700 );
	motherboard = &motherboards[4];
	AR(0) = rom_base | motherboard->BASE;
	MOCK::reset_all();
	MOCK::make<bool()>("has_fpu");
	MOCK::make<bool(uint32_t)>("has_ether");
	MOCK::make<bool(uint32_t)>("check_LMG1");
	DEFINE_ROM( 04640 );
	DEFINE_ROM( 047B0 );
	DEFINE_ROM( 047BA );
}
// 4B0C
BOOST_DATA_TEST_CASE( fpu_check, bdata::xrange(2), tf )  {
	prepare();
	MOCK::get<bool()>("has_fpu")->always( tf );
	DR(2) = 1 << ( HWCFG_T::HAS_FPU + 16);
	TEST_ROM2( 04B0C, 0309C );
	SET_D2( DR(2) );
	BOOST_CHECK_EQUAL( HWCFG_FLAGS.test( HWCFG_T::HAS_FPU) , tf );
}

BOOST_DATA_TEST_CASE( ether_check, bdata::xrange(2), tf )  {
	prepare();
	MOCK::get<bool(uint32_t)>("has_ether")->always( tf, 0x50F0A000 );
	DR(0) = 1 << INIT_HW_FLG_T::ETHER;
	TEST_ROM2( 04B0C, 0309C );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK_EQUAL( INIT_HW_FLG.test( INIT_HW_FLG_T::ETHER ), tf);
}


BOOST_DATA_TEST_CASE( lowmem, bdata::xrange(2), tf )  {
	prepare();
	MOCK::get<bool(uint32_t)>("check_LMG1")->always( tf, 0x74 );
	DR(0) = 1 << INIT_HW_FLG_T::SPECIAL_LOWMEM;
	TEST_ROM2( 04B0C, 0309C );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK_EQUAL( INIT_HW_FLG.test( INIT_HW_FLG_T::SPECIAL_LOWMEM ), tf );
}

