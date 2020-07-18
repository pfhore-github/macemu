#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "glu.hpp"
#include "msc.hpp"
#include "jaws.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "via_test.hpp"
#include "powerbook.hpp"
#include <memory>
using namespace ROM;
void UT_get_sawsVia2(const std::shared_ptr<IO_BASE>& via, uint8_t v);
template<class T>
void prepare() {
	fixture f(std::make_unique<T>());
	rom_base = 0x40800000;
}
class PB_EX_REG_TEST : public PB_EX_REG {
public:
	void UT_PUSH_OUT(uint8_t v) { c_out.push_back(v); }
};
BOOST_AUTO_TEST_CASE( msc_ok ) {
	prepare<MSC>();
	auto pb_ex_ut = std::make_shared<PB_EX_REG_TEST>();
	machine->pb_ex = pb_ex_ut; 
	pb_ex_ut->UT_PUSH_OUT( 0xab );
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0x50F26000;
	AR(5) = 0;
	TEST_ROM ( 47928 );
	BOOST_CHECK_EQUAL( DR(3), 0xab );
	BOOST_CHECK_EQUAL( DR(6), 0 );
	BOOST_CHECK( DR(4) & 0x80000000 );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( not_yet ) {
	prepare<MSC>();
	auto pb_ex_ut = std::make_shared<PB_EX_REG_TEST>();
	machine->pb_ex = pb_ex_ut; 
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0x50F26000;
	AR(5) = 0;
	ROMWrapper::run_47928();
	BOOST_CHECK_EQUAL( DR(6), 2 );
	BOOST_CHECK( ! cpu.Z );
}


BOOST_AUTO_TEST_CASE( jaws ) {
	prepare<JAWS>();
	auto pb_ex_ut = std::make_shared<PB_EX_REG_TEST>();
	machine->pb_ex = pb_ex_ut; 
	pb_ex_ut->UT_PUSH_OUT( 0xab );
	machine->via2->clear( VIA_REG::RB, 2);
	AR(0) = 0x40803608;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	ROMWrapper::run_47928();
	BOOST_CHECK( !( DR(4) & 0x80000000) );
}

BOOST_AUTO_TEST_CASE( non_pb ) {
	prepare<GLU>();
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0x50F26000;
	AR(5) = 0;
	ROMWrapper::run_47928();
	BOOST_CHECK_EQUAL( DR(6), 2 );
	BOOST_CHECK( ! cpu.Z );
}
