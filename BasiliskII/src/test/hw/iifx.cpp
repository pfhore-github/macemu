#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via_test.hpp"

BOOST_AUTO_TEST_SUITE(via);

BOOST_AUTO_TEST_CASE( iifx_exp1_ok )  {
	VIA_TEST v;
	v.set_orb( 0x1 );
	v.set_irb( 0x10 );
	v.b[2].set_read_data( { true } );
	v.b[3].set_read_data( { false } );
	v.b[4].set_read_data( { false } );
	v.b[5].set_read_data( { false } );
	v.write( VIA_REG::DDRB, 0xC3 );
	BOOST_TEST( v.read(VIA_REG::ORB) == 0x05 );
}
BOOST_AUTO_TEST_SUITE_END();
