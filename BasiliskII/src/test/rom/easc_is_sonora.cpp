#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
void prepare(MB_TYPE m) {
	fixture f(m);
}
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE( BD43A, bdata::xrange(2), is_sonora ) {
	prepare( is_sonora ? MB_TYPE::SONORA : MB_TYPE::GLU );
	AR(6) = 0x50F14000;
	if( rom ) {
		test_rom( 0x408BD43A, 0x40845C7C );
	} else {
		ROMWrapper::run_BD43A();
	}
	BOOST_TEST( cpu.PC == 0x40845C7C );
	BOOST_TEST( cpu.Z == is_sonora );
}

BOOST_DATA_TEST_CASE( BD44C, bdata::xrange(2), is_sonora ) {
	prepare( is_sonora ? MB_TYPE::SONORA : MB_TYPE::GLU );
	AR(6) = 0x50F14000;
	if( rom ) {
		test_rom( 0x408BD44C, 0x40845CAE );
	} else {
		ROMWrapper::run_BD44C();
	}
	BOOST_TEST( cpu.PC == 0x40845CAE );
	BOOST_TEST( cpu.Z == is_sonora );
}

BOOST_DATA_TEST_CASE( BD45E, bdata::xrange(2), is_sonora ) {
	prepare( is_sonora ? MB_TYPE::SONORA : MB_TYPE::GLU );
	AR(6) = 0x50F14000;
	if( rom ) {
		test_rom( 0x408BD45E, 0x40845D36 );
	} else {
		ROMWrapper::run_BD45E();
	}
	BOOST_TEST( cpu.PC == 0x40845D36 );
	BOOST_TEST( cpu.Z == is_sonora );
}

BOOST_DATA_TEST_CASE( BD470, bdata::xrange(2), is_sonora ) {
	prepare( is_sonora ? MB_TYPE::SONORA : MB_TYPE::GLU );
	AR(6) = 0x50F14000;
	if( rom ) {
		test_rom( 0x408BD470, 0x40845E0E );
	} else {
		ROMWrapper::run_BD470();
	}
	BOOST_TEST( cpu.PC == 0x40845E0E );
	BOOST_TEST( cpu.Z == is_sonora );
}

BOOST_DATA_TEST_CASE( BD482, bdata::xrange(2), is_sonora ) {
	prepare( is_sonora ? MB_TYPE::SONORA : MB_TYPE::GLU );
	AR(6) = 0x50F14000;
	if( rom ) {
		test_rom( 0x408BD482, 0x40845E2C );
	} else {
		ROMWrapper::run_BD482();
	}
	BOOST_TEST( cpu.PC == 0x40845E2C );
	BOOST_TEST( cpu.Z == is_sonora );
}
