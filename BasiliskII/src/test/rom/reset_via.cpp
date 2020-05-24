#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
using namespace ROM;

void init(MB_TYPE t) {
	fixture f(t);
	MOCK::reset_all();
	clear_global();
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
}

BOOST_AUTO_TEST_CASE( none )  {
	init(MB_TYPE::GLU);
	DR(0) = 0;
	TEST_ROM( 02E8C );
}


BOOST_AUTO_TEST_CASE( via1 )  {
	init(MB_TYPE::GLU);
	AR(0) = 0x408032D4;
	AR(1) = 0x40803688;
	DR(0) = 1 << INIT_HW_FLG_T::VIA1;
	auto via1 = std::make_shared<IO_TEST<VIA1>>();

	machine->via1 = via1;
	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::ORA, { 1 } );
	via1->verify( VIA_REG::DDRA, { 0x3f } );
	via1->verify( VIA_REG::ORB, { 0x37 } );
	via1->verify( VIA_REG::DDRB, { 0xb7 });
	via1->verify( VIA_REG::PCR, { 0 });
	via1->verify( VIA_REG::ACR, { 0 });
	via1->verify( VIA_REG::IER, { 0x7f });
		
}

BOOST_AUTO_TEST_CASE( via2 )  {
	init(MB_TYPE::GLU);
	AR(0) = 0x408032D4;
	AR(1) = 0x40803688;
	DR(0) = 1 << INIT_HW_FLG_T::VIA2;
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via2->set_read_data( VIA_REG::ORA, { 0x80 } );
	machine->via2 = via2;
	TEST_ROM( 02E8C );
	via2->verify(VIA_REG::ORA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0xc0 });
	via2->verify(VIA_REG::ORB, { 0x07 });
	via2->verify(VIA_REG::DDRB, { 0x80 });
	via2->verify(VIA_REG::PCR, { 0x60 });
	via2->verify(VIA_REG::ACR, { 0xc0 });
	via2->verify(VIA_REG::IER, { 0x7f });
	via2->verify(VIA_REG::TIMER1_L, { 0x6e });
	via2->verify(VIA_REG::TIMER1_H, { 0x19 });
		
}

BOOST_AUTO_TEST_CASE( rbv_pbDuo_w_fpu )  {
	init(MB_TYPE::MSC);
	AR(0) = 0x408A8234;
	AR(1) = 0x408A7A30;
	DR(0) = 1 << INIT_HW_FLG_T::RBV;
	DR(1) = 1 << ROM_FLG_T::MONITOR;
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	machine->rbv = rbv;
	TEST_ROM( 02E8C );
	rbv->verify(RBV_REG::IER, { 0x7f });
	rbv->verify(RBV_REG::ORB, { 0x8f });
	rbv->verify(RBV_REG::SIER, { 0x7f });
		
}

BOOST_AUTO_TEST_CASE( rbv_iici )  {
	init(MB_TYPE::MDU);
	AR(0) = 0x40803378;
	AR(1) = 0x40803760;
	DR(0) = 1 << INIT_HW_FLG_T::RBV;
	DR(1) = 0;

	auto rbv = std::make_shared<IO_TEST<RBV>>();
	machine->rbv = rbv;
	TEST_ROM( 02E8C );
	rbv->verify(RBV_REG::IER, { 0x7f });
	rbv->verify(RBV_REG::ORB, { 0x8f });
	rbv->verify(RBV_REG::MONP, { 0x40 });
	rbv->verify(RBV_REG::SIER, { 0xff });
}

