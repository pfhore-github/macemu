#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "asc.hpp"
#include "sonora.hpp"
using namespace ROM;
namespace ROM {
int check_easc_fifo_half() { return MOCK::invoke<int>("check_easc_fifo_half"); }
}
void prepare() {
	fixture f(std::make_unique<Sonora>());
	rom_base = 0x40800000;
	MOCK::make<int()>("check_easc_fifo_half");
	DEFINE_ROM ( BD41E );
}
BOOST_AUTO_TEST_CASE( ut ) {
	prepare();
	AR(6) = 0x50F14000;
	MOCK::get<int()>("check_easc_fifo_half")
		->nth_is(1, 1)
		.nth_is(2, 0);
	TEST_ROM( 45E3C );
}

		
