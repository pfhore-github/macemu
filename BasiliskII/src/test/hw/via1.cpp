#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "test_common.hpp"
#include <atomic>
#include "via.hpp"
#include "via1.hpp"
#include "rtc.hpp"
#include "adb.h"
#include "glu.hpp"
#include "machine.hpp"
struct ADB_MOCK : public ADBDevice {
	using ADBDevice::ADBDevice;
	std::array<uint16_t, 3> listens;
	std::array<std::vector<uint8_t>, 3> talks;
	bool reset_ = false;
	bool flush_ = false;
	std::vector<uint8_t> talk0() { return talks[0];}
	std::vector<uint8_t> talk1() { return talks[1];}
	std::vector<uint8_t> talk2() { return talks[2];}
	void listen0(uint16_t v) { listens[0] = v;}
	void listen1(uint16_t v) { listens[1] = v;}
	void listen2(uint16_t v) { listens[2] = v;}
	void reset() { reset_ = true; }
	void flush() { flush_ = true; }
};

namespace bdata = boost::unit_test::data;
VIA1 tester(1);
BOOST_DATA_TEST_CASE( regA, bdata::xrange(8), x )  {
	machine = std::make_unique<GLU>(GLU::MODEL::IIx);
	machine->model_map[0] = (x) & 1;
	machine->model_map[1] = (x >> 1) & 1;
	machine->model_map[2] = (x >> 2) & 1;
	machine->model_map[3] = (x >> 3) & 1;
	uint8_t v = tester.read( 15  );
	BOOST_TEST( (v >> 1 & 1) == (x & 1) ); 
	BOOST_TEST( (v >> 2 & 1) == (x >> 1& 1) ); 
	BOOST_TEST( (v >> 4 & 1) == (x >> 2& 1) ); 
	BOOST_TEST( (v >> 6 & 1) == (x >> 3& 1) ); 
}

BOOST_AUTO_TEST_SUITE( adb )
BOOST_DATA_TEST_CASE( reset, bdata::xrange(8), port )  {
	machine = std::make_unique<GLU>(GLU::MODEL::IIx);
	auto adb_mock = std::make_shared<ADB_MOCK>(machine->via1->adb.get(), 8 + port );
	machine->via1->adb->devices[ 8 + port ] = adb_mock;
	machine->via1->write(VIA_REG::ACR, 0x1C);
	machine->via1->write(VIA_REG::SR, 0 );
	machine->via1->write(VIA_REG::DDRB, 0x30 );
	machine->via1->write(VIA_REG::RB, 0 );
	BOOST_CHECK( adb_mock->reset_ );
}

BOOST_DATA_TEST_CASE( flush, bdata::xrange(8), port )  {
	machine = std::make_unique<GLU>(GLU::MODEL::IIx);
	auto adb_mock = std::make_shared<ADB_MOCK>(machine->via1->adb.get(), 8 + port );
	machine->via1->adb->devices[ 8 + port ] = adb_mock;
	machine->via1->write(VIA_REG::ACR, 0x1C);
	machine->via1->write(VIA_REG::SR, 0x81 | port << 4 );
	machine->via1->write(VIA_REG::DDRB, 0x30 );
	machine->via1->write(VIA_REG::RB, 0 );
	BOOST_CHECK( adb_mock->flush_ );
}

BOOST_DATA_TEST_CASE( talk, bdata::xrange(8) * bdata::xrange(3), port, n )  {
	machine = std::make_unique<GLU>(GLU::MODEL::IIx);
	auto adb_mock = std::make_shared<ADB_MOCK>(machine->via1->adb.get(), 8 + port );
	adb_mock->talks[n] = { 1, 2, 3 };
	machine->via1->adb->devices[ 8 + port ] = adb_mock;
	
	machine->via1->write(VIA_REG::ACR, 0x1C);
	machine->via1->write(VIA_REG::SR, 0x8C | port << 4 | n );
	machine->via1->write(VIA_REG::DDRB, 0x30 );
	machine->via1->write(VIA_REG::RB, 0 );
	machine->via1->write(VIA_REG::ACR, 0xC);
	machine->via1->write(VIA_REG::RB, 0x10 );
	BOOST_CHECK_EQUAL( machine->via1->read(VIA_REG::SR), 3 );
	machine->via1->write(VIA_REG::RB, 0x20 );
	BOOST_CHECK_EQUAL( machine->via1->read(VIA_REG::SR), 1 );
	machine->via1->write(VIA_REG::RB, 0x10 );
	BOOST_CHECK_EQUAL( machine->via1->read(VIA_REG::SR), 2 );
	machine->via1->write(VIA_REG::RB, 0x20 );
	BOOST_CHECK_EQUAL( machine->via1->read(VIA_REG::SR), 3 );
}

BOOST_DATA_TEST_CASE( listen, bdata::xrange(8) * bdata::xrange(3), port, n )  {
	machine = std::make_unique<GLU>(GLU::MODEL::IIx);
	auto adb_mock = std::make_shared<ADB_MOCK>(machine->via1->adb.get(), 8 + port );
	machine->via1->adb->devices[ 8 + port ] = adb_mock;
	
	machine->via1->write(VIA_REG::ACR, 0x1C);
	machine->via1->write(VIA_REG::SR, 0x88 | port << 4 | n );
	machine->via1->write(VIA_REG::DDRB, 0x30 );
	machine->via1->write(VIA_REG::RB, 0 );
	machine->via1->write(VIA_REG::SR, 0xDE );
	machine->via1->write(VIA_REG::RB, 0x10 );
	machine->via1->write(VIA_REG::SR, 0xAD );
	machine->via1->write(VIA_REG::RB, 0x20 );
	BOOST_CHECK_EQUAL( adb_mock->listens[n], 0xDEAD );
}

BOOST_AUTO_TEST_SUITE_END(  );
