#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via.hpp"
#include "glu.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
#include "mdu.hpp"
#include "msc.hpp"
#include "v8.hpp"
#include "oss.hpp"
#include "mcu.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "sonora.hpp"
#include "ardbeg.hpp"
#include "memcjr.hpp"
namespace bdata = boost::unit_test::data;

using namespace ROM;
template<class T, class U>
void init(U p) {
	fixture f(std::make_unique<T>(p));
	MOCK::reset_all();
	clear_global();
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	ROMWrapper::run_02F64();
}



BOOST_AUTO_TEST_CASE( glue )  {
	init<GLU>(GLU::MODEL::IIx);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );
	via1->watch( VIA_REG::IER );

	via2->watch( VIA_REG::RA );
	via2->watch( VIA_REG::DDRA );
	via2->watch( VIA_REG::RB );
	via2->watch( VIA_REG::DDRB );
	via2->watch( VIA_REG::PCR );
	via2->watch( VIA_REG::ACR );
	via2->watch( VIA_REG::IER );
	via2->watch( VIA_REG::TIMER1_L );
	via2->watch( VIA_REG::TIMER1_H );
	via2->set_read_data( VIA_REG::RA, { 0x80 } );
	machine->via1 = via1;
	machine->via2 = via2;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 1 } );
	via1->verify( VIA_REG::DDRA, { 0x3f } );
	via1->verify( VIA_REG::RB, { 0x37 } );
	via1->verify( VIA_REG::DDRB, { 0xb7 });
	via1->verify( VIA_REG::PCR, { 0 });
	via1->verify( VIA_REG::ACR, { 0 });
	via1->verify( VIA_REG::IER, { 0x7f });

	via2->verify(VIA_REG::RA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0xc0 });
	via2->verify(VIA_REG::RB, { 0x07 });
	via2->verify(VIA_REG::DDRB, { 0x80 });
	via2->verify(VIA_REG::PCR, { 0x60 });
	via2->verify(VIA_REG::ACR, { 0xc0 });
	via2->verify(VIA_REG::IER, { 0x7f });
	via2->verify(VIA_REG::TIMER1_L, { 0x6e });
	via2->verify(VIA_REG::TIMER1_H, { 0x19 });
		
}

BOOST_AUTO_TEST_CASE( mdu )  {
	init<MDU>(MDU::MODEL::UNKNOWN);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	via2->watch( VIA_REG::RA );
	via2->watch( VIA_REG::DDRA );
	via2->watch( VIA_REG::RB );
	via2->watch( VIA_REG::DDRB );
	via2->watch( VIA_REG::PCR );
	via2->watch( VIA_REG::ACR );
	via2->set_read_data( VIA_REG::RA, { 0x80 } );
	machine->via1 = via1;
	machine->via2 = via2;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x21 } );
	via1->verify( VIA_REG::DDRA, { 0x2f } );
	via1->verify( VIA_REG::RB, { 0xf7 } );
	via1->verify( VIA_REG::DDRB, { 0xf7 });
	via1->verify( VIA_REG::PCR, { 0 });
	via1->verify( VIA_REG::ACR, { 0 });
	
	via2->verify(VIA_REG::RA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0xc0 });
	via2->verify(VIA_REG::RB, { 0x0f });
	via2->verify(VIA_REG::DDRB, { 0x89 });
	via2->verify(VIA_REG::PCR, { 0x00 });
	via2->verify(VIA_REG::ACR, { 0xc0 });
		
}

BOOST_AUTO_TEST_CASE( v8 )  {
	init<V8>(V8::MODEL::LC);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->watch(RBV_REG::IER);
	rbv->watch(RBV_REG::RB);
	rbv->watch(RBV_REG::MONP);
	rbv->watch(RBV_REG::SIER);

	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	machine->rbv = rbv;
	machine->via1 = via1;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x26 } );
	via1->verify( VIA_REG::DDRA, { 0x2f } );
	via1->verify( VIA_REG::RB, { 0x48 } );
	via1->verify( VIA_REG::DDRB, { 0x30 });
	via1->verify( VIA_REG::PCR, { 0 });
	via1->verify( VIA_REG::ACR, { 0x1c });

	rbv->verify(RBV_REG::IER, { 0x7f });
	rbv->verify(RBV_REG::RB, { 0x8f });
	rbv->verify(RBV_REG::MONP, { 0x40 });
	rbv->verify(RBV_REG::SIER, { 0xff });
	
		
}

