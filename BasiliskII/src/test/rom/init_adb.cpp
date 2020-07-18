#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

#include "glu.hpp"
#include "via.hpp"
#include "via1.hpp"
//#include "adb.hpp"
namespace bdata = boost::unit_test::data;

namespace ROM {
void getHWInfo(uint8_t);
}

void prepare(std::unique_ptr<Machine>&& m) {
	fixture f(std::move(m));
	ROMWrapper::run_02F64();
	ROMWrapper::run_02E8C();
	clear_global();
}
using namespace ROM;
BOOST_AUTO_TEST_CASE( exec ) {
	prepare(std::make_unique<GLU>(GLU::MODEL::IIx));	
	test_rom( 0x40885714 );
}


