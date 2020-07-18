#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "via1.hpp"
#include "via_test.hpp"
#include "msc_test.hpp"
#include <memory>
using namespace ROM;

template<class T, class... Args>
void prepare(Args&&... args)  {
	fixture f( std::make_unique<T>(std::forward<Args>(args)... ));
	DR(2) = 0;
	ROMWrapper::run_02F64();
}
BOOST_AUTO_TEST_CASE( non_msc ) {
	prepare<JAWS>();
	machine->via2->write( VIA_REG::DDRB, 0xbd );
	auto pb_ex_ut = std::make_shared<MockMSC>();
	machine->pb_ex = pb_ex_ut;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xE2;
	TEST_ROM( 478C2 );
	std::vector<uint8_t> expected = { 0xE2 };
	BOOST_CHECK_EQUAL_COLLECTIONS( pb_ex_ut->executed_cmd.begin(), pb_ex_ut->executed_cmd.end(),
								   expected.begin(), expected.end() );
	BOOST_CHECK_EQUAL( DR(6), 0 );
	BOOST_CHECK( cpu.Z );
	BOOST_CHECK( !(DR(4) & 0x80000000) );

}
BOOST_AUTO_TEST_CASE( msc ) {
	prepare<MSC>();

	auto pb_ex_ut = std::make_shared<MockMSC>();
	machine->pb_ex = pb_ex_ut;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xE2;
	TEST_ROM( 478C2 );
	std::vector<uint8_t> expected = { 0xE2 };
	BOOST_CHECK( (DR(4) & 0x80000000) );
	BOOST_CHECK_EQUAL( DR(6), 0 );
	BOOST_CHECK( cpu.Z );
	BOOST_CHECK_EQUAL_COLLECTIONS( pb_ex_ut->executed_cmd.begin(), pb_ex_ut->executed_cmd.end(),
								   expected.begin(), expected.end() );
}

