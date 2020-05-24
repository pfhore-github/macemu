#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
#include "mcu.hpp"
#include "jaws.hpp"
#include "memcjr.hpp"
#include <memory>
namespace bdata = boost::unit_test::data;
extern uint32_t mac_model_id;
using namespace ROM;

void prepare(MB_TYPE mb_t, const MOTHERBOARD_TABLE_T* mb, const MACHINE_TABLE_T* mc) {
	clear_global();
	fixture f(mb_t);
	set_sr( 0x2700 );
	DR(0) = mc->init_flg.to_ulong();
	AR(0) = rom_base | mb->BASE;
	AR(1) = rom_base | mc->BASE;
}
BOOST_AUTO_TEST_CASE( rbv_lciii_p )  {
	prepare(MB_TYPE::SONORA, &motherboards[7], &new_machines[5]);
	machine->model_id = 0xA55A0003;
	auto rbv = std::make_shared<IO_TEST<SonorRBV>>();
	machine->rbv = rbv;
	TEST_ROM( 04866 );
	rbv->verify(1, { 0x40 });
	rbv->verify(4, { 0 });
	rbv->verify(5, { 3 });

}
BOOST_AUTO_TEST_CASE( rbv_lciii )  {
	prepare(MB_TYPE::SONORA, &motherboards[7], &new_machines[4]);
	machine->model_id = 0xA55A0002;
	auto rbv = std::make_shared<IO_TEST<SonorRBV>>();
	machine->rbv = rbv;
	TEST_ROM( 04866 );
	rbv->verify(1, { 0x40 });
	rbv->verify(4, { 0 });
	rbv->verify(5, { 3, 2 });
}


BOOST_DATA_TEST_CASE( mcu, bdata::xrange(2), tf )  {
	prepare(MB_TYPE::MCU, &motherboards[4], &old_machines[8]);   
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via2->set_read_data(0, { uint8_t(tf ? 0x20 : 0) } );
	via2->set_read_data(2, { 0 } );
	auto mcu = std::make_shared<IO_TEST<MCU_REG>>();
	mcu->set_read_data(0x48, { 0 } );
	mcu->set_read_data(0x49, { 0 } );
	mcu->set_read_data(0x4A, { 0 } );
	mcu->set_read_data(0x4B, { 0 } );
	machine->via2 = via2;
	machine->mcu = mcu;
	TEST_ROM( 04866 );
	uint32_t val = tf ? 0x138b0810 : 0x124f0810;
	for(int i = 0; i < 32 ; ++i ) {
		uint32_t x = val >> i;
		mcu->verify( i*4, { uint8_t( x >> 24 ) });
		mcu->verify( i*4+1, { uint8_t( x >> 16 ) });
		mcu->verify( i*4+2, { uint8_t( x >> 8 ) });
		mcu->verify( i*4+3, { uint8_t( x ) });
	}
	for( int i = 0x80; i < 0x87; ++i ) {
		mcu->verify( i , { 0 });
	}
	mcu->verify( 0x87, { 1 });
	for(int i = 0xa0; i < 0xbc; i += 4 ) { 
		mcu->verify( i, { 0 });
		mcu->verify( i+1, { 0 });
		mcu->verify( i+2, { 0xff });
		mcu->verify( i+3, { 0xff });
	}
}


BOOST_AUTO_TEST_CASE( jaws )  {
	prepare(MB_TYPE::JAWS, &motherboards[5], &old_machines[11]);

	machine->model_id = 0xA55A0002;
	auto pb = std::make_shared<IO_TEST<JAWS_REG>>();
	auto pb2 = std::make_shared<IO_TEST<JAWS_REG2>>();
	pb2->set_read_data(0, { 0xffU });
	machine->pb = pb;
	machine->pb2 = pb2;
	TEST_ROM( 04866 );
	pb->verify( 0, { 1 } );
}

BOOST_AUTO_TEST_CASE( memcjr_quadra605_20Mhz )  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[10]);
	machine->model_id = 0xa55a2224;
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xA5 } );
	k1->verify(0x31, { 0x5A });
	k1->verify(0x32, { 0x01 });
	k1->verify(0x33, { 0x01 });
	k1->verify(0x34, { 0xA5 });
	k1->verify(0x35, { 0x5A });
	k1->verify(0x36, { 0x01 });
	k1->verify(0x37, { 0x1D });
	k1->verify(0x7C, { 0x06, 0x76 });
	k1->verify(0x7D, { 0x95, 0x95 });
	k1->verify(0x7E, { 0x68, 0x68 });
	k1->verify(0x7F, { 0x04, 0x04 });
	k2->verify( 0, { 1 } );
	k2->verify( 0x600, { 0x02 } );
	k2->verify( 0x601, { 0x80 } );
}

