#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "glu.hpp"
#include "oss.hpp"
#include "scc_impl.hpp"
#include <memory>
std::function<void()> get_hw_info2_wrapper;
namespace ROM {

void getHWInfo2(uint8_t) {
	get_hw_info2_wrapper();
}
}
template<class T>
void prepare() {
	clear_global();
	fixture f(std::make_unique<T>());	
	rom_base = 0x40800000;
	DEFINE_ROM(47752);
}
using namespace ROM;
BOOST_AUTO_TEST_CASE( glu ) {
	prepare<GLU>();
	AR(6) = 0;
	get_hw_info2_wrapper = 
		[]() {	
			motherboard = &motherboards[0];
			model = &old_machines[0];
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IOP_SCC );
		};
	if( rom ) {		
		test_rom( 0x408B9F46 );
	} else {
		ROMWrapper::run_B9F46();
	}
	BOOST_TEST( AR(2) == 0x50F00000 );
	BOOST_TEST( DR(2) == 0 );
	BOOST_TEST( AR(3) == 0x50F04000 );
	BOOST_TEST( DR(3) == 0 );
}

BOOST_AUTO_TEST_CASE( oss ) {
	prepare<OSS>();
	AR(6) = 0;
	get_hw_info2_wrapper = 
		[]() {	
			motherboard = &motherboards[2];
			model = &old_machines[5];
			INIT_HW_FLG.set( INIT_HW_FLG_T::IOP_SCC );
		};
	if( rom ) {		
		test_rom( 0x408B9F46 );
	} else {
		ROMWrapper::run_B9F46();
	}
	BOOST_TEST( AR(2) == 0x50F00000 );
	BOOST_TEST( DR(2) == 0 );
	BOOST_TEST( AR(3) == 0x50F04020 );
	BOOST_TEST( DR(3) == 0 );
}

