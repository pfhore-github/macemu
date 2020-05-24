#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(move16, fixture);
BOOST_AUTO_TEST_CASE( inc_inc ) {
	cpu.R[8+1] = 0x1000;
	cpu.R[8+2] = 0x2000;
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0x9ABCDEF0);
	test_write32(0x1008, 0xFEDCBA98);
	test_write32(0x100C, 0x76543210);
	test_cpu( { 0173041, 0120000 } );
	BOOST_TEST( test_read32(0x2000) == 0x12345678 );
	BOOST_TEST( test_read32(0x2004) == 0x9ABCDEF0 );
	BOOST_TEST( test_read32(0x2008) == 0xFEDCBA98 );
	BOOST_TEST( test_read32(0x200C) == 0x76543210 );
	BOOST_TEST( cpu.R[8+1] == 0x1010 );
	BOOST_TEST( cpu.R[8+2] == 0x2010 );
}

BOOST_AUTO_TEST_CASE( imm_reg ) {
	cpu.R[8+2] = 0x2000;
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0x9ABCDEF0);
	test_write32(0x1008, 0xFEDCBA98);
	test_write32(0x100C, 0x76543210);
	test_cpu( { 0173032, 0, 0x1000 } );
	BOOST_TEST( test_read32(0x2000) == 0x12345678 );
	BOOST_TEST( test_read32(0x2004) == 0x9ABCDEF0 );
	BOOST_TEST( test_read32(0x2008) == 0xFEDCBA98 );
	BOOST_TEST( test_read32(0x200C) == 0x76543210 );
}

BOOST_AUTO_TEST_CASE( imm_inc ) {
	cpu.R[8+2] = 0x2000;
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0x9ABCDEF0);
	test_write32(0x1008, 0xFEDCBA98);
	test_write32(0x100C, 0x76543210);
	test_cpu( { 0173012, 0, 0x1000 } );
	BOOST_TEST( test_read32(0x2000) == 0x12345678 );
	BOOST_TEST( test_read32(0x2004) == 0x9ABCDEF0 );
	BOOST_TEST( test_read32(0x2008) == 0xFEDCBA98 );
	BOOST_TEST( test_read32(0x200C) == 0x76543210 );
	BOOST_TEST( cpu.R[8+2] == 0x2010 );
}

BOOST_AUTO_TEST_CASE( reg_imm ) {
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0x9ABCDEF0);
	test_write32(0x1008, 0xFEDCBA98);
	test_write32(0x100C, 0x76543210);
	test_cpu( { 0173022, 0, 0x2000 } );
	BOOST_TEST( test_read32(0x2000) == 0x12345678 );
	BOOST_TEST( test_read32(0x2004) == 0x9ABCDEF0 );
	BOOST_TEST( test_read32(0x2008) == 0xFEDCBA98 );
	BOOST_TEST( test_read32(0x200C) == 0x76543210 );
}

BOOST_AUTO_TEST_CASE( inc_imm ) {
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0x9ABCDEF0);
	test_write32(0x1008, 0xFEDCBA98);
	test_write32(0x100C, 0x76543210);
	test_cpu( { 0173002, 0, 0x2000 } );
	BOOST_TEST( test_read32(0x2000) == 0x12345678 );
	BOOST_TEST( test_read32(0x2004) == 0x9ABCDEF0 );
	BOOST_TEST( test_read32(0x2008) == 0xFEDCBA98 );
	BOOST_TEST( test_read32(0x200C) == 0x76543210 );
	BOOST_TEST( cpu.R[8+2] == 0x1010 );
}

BOOST_AUTO_TEST_SUITE_END();
