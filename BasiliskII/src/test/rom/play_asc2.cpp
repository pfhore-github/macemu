#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"
#include "via.hpp"

using namespace ROM;
namespace ROM {
void play_asc( const ASC_param& p) {
	BOOST_CHECK_EQUAL( p.volume, 0x0607 );
	BOOST_CHECK_EQUAL( p.wait, 39 );
	BOOST_CHECK_EQUAL( p.delay, 0 );
	BOOST_CHECK_EQUAL( p.repeat, 5000 );
	BOOST_CHECK_EQUAL( p.chs, 4 );
	BOOST_CHECK_EQUAL( p.freq[0], 49308 );
	BOOST_CHECK_EQUAL( p.freq[1], 57526 );
	BOOST_CHECK_EQUAL( p.freq[2], 73962 );
	BOOST_CHECK_EQUAL( p.freq[3], 98993 );
}
}
BOOST_AUTO_TEST_CASE( ring ) {
	fixture f;
	AR(5) = 0x50f00000;
	AR(3) = 0x50F14000;
	DEFINE_ROM( 0706E );
	TEST_ROM( 07058 );
}
