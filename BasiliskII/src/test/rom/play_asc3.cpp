#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"
#include "via.hpp"

using namespace ROM;
namespace ROM {
void play_asc( const ASC_param& p) {
	BOOST_TEST( p.volume == 0x0506 );
	BOOST_TEST( p.wait == 39 );
	BOOST_TEST( p.delay == 3000 );
	BOOST_TEST( p.repeat == 20000 );
	BOOST_TEST( p.chs == 1 );
	BOOST_TEST( p.freq[0] == 197234 );
}
}
BOOST_AUTO_TEST_CASE( ring ) {
	fixture f;
	AR(5) = 0x50f00000;
	AR(3) = 0x50F14000;
	DEFINE_ROM( 0706E );
	TEST_ROM( 0705E );
}
