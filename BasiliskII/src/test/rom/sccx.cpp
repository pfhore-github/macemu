#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "prototype.hpp"
#include "data.hpp"
#include "machine.hpp"
#include "scc.hpp"
#include "glu.hpp"
#include "scc_impl.hpp"
#include <memory>
namespace bdata = boost::unit_test::data;
using namespace ROM;
void prepare() {
	fixture f;
}
BOOST_AUTO_TEST_CASE( B98BC ) {
	prepare();
	AR(6) = 0;
	rom_routines[ 0x47752 ] =
		[]() {	
			AR(0) = 0x4080341C;
			AR(1) = 0x408037F0;
			INIT_HW_FLG.set( INIT_HW_FLG_T::IOP_SCC );
		};
	if( rom ) {		
		test_rom( 0x408B98BC );
	} else {
//		get_scc_addr();
	}
}

BOOST_AUTO_TEST_SUITE_END();

