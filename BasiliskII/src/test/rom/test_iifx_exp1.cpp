#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "oss.hpp"
using namespace ROM;
void prepare(MB_TYPE m) {
	fixture f(m);
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
	AR(2) = 0x50F1E000;
}

BOOST_AUTO_TEST_CASE( invalid )  {
	prepare(MB_TYPE::GLU);
	TEST_ROM( 04672 );
}

BOOST_AUTO_TEST_CASE( ng1 )  {
	prepare(MB_TYPE::OSS);
	auto exp1 = std::make_shared<IO_TEST<IIFX_exp1>>();
	machine->exp1 = exp1;
	exp1->set_read_data( 0, { 0, 0, 0, 0, 0x11, 0 } );
	TEST_ROM( 04672 );
	exp1->verify( 0, { 0xff, 0x11 } );
	exp1->verify( 0x10, { 0xff, 0xff, 0, 0xff } );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_CASE( ng2 )  {
	prepare(MB_TYPE::OSS);
	auto exp1 = std::make_shared<IO_TEST<IIFX_exp1>>();
	machine->exp1 = exp1;
	exp1->set_read_data( 0, { 0, 0, 0, 0, 0x11, 1, 1 } );
	TEST_ROM( 04672 );
	exp1->verify( 0, { 0xff, 0, 0x11, } );
	exp1->verify( 0x10, { 0xff, 0xff, 0, 0xff, 0xff, 0xff } );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_CASE( ok )  {
	prepare(MB_TYPE::OSS);
	auto exp1 = std::make_shared<IO_TEST<IIFX_exp1>>();
	machine->exp1 = exp1;
	exp1->set_read_data( 0, { 0, 0, 0, 0, 0x11, 1, 0 } );
	TEST_ROM( 04672 );
	exp1->verify( 0, { 0xff, 0, 0x11, } );
	exp1->verify( 0x10, { 0xff, 0xff, 0, 0xff, 0xff, 0xff } );
	BOOST_CHECK( cpu.Z );
}