BOOST_AUTO_TEST_CASE( memcjr_quadra605_25Mhz )  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[11]);
	machine->model_id = 0xa55a2225;
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify( 0x30, { 0xA5 } );
	k1->verify(0x31, { 0x5A });
	k1->verify(0x32, { 0x00 });
	k1->verify(0x33, { 0x1A });
	k1->verify(0x34, { 0xA5 });
	k1->verify(0x35, { 0x5A });
	k1->verify(0x36, { 0x01 });
	k1->verify(0x37, { 0x6B });
	k1->verify(0x7C, { 0x6A, 0xAE });
	k1->verify(0x7D, { 0x95, 0x95 });
	k1->verify(0x7E, { 0x68, 0x68 });
	k1->verify(0x7F, { 0x00, 0x05 });
	k2->verify( 0, { 1 } );
	k2->verify(0x600, { 0x01 });
	k2->verify(0x601, { 0xE0 });
}

BOOST_AUTO_TEST_CASE( memcjr_quadra605_33Mhz )  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[12]);
	machine->model_id = 0xa55a2226;
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xA5 });
	k1->verify(0x31, { 0x5A });
	k1->verify(0x32, { 0x00 });
	k1->verify(0x33, { 0xDC });
	k1->verify(0x34, { 0xA5 });
	k1->verify(0x35, { 0x5A });
	k1->verify(0x36, { 0x01 });
	k1->verify(0x37, { 0xE7 });
	k1->verify(0x7C, { 0x72, 0x9E });
	k1->verify(0x7D, { 0x95, 0x95 });
	k1->verify(0x7E, { 0x68, 0x68 });
	k1->verify(0x7F, { 0x03, 0x07 });
	k2->verify( 0, { 0 } );
	k2->verify(0x600, { 0x00 });
	k2->verify(0x601, { 0xD5 });
}

BOOST_AUTO_TEST_CASE( memcjr_LC630_2)  {
	prepare(MB_TYPE::MEMCjr, &motherboards[10], &new_machines[18]);
	machine->model_id = 0xa55a2253;
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xA5 });
	k1->verify(0x31, { 0x5A });
	k1->verify(0x32, { 0x02 });
	k1->verify(0x33, { 0xDC });
	k1->verify(0x34, { 0xA5 });
	k1->verify(0x35, { 0x5A });
	k1->verify(0x36, { 0x02 });
	k1->verify(0x37, { 0x55 });
	k1->verify(0x7C, { 0x72, 0x56 });
	k1->verify(0x7D, { 0x95, 0x95 });
	k1->verify(0x7E, { 0x68, 0x68 });
	k1->verify(0x7F, { 0x0B, 0x09 });
	k2->verify( 0, { 0 } );
	k2->verify(0x500, { 0x90 });
	k2->verify(0x501, { 0xB7 });
	k2->verify(0x600, { 0x00 });
	k2->verify(0x601, { 0x00 });
}

BOOST_AUTO_TEST_CASE( memcjr_unreleased)  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[15]);
	machine->model_id = 0xa55a2231;
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data( 0, { 1 } );
	k2->set_read_data( 0x100, { 0 } );
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k2->verify( 0, { 1, 0x11 } );
	k2->verify( 0x100, { 0x08 } );
	k2->verify( 0x600, { 0x01 } );
	k2->verify( 0x601, { 0xE0 } );
}

BOOST_AUTO_TEST_CASE( memcjr_centris610)  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[15]);
	machine->model_id = 0xa55a2bad;
	auto via = std::make_shared<IO_TEST<VIA1>>();
	via->set_read_data(15, { 0x40 });
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data(0, { 0xab } );
	k2->set_read_data(1, { 0xcd } );
	k2->set_read_data(2, { 0xef } );
	k2->set_read_data(3, { 0x98 } );
	machine->via1 = via;
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xa5 });
	k1->verify(0x31, { 0x5a });
	k1->verify(0x32, { 0x01 });
	k1->verify(0x33, { 0x01 });
	k1->verify(0x34, { 0xa5 });
	k1->verify(0x35, { 0x5a });
	k1->verify(0x36, { 0x01 });
	k1->verify(0x37, { 0x1D });
	k2->verify(0x0, { 0xab });
	k2->verify(0x1, { 0xcd });
	k2->verify(0x2, { 0xef });
	k2->verify(0x3, { 0x91 });
	k2->verify(0x600, { 0xab });
	k2->verify(0x601, { 0xcd });
	k2->verify(0x602, { 0x02 });
	k2->verify(0x603, { 0x80 });
}

