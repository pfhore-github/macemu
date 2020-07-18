#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"
#include "via.hpp"
#include "glu.hpp"
#include "oss.hpp"
#include "sonora.hpp"

namespace ROM {
void play_asc_2() { MOCK::invoke<void>("play_asc_2"); }
void play_easc(int i) { MOCK::invoke<void>("play_easc", i); }
}
template<class T>
void prepare() {
	fixture f(std::make_unique<T>());
	AR(3) = 0x50F14000;
	AR(5) = 0x50F00000;
	MOCK::reset_all();
	MOCK::make<void()>("play_asc_2")->always_do( []() { cpu.PC = AR(6);} );
	MOCK::make<void(int)>("play_easc");
	DEFINE_ROM(07058);
	DEFINE_ROM(45C64);
}
using namespace ROM;
BOOST_AUTO_TEST_CASE( asc ) {
	prepare<GLU>();
	TEST_ROM( 45C1C );
	MOCK::verify("play_asc_2");
}

BOOST_AUTO_TEST_CASE( easc ) {
	prepare<Sonora>();
	MOCK::get<void(int)>("play_easc")->always( 1 );
	TEST_ROM( 45C1C );
	MOCK::verify( "play_easc" );
}
