#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via1.hpp"
#include "via_test.hpp"
#include <memory>
using namespace ROM;
struct MockMSC : public MSC_REG {
	std::vector<uint8_t> executed_cmd;
	uint8_t cmd(uint8_t c) override {
		executed_cmd.push_back( c );
		return 0;
	}
};
uint8_t UT_get_JawsVia2(const std::shared_ptr<IO_BASE>& via);
uint8_t UT_get_SR_MSC(const std::shared_ptr<VIA>& via);
void prepare(MB_TYPE m)  {
	fixture f( m );
}
BOOST_AUTO_TEST_CASE( non_msc ) {
	prepare(MB_TYPE::JAWS);
	AR(0) = 0x40803608;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	machine->via2->write(VIA_REG::DDRB, 0xbd);
	machine->via2->write(VIA_REG::RB, 0x66);
	TEST_ROM( 478C2 );
	BOOST_TEST( UT_get_JawsVia2(machine->via2) == 0xff );
	BOOST_TEST( DR(2) == 0 );
	BOOST_TEST( cpu.Z );

}
BOOST_AUTO_TEST_CASE( msc ) {
	prepare(MB_TYPE::MSC);
	AR(0) = 0x408A8234;
	
	auto msc = std::make_shared<MockMSC>();
	machine->msc = msc;
	AR(2) = 0x50f00000;
	AR(3) = 0x50F26000;
	ROM_FLG.set( ROM_FLG_T::MSC );
	AR(5) = 0;
	DR(3) = 0xff;
	TEST_ROM( 478C2 );
	BOOST_TEST( DR(4) & 0x80000000 );
	std::vector<uint8_t> expected = { 0xff };
	BOOST_TEST( msc->executed_cmd  == expected );
	BOOST_TEST( DR(2) == 0 );
	BOOST_TEST( cpu.Z );
}

BOOST_AUTO_TEST_CASE( not_ready ) {
	prepare(MB_TYPE::JAWS);
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xFD);
	via2->reset_written();
	via2->b[TRANS_READY].set_read_data({ false, true } );
	machine->via2 = via2;
	AR(0) = 0x40803608;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	TEST_ROM( 478C2 );
	BOOST_TEST( DR(6) == 2 );
	BOOST_TEST( ! cpu.Z );
	via2->b[TRANS_STANDBY].verify( { false, true } );
}

BOOST_AUTO_TEST_CASE( not_ready2 ) {
	prepare(MB_TYPE::JAWS);
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xFD);
	via2->reset_written();
	
	via2->b[TRANS_READY].set_read_data({ false, false, false } );
	machine->via2 = via2;
	AR(0) = 0x40803608;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	TEST_ROM( 478C2 );
	BOOST_TEST( ! cpu.Z );
	via2->b[TRANS_STANDBY].verify( { false, true, true } );
}

BOOST_AUTO_TEST_CASE( ready ) {
	prepare(MB_TYPE::JAWS);
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xFd);
	via2->reset_written();
	
	via2->b[TRANS_READY].set_read_data({ false, false, true } );
	machine->via2 = via2;
	AR(0) = 0x40803608;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	TEST_ROM( 478C2 );
	BOOST_TEST( cpu.Z );
	via2->b[TRANS_STANDBY].verify( { false, true, true } );
}