BOOST_AUTO_TEST_CASE( memcjr_quadra610)  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[15]);
	machine->model_id = 0xa55a2bad;
	auto via = std::make_shared<IO_TEST<VIA1>>();
	via->set_read_data(15, { 0x4 });
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data(0, { 0xab } );
	k2->set_read_data(1, { 0xcd } );
	k2->set_read_data(2, { 0xef } );
	k2->set_read_data(3, { 0x98 } );
	machine->via1 = via;
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xa5 });
	k1->verify(0x31, { 0x5a });
	k1->verify(0x32, { 0x00 });
	k1->verify(0x33, { 0x1a });
	k1->verify(0x34, { 0xa5 });
	k1->verify(0x35, { 0x5a });
	k1->verify(0x36, { 0x01 });
	k1->verify(0x37, { 0x6b });
	k2->verify(0x0, { 0xab });
	k2->verify(0x1, { 0xcd });
	k2->verify(0x2, { 0xef });
	k2->verify(0x3, { 0x91 });
	k2->verify(0x600, { 0xab });
	k2->verify(0x601, { 0xcd });
	k2->verify(0x602, { 0x01 });
	k2->verify(0x603, { 0xe0 });
}

BOOST_AUTO_TEST_CASE( memcjr_quadra650)  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[15]);
	machine->model_id = 0xa55a2bad;
	auto via = std::make_shared<IO_TEST<VIA1>>();
	via->set_read_data(15, { 0x10 } );
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data(0, { 0xab } );
	k2->set_read_data(1, { 0xcd } );
	k2->set_read_data(2, { 0xef } );
	k2->set_read_data(3, { 0x98 } );
	machine->via1 = via;
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xa5 });
	k1->verify(0x31, { 0x5a });
	k1->verify(0x32, { 0x00 });
	k1->verify(0x33, { 0xa3 });
	k1->verify(0x34, { 0xa5 });
	k1->verify(0x35, { 0x5a });
	k1->verify(0x36, { 0x01 });
	k1->verify(0x37, { 0xe7 });
	k2->verify(0x0, { 0xab });
	k2->verify(0x1, { 0xcd });
	k2->verify(0x2, { 0xef });
	k2->verify(0x3, { 0x90 });
	k2->verify(0x600, { 0xab });
	k2->verify(0x601, { 0xcd });
	k2->verify(0x602, { 0x00 });
	k2->verify(0x603, { 0xd5 });
}

BOOST_AUTO_TEST_CASE( memcjr_unknown_quadra)  {
	prepare(MB_TYPE::MEMCjr, &motherboards[9], &new_machines[15]);
	machine->model_id = 0xa55a2bad;
	auto via = std::make_shared<IO_TEST<VIA1>>();
	via->set_read_data( 15, { 0x14 } );
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data(0, { 0xab } );
	k2->set_read_data(1, { 0xcd } );
	k2->set_read_data(2, { 0xef } );
	k2->set_read_data(3, { 0x98 } );
	machine->via1 = via;
	machine->reg1 = k1;
	machine->reg2 = k2;
	TEST_ROM( 04866 );
	k1->verify(0x30, { 0xa5 });
	k1->verify(0x31, { 0x5a });
	k1->verify(0x32, { 0x02 });
	k1->verify(0x33, { 0xf4 });
	k1->verify(0x34, { 0xa5 });
	k1->verify(0x35, { 0x5a });
	k1->verify(0x36, { 0x02 });
	k1->verify(0x37, { 0x55 });
	k2->verify(0x0, { 0xab });
	k2->verify(0x1, { 0xcd });
	k2->verify(0x2, { 0xef });
	k2->verify(0x3, { 0x90 });
	k2->verify(0x600, { 0xab });
	k2->verify(0x601, { 0xcd });
	k2->verify(0x602, { 0x00 });
	k2->verify(0x603, { 0x00 });
}


