#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
#include "machine.hpp"
#include "devices/serial.hpp"
namespace ROM {
}
struct TestDevice : public SerialDevice {
	uint8_t written = 0;
	void hsk_o(bool) override {}
	void t_xd(uint8_t v) override { written = v; }
	void abort() override {}
};
void dev_run(SCC& scc, TestDevice& dev) {
	scc.r_xd(dev, 0x15);
}

BOOST_AUTO_TEST_CASE( read_modem ) {
	auto scc = std::make_shared<SCC>( std::make_shared<Z8530>());
	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	dev_run(*scc, *dev);
	BOOST_TEST( scc->bit(SCC_REG::A_CMD, 0) );
	BOOST_TEST( scc->read(SCC_REG::A_DATA) == 0x15 );
}

BOOST_AUTO_TEST_CASE( write_modem ) {
	auto scc = std::make_shared<SCC>( std::make_shared<Z8530>());
	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	scc->write( SCC_REG::A_DATA, 0x23 );
	BOOST_TEST( dev->written == 0x23 );
	scc->write( SCC_REG::A_CMD, 1 );
	BOOST_TEST( scc->bit( SCC_REG::A_CMD, 0 ) );
}



