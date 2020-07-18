#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via_test.hpp"
#include "msc_test.hpp"
#include <memory>
using namespace ROM;
static bool fail;
namespace ROM {
void sad_mac( uint32_t) { fail = true; }
}
template<class T, class U>
void prepare(U e) {
	fixture f(std::make_unique<T>(e));
	
	fail = false;
	DEFINE_ROM( 02F64 );
	DEFINE_ROM( 47840 );
	DEFINE_ROM( 478C2 );
	DEFINE_ROM( 47928 );
	DEFINE_ROM( 4AA98 );

}
class MSC_TEST : public PB_EX_REG {
    int called = 0;
	const int fail;
public:
	MSC_TEST(int f = -1) :fail(f) {}
	std::vector<uint8_t> do_cmd(uint8_t c) override {
		if( c == 0xE2 && fail == called++ ) {
			return {};
		}
		return PB_EX_REG::do_cmd(c);
	}
};
BOOST_AUTO_TEST_CASE( non_duo ) {
	prepare<JAWS>(true);
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_02F64();
	TEST_ROM( B29C4 );
}

BOOST_AUTO_TEST_CASE( success ) {
	prepare<MSC>(MSC::MODEL::PB_Duo270c);
	DR(2) = 0;
	ROMWrapper::run_02F64();
	TEST_ROM( B29C4 );
	BOOST_TEST( !fail );
}


BOOST_AUTO_TEST_CASE( failure ) {
	prepare<MSC>(MSC::MODEL::PB_Duo270c);
	machine->pb_ex = std::make_shared<MSC_TEST>(1);
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	TEST_ROM( B29C4 );
	BOOST_TEST( fail );
}

