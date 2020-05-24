#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "asc.hpp"
#include <memory>
#include <stdio.h>
#include <boost/test/tools/output_test_stream.hpp>
using boost::test_tools::output_test_stream;
class XASC : public ASC {
	output_test_stream output;
public:
	XASC() :ASC(1), output("bin/easc_beep_expected.bin", true, false) {}
	void write(int base, uint8_t w) override {
		if( base == 0 ) {
			output.put(w);
		} else if( base == 0x400 ) {
		} else {
			ASC::write(base, w);
		}
	}
	void expect() {output.match_pattern(); }
};
namespace ROM {
int check_easc_fifo_empty() { return MOCK::invoke<int>("check_easc_fifo_empty"); }
}
using namespace ROM;
void prepare() {
	static int n = 0;
	fixture f(MB_TYPE::SONORA);
	MOCK::make<int()>("check_easc_fifo_empty")
		->always_do( []() { return (++n) % 400 == 0 ? 1 : 0; } );
	DEFINE_ROM( BD3EC );
}
BOOST_AUTO_TEST_CASE( ring ) {
	prepare();
	AR(6) = 0x50F14000;
	auto ut_asc = std::make_shared<XASC>();
	machine->asc = ut_asc;
	DR(5) = 0x4000;
	DR(2) = 0xA1458000;
	DR(4) = 0x0000BFC9;
	DR(3) = 0;
	TEST_ROM( 45EA2 );
	ut_asc->expect();
}

