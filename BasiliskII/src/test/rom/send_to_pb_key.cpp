#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "rbv.hpp"
#include "msc.hpp"
#include "jaws.hpp"
#include "glu.hpp"
using namespace ROM;
namespace bdata = boost::unit_test::data;
namespace ROM {
bool send_to_pb_dword(uint32_t v) { return MOCK::invoke<bool>("send_to_pb_dword", v); }
}
void prepare(std::unique_ptr<Machine>&& m) {
	fixture f(std::move(m));
	MOCK::reset_all();
	MOCK::make<bool(uint32_t)>("send_to_pb_dword");
	DEFINE_ROM( 47840 );
}
BOOST_DATA_TEST_CASE( has_monitor, bdata::xrange(2), tf) {
	prepare( std::make_unique<MSC>() );
	AR(0) = 0x408A8234;
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->set_read_data(RBV_REG::PB_RBV_X33, { 0 } );
	rbv->set_read_data(0, { 0 } );
	machine->rbv = rbv;
	MOCK::get<bool(uint32_t)>("send_to_pb_dword")->always( tf, 0x00B30110 );
	TEST_ROM( 47826 );
	BOOST_CHECK( cpu.Z == tf);
}

BOOST_DATA_TEST_CASE( no_monitor, bdata::xrange(2), tf) {
	prepare( std::make_unique<GLU>()  );
	AR(0) = 0x408032D4;
	MOCK::get<bool(uint32_t)>("send_to_pb_dword")->always( tf, 0x00B30110 );
	TEST_ROM( 47826 );
	BOOST_CHECK( cpu.Z == tf);
}

