#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <atomic>
#include "via1.hpp"
#include "via.hpp"
#include "msc.hpp"
#include "powerbook.hpp"

namespace bdata = boost::unit_test::data;
struct PB_EX_REG_MOCK : public PB_EX_REG {
	std::unordered_map<uint8_t, std::vector<uint8_t>> mock;
	std::vector<uint8_t> do_cmd(uint8_t c) override { return mock[c]; }
	unsigned int arg_size(uint8_t c) override { return 0; }		
};
void prepare() {
	machine = std::make_unique<MSC>();
}

BOOST_AUTO_TEST_CASE( run )  {
	prepare();
	auto mock = std::make_shared<PB_EX_REG_MOCK>();
	mock->mock[ 0x12 ] = { 0x34 };
	machine->pb_ex = mock;
	machine->via1->set( VIA_REG::ACR, 4 ); // SR is shift out mode
	machine->via1->write( VIA_REG::SR, 0x12 );
	machine->rbv->clear( VIA_REG::RB, PB_TRANS_READ_MODE );
	bool ret = false;
	if( ! machine->rbv->bit( VIA_REG::RB, PB_TRANS_READY ) ) {
		machine->rbv->set( VIA_REG::RB, PB_TRANS_READ_MODE );
		if( machine->rbv->bit( VIA_REG::RB, PB_TRANS_READY ) ) {
			ret = true;
		}
	}
	BOOST_CHECK( ret );
}
