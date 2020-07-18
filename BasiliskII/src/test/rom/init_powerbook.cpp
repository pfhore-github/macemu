#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "glu.hpp"
#include "via_test.hpp"
#include "msc_test.hpp"
#include <memory>
using namespace ROM;
namespace ROM {
void init_pb_duo() {}
}
BOOST_AUTO_TEST_CASE( non_pb ) {
	clear_global();
	fixture f(std::make_unique<GLU>(GLU::MODEL::IIx));
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	TEST_ROM( 8AC00 );
}
class MSC_TEST : public PB_EX_REG {
	size_t cnt = 0;
	std::vector<uint8_t> result;
public:
	MSC_TEST(const std::vector<uint8_t>& v) :result(v) {}
	std::vector<uint8_t> do_cmd(uint8_t c) override {		
		if( c == 0x3A ) {
			if( cnt < result.size() ) {
				return { 0xff, result[cnt++] };
			} else {
				return { 0 };
			}
		}
		return PB_EX_REG::do_cmd(c);
	}
};
BOOST_AUTO_TEST_CASE( pb_msc_bpp_1 ) {
	clear_global();
	fixture f(std::make_unique<MSC>(MSC::MODEL::PB_Duo270c));
	machine->pb_ex = std::make_shared<MSC_TEST>( std::vector<uint8_t> { 0xA8, 0 } );
	auto pb_test = std::make_shared<IO_TEST<PB_REG>>();
	pb_test->set_read_data( 0x44, { 5 } );	
	machine->pb = pb_test;
	auto rbv_test = std::make_shared<IO_TEST<RBV>>();
	rbv_test->set_read_data( 4, {0xff});
	machine->rbv = rbv_test;
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	DEFINE_ROM( B29C4 );
	DEFINE_ROM( 02F64 );
	DEFINE_ROM( 47840 );
	DEFINE_ROM( 478C2 );
	DEFINE_ROM( 47928 );
	HWCFG_FLAGS.set( HWCFG_T::POWERBOOK );
	write_l(0xD18, 0x3000 );
	write_l(0x3112, 0 );
	if( rom ) {
		test_rom( 0x4088AC00, 0x4088AD4A );
	} else {
		init_powerbook();
	}
}

BOOST_AUTO_TEST_CASE( pb_has_lowmem ) {
	clear_global();
	fixture f(std::make_unique<MSC>(MSC::MODEL::PB_Duo270c));
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	DEFINE_ROM( B29C4 );
	DEFINE_ROM( 02F64 );
	DEFINE_ROM( 47840 );
	DEFINE_ROM( 47928 );

	rom_routines[ 0x4000 ] = []() { cpu.PC = AR(6); };
	HWCFG_FLAGS.set( HWCFG_T::POWERBOOK );
	write_l(0xD18, 0x3000 );
	write_l(0x3112, 0x53415241 );
	write_l(0x3012, 0x4000 );
	if( rom ) {
		test_rom( 0x4088AC00, 0x4088AD4A );
	} else {
		init_powerbook();
	}
}
