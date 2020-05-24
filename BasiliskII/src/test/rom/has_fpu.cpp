#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
void prepare() {
	fixture f;
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
}
BOOST_AUTO_TEST_CASE( success )  {
	prepare();
	TEST_ROM2( 04640, 04B18 );
	BOOST_CHECK( cpu.Z );	
}
BOOST_AUTO_TEST_CASE( fail )  {
	prepare();
	FPU* fpu_b = fpu;
	NoFPU nofpu;
	fpu = &nofpu;
	TEST_ROM2( 04640, 04B18 );
	BOOST_CHECK( ! cpu.Z );	
	fpu = fpu_b;
}