BOOST_AUTO_TEST_CASE( oss )  {
	init<OSS>(true);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	machine->via1 = via1;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x01 } );
	via1->verify( VIA_REG::DDRA, { 0x07 } );
	via1->verify( VIA_REG::RB, { 0x87 } );
	via1->verify( VIA_REG::DDRB, { 0x87 });
	via1->verify( VIA_REG::PCR, { 0 });
	via1->verify( VIA_REG::ACR, { 0 });
	
		
}

BOOST_AUTO_TEST_CASE( mcu_iop )  {
	init<MCU>(MCU::MODEL::Q900);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	via2->watch( VIA_REG::RA );
	via2->watch( VIA_REG::DDRA );
	via2->watch( VIA_REG::RB );
	via2->watch( VIA_REG::DDRB );
	via2->watch( VIA_REG::PCR );
	via2->watch( VIA_REG::ACR );
	via2->set_read_data( VIA_REG::RA, { 0x80 } );
	machine->via1 = via1;
	machine->via2 = via2;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0 } );
	via1->verify( VIA_REG::DDRA, { 0 } );
	via1->verify( VIA_REG::RB, { 0xc9 } );
	via1->verify( VIA_REG::DDRB, { 0xf1 });
	via1->verify( VIA_REG::PCR, { 0x22 });
	via1->verify( VIA_REG::ACR, { 0x1c });

	via2->verify(VIA_REG::RA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0x80 });
	via2->verify(VIA_REG::RB, { 0x03 });
	via2->verify(VIA_REG::DDRB, { 0xc8 });
	via2->verify(VIA_REG::PCR, { 0x22 });
	via2->verify(VIA_REG::ACR, { 0xc0 });
		
}

BOOST_AUTO_TEST_CASE( mcu_non_iop )  {
	init<MCU>(MCU::MODEL::Q700);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	via2->watch( VIA_REG::RA );
	via2->watch( VIA_REG::DDRA );
	via2->watch( VIA_REG::RB );
	via2->watch( VIA_REG::DDRB );
	via2->watch( VIA_REG::PCR );
	via2->watch( VIA_REG::ACR );
	via2->watch( VIA_REG::IER );
	via2->set_read_data( VIA_REG::RA, { 0x80 } );
	machine->via1 = via1;
	machine->via2 = via2;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x20 } );
	via1->verify( VIA_REG::DDRA, { 0x28 } );
	via1->verify( VIA_REG::RB, { 0xf7 } );
	via1->verify( VIA_REG::DDRB, { 0xf7 });
	via1->verify( VIA_REG::PCR, { 0x22 });
	via1->verify( VIA_REG::ACR, { 0x00 });

	via2->verify(VIA_REG::RA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0x80 });
	via2->verify(VIA_REG::RB, { 0x06 });
	via2->verify(VIA_REG::DDRB, { 0x99 });
	via2->verify(VIA_REG::PCR, { 0x22 });
	via2->verify(VIA_REG::ACR, { 0xc0 });
	via2->verify(VIA_REG::IER, { 0x7f });
		
}

BOOST_AUTO_TEST_CASE( jaws )  {
	init<JAWS>(true);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	via2->watch( VIA_REG::RA );
	via2->watch( VIA_REG::DDRA );
	via2->watch( VIA_REG::RB );
	via2->watch( VIA_REG::DDRB );
	via2->watch( VIA_REG::PCR );
	via2->watch( VIA_REG::ACR );
	via2->watch( VIA_REG::IER );
	via2->set_read_data( VIA_REG::RA, { 0x80 } );
	machine->via1 = via1;
	machine->via2 = via2;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x21 } );
	via1->verify( VIA_REG::DDRA, { 0x28 } );
	via1->verify( VIA_REG::RB, { 0x87 } );
	via1->verify( VIA_REG::DDRB, { 0x87 });
	via1->verify( VIA_REG::PCR, { 0x22 });
	via1->verify( VIA_REG::ACR, { 0x00 });

	via2->verify(VIA_REG::RA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0x00 });
	via2->verify(VIA_REG::RB, { 0x66 });
	via2->verify(VIA_REG::DDRB, { 0xBD });
	via2->verify(VIA_REG::PCR, { 0x22 });
	via2->verify(VIA_REG::ACR, { 0x00 });
	via2->verify(VIA_REG::IER, { 0x7f });
		
}



