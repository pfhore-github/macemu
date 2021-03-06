#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "data.hpp"
#include "mdu.hpp"
#include "glu.hpp"
#include "data.hpp"
#include "machine.hpp"
using namespace ROM;
void prepare(std::unique_ptr<Machine>&& m) {
	fixture f(std::move(m));
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16);
}

BOOST_AUTO_TEST_CASE( success)  {
	prepare(std::make_unique<MDU>());
	AR(2) = 0x50F26013;
	TEST_ROM( 046D8 );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail)  {
	prepare(std::make_unique<GLU>());
	AR(2) = 0x50F10013;
	TEST_ROM( 046D8 );
	BOOST_CHECK( ! cpu.Z );
}


