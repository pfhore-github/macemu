#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "data.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "machine.hpp"
using namespace ROM;

void prepare() {
	fixture f;
	set_sr( 0x2700 );
	AR(0) = 0x40803564;
	DR(0) = 1 << INIT_HW_FLG_T::VIA1;
}	
BOOST_AUTO_TEST_CASE( via1_via2 )  {
	prepare();
	DR(0) |= 1 << INIT_HW_FLG_T::VIA2;
	auto via1_test = std::make_shared<IO_TEST<VIA1>>();
	via1_test->set_read_data(15, { 0xff } );
	via1_test->set_read_data(3, { 0xfe } );
	via1_test->set_read_data(0, { 0xfd } );
	via1_test->set_read_data(2, { 0xfc } );
	auto via2_test = std::make_shared<IO_TEST<VIA2>>();
	via2_test->set_read_data(15, { 0xef } );
	via2_test->set_read_data(3, { 0xee } );
	via2_test->set_read_data(0, { 0xed } );
	via2_test->set_read_data(2, { 0xec } );
	machine->via1 = via1_test;
	machine->via2 = via2_test;
	TEST_ROM( 047CC );
	via1_test->verify( 15, { 0xff });
	via1_test->verify( 3, { 0, 0xfe });
	via1_test->verify( 0, { 0xfd } );
	via1_test->verify( 2, { 0xc0, 0xfc } );
	via2_test->verify( 15, { 0xef });
	via2_test->verify( 3, { 0, 0xee });
	via2_test->verify( 0, { 0xed });
	via2_test->verify( 2, { 0, 0xec });

	BOOST_CHECK_EQUAL( DR(1), 0xfffdefed );
}

BOOST_AUTO_TEST_CASE( via1 )  {
	prepare();
	DR(0) &= ~(1 << INIT_HW_FLG_T::VIA2);
	auto via1_test = std::make_shared<IO_TEST<VIA1>>();
	via1_test->set_read_data(15, { 0xff });
	via1_test->set_read_data(3, { 0xfe });
	via1_test->set_read_data(0, { 0xfd });
	via1_test->set_read_data(2, { 0xfc });
	machine->via1 = via1_test;
	TEST_ROM( 047CC );
	BOOST_CHECK_EQUAL( DR(1), 0xfffd0000);
	via1_test->verify( 15, { 0xff });
	via1_test->verify( 3, { 0, 0xfe });
	via1_test->verify( 0, { 0xfd } );
	via1_test->verify( 2, { 0xc0, 0xfc } );
}