BOOST_DATA_TEST_CASE( msc, bdata::xrange(2), color )  {
	init<MSC>(color ? MSC::MODEL::PB_Duo270c : MSC::MODEL::PB_Duo210);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto rbv = std::make_shared<IO_TEST<MSCRbv>>(nullptr);
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	rbv->watch(RBV_REG::IER);
	rbv->watch(RBV_REG::RB);
	rbv->watch(RBV_REG::SIER);
	rbv->set_read_data( RBV_REG::RB, { 0x8f } );

	machine->via1 = via1;
	machine->rbv = rbv;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x00 } );
	via1->verify( VIA_REG::DDRA, { 0x00 } );
	via1->verify( VIA_REG::RB, { 0x00 } );
	via1->verify( VIA_REG::DDRB, { 0x00 });
	via1->verify( VIA_REG::PCR, { 0x02 });
	via1->verify( VIA_REG::ACR, { 0x1c });

	rbv->verify(RBV_REG::IER, { 0x7f });
	if( ! color ) {
		rbv->verify(RBV_REG::RB, { 0x8f, 0x8e });
	} else {
		rbv->verify(RBV_REG::RB, { 0x8f });
	}
	rbv->verify(RBV_REG::SIER, { 0x7f });
		
}

BOOST_AUTO_TEST_CASE( sonora )  {
	init<Sonora>(Sonora::MODEL::LCIII_p);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->watch(RBV_REG::IER);
	rbv->watch(RBV_REG::RB);
	rbv->watch(RBV_REG::MONP);
	rbv->watch(RBV_REG::SIER);

	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	machine->rbv = rbv;
	machine->via1 = via1;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x20 } );
	via1->verify( VIA_REG::DDRA, { 0x28 } );
	via1->verify( VIA_REG::RB, { 0x88 } );
	via1->verify( VIA_REG::DDRB, { 0xB0 });
	via1->verify( VIA_REG::PCR, { 0x20 });
	via1->verify( VIA_REG::ACR, { 0x1c });

	rbv->verify(RBV_REG::IER, { 0x7f });
	rbv->verify(RBV_REG::RB, { 0x8f });
	rbv->verify(RBV_REG::MONP, { 0x40 });
	rbv->verify(RBV_REG::SIER, { 0xff });
	
		
}

BOOST_AUTO_TEST_CASE( ardbeg )  {
	init<Ardbeg>(Ardbeg::MODEL::LC550);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->watch(RBV_REG::IER);
	rbv->watch(RBV_REG::RB);
	rbv->watch(RBV_REG::MONP);
	rbv->watch(RBV_REG::SIER);

	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	machine->rbv = rbv;
	machine->via1 = via1;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x20 } );
	via1->verify( VIA_REG::DDRA, { 0x28 } );
	via1->verify( VIA_REG::RB, { 0xB8 } );
	via1->verify( VIA_REG::DDRB, { 0xB0 });
	via1->verify( VIA_REG::PCR, { 0x00 });
	via1->verify( VIA_REG::ACR, { 0x1c });

	rbv->verify(RBV_REG::IER, { 0x7f });
	rbv->verify(RBV_REG::RB, { 0x8f });
	rbv->verify(RBV_REG::MONP, { 0x40 });
	rbv->verify(RBV_REG::SIER, { 0xff });
	
		
}

BOOST_AUTO_TEST_CASE( primetime )  {
	init<MEMCjr>(MEMCjr::MODEL::Q605_33);
	auto via1 = std::make_shared<IO_TEST<VIA1>>();
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via1->watch( VIA_REG::RA );
	via1->watch( VIA_REG::DDRA );
	via1->watch( VIA_REG::RB );
	via1->watch( VIA_REG::DDRB );
	via1->watch( VIA_REG::PCR );
	via1->watch( VIA_REG::ACR );

	via2->watch( VIA_REG::RA );
	via2->watch( VIA_REG::DDRA );
	via2->watch( VIA_REG::RB );
	via2->watch( VIA_REG::DDRB );
	via2->watch( VIA_REG::PCR );
	via2->watch( VIA_REG::ACR );
	via2->set_read_data( VIA_REG::RA, { 0x80 } );
	machine->via1 = via1;
	machine->via2 = via2;

	TEST_ROM( 02E8C );
	via1->verify( VIA_REG::RA, { 0x20 } );
	via1->verify( VIA_REG::DDRA, { 0x28 } );
	via1->verify( VIA_REG::RB, { 0xf8 } );
	via1->verify( VIA_REG::DDRB, { 0xf0 });
	via1->verify( VIA_REG::PCR, { 0 });
	via1->verify( VIA_REG::ACR, { 0x1c });

	via2->verify(VIA_REG::RA, { 0x80 });
	via2->verify(VIA_REG::DDRA, { 0x00 });
	via2->verify(VIA_REG::RB, { 0x0e });
	via2->verify(VIA_REG::DDRB, { 0xff });
	via2->verify(VIA_REG::PCR, { 0x26 });
	via2->verify(VIA_REG::ACR, { 0xc0 });
		
}
