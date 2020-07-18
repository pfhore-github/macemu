#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"
#include "via.hpp"
#include "glu.hpp"
#include "sonora.hpp"
namespace ROM {
void play_asc_5() { MOCK::invoke<void>("play_asc_5"); }
void play_easc(int i) { MOCK::invoke<void>("play_easc", i); }
}
void prepare(std::unique_ptr<Machine>&& m) {
	fixture f(std::move(m));
	AR(3) = 0x50F14000;
	AR(5) = 0x50F00000;
	MOCK::reset_all();
	MOCK::make<void()>("play_asc_5")->always_do( []() { cpu.PC = AR(6);} );
	MOCK::make<void(int)>("play_easc");
	DEFINE_ROM( 0706A );
	DEFINE_ROM( 45C64 );
}
using namespace ROM;
BOOST_AUTO_TEST_CASE( asc ) {
	prepare(std::make_unique<GLU>());
	TEST_ROM( 45C52 );
	MOCK::verify("play_asc_5");
}

BOOST_AUTO_TEST_CASE( easc ) {
	prepare(std::make_unique<Sonora>());
	MOCK::get<void(int)>("play_easc")->always( 4 );
	TEST_ROM( 45C52 );
	MOCK::verify("play_easc");
}
