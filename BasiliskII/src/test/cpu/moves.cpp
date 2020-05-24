#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(moves_byte, fixture);
BOOST_AUTO_TEST_CASE( user ) {
	cpu.S = false;
	cpu.R[8+5] = 0x1000;
	test_cpu({ 0007020, 0 });
	test_in_exception(8) ;
}
BOOST_AUTO_TEST_CASE( dfc ) {
	cpu.S = true;
	cpu.R[8+5] = 0x3ffffff0;
	cpu.DFC = 3;
	cpu.R[2] = 34;
	test_cpu({ 0007025, 0024000 });
	test_in_exception(2) ;
	uint16_t v = test_read16(cpu.R[15]+0xc);
	BOOST_TEST( (v & 0x17f) == ((1 << 8) | (ATTR_SZ_B << 5) | (2 << 3 | 3)));	
}

BOOST_DATA_TEST_CASE( from_dn, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	cpu.R[2] = 11;
	test_ea_b(cpu, { 0007000, 0024000 }, 0, ea, 11 );
}

BOOST_AUTO_TEST_CASE( sfc ) {
	cpu.S = true;
	cpu.R[8+5] = 0x3ffffff0;
	cpu.SFC = 3;
	test_cpu({ 0007025, 0020000 });
	test_in_exception(2) ;
	uint16_t v = test_read16(cpu.R[15]+0xc);
	BOOST_TEST( (v & 0x17f) == ((ATTR_SZ_B << 5) | (2 << 3 | 3)));	
}

BOOST_DATA_TEST_CASE( to_dn, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	test_ea_b(cpu, { 0007000, 0020000 }, 13, ea, 13 );
	BOOST_TEST( cpu.R[2] == 13 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(moves_word, fixture);
BOOST_AUTO_TEST_CASE( user ) {
	cpu.S = false;
	cpu.R[8+5] = 0x1000;
	test_cpu({ 0007120, 0 });
	test_in_exception(8) ;
}
BOOST_AUTO_TEST_CASE( dfc ) {
	cpu.S = true;
	cpu.R[8+5] = 0x3ffffff0;
	cpu.DFC = 3;
	cpu.R[2] = 3434;
	test_cpu({ 0007125, 0024000 });
	test_in_exception(2) ;
	uint16_t v = test_read16(cpu.R[15]+0xc);
	BOOST_TEST( (v & 0x17f) == ((1 << 8) | (ATTR_SZ_W << 5) | (2 << 3 | 3)));	
}

BOOST_DATA_TEST_CASE( from_dn, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	cpu.R[2] = 1111;
	test_ea_w(cpu, { 0007100, 0024000 }, 0, ea, 1111 );
}

BOOST_DATA_TEST_CASE( from_an, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	cpu.R[8+2] = 1111;
	test_ea_w(cpu, { 0007100, 0124000 }, 0, ea, 1111 );
}

BOOST_AUTO_TEST_CASE( sfc ) {
	cpu.S = true;
	cpu.R[8+5] = 0x3ffffff0;
	cpu.SFC = 3;
	test_cpu({ 0007125, 0020000 });
	test_in_exception(2) ;
	uint16_t v = test_read16(cpu.R[15]+0xc);
	BOOST_TEST( (v & 0x17f) == ((ATTR_SZ_W << 5) | (2 << 3 | 3)));	
}

BOOST_DATA_TEST_CASE( to_dn, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	test_ea_w(cpu, { 0007100, 0020000 }, 1300, ea, 1300 );
	BOOST_TEST( cpu.R[2] == 1300 );
}

BOOST_DATA_TEST_CASE( to_an, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	test_ea_w(cpu, { 0007100, 0120000 }, 1300, ea, 1300 );
	BOOST_TEST( cpu.R[8+2] == 1300 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(moves_long, fixture);
BOOST_AUTO_TEST_CASE( user ) {
	cpu.S = false;
	cpu.R[8+5] = 0x1000;
	test_cpu({ 0007220, 0 });
	test_in_exception(8) ;
}
BOOST_AUTO_TEST_CASE( dfc ) {
	cpu.S = true;
	cpu.R[8+5] = 0x3ffffff0;
	cpu.DFC = 3;
	cpu.R[2] = 343434;
	test_cpu({ 0007225, 0024000 });
	test_in_exception(2) ;
	uint16_t v = test_read16(cpu.R[15]+0xc);
	BOOST_TEST( (v & 0x17f) == ((1 << 8) | (ATTR_SZ_L << 5) | (2 << 3 | 3)));	
}

BOOST_DATA_TEST_CASE( from_dn, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	cpu.R[2] = 111111;
	test_ea_l(cpu, { 0007200, 0024000 }, 0, ea, 111111 );
}

BOOST_DATA_TEST_CASE( from_an, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	cpu.R[8+2] = 111111;
	test_ea_l(cpu, { 0007200, 0124000 }, 0, ea, 111111 );
}

BOOST_AUTO_TEST_CASE( sfc ) {
	cpu.S = true;
	cpu.R[8+5] = 0x3ffffff0;
	cpu.SFC = 3;
	test_cpu({ 0007225, 0020000 });
	test_in_exception(2) ;
	uint16_t v = test_read16(cpu.R[15]+0xc);
	BOOST_TEST( (v & 0x17f) == ((ATTR_SZ_L << 5) | (2 << 3 | 3)));	
}

BOOST_DATA_TEST_CASE( to_dn, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	test_ea_l(cpu, { 0007200, 0020000 }, 130000, ea, 130000 );
	BOOST_TEST( cpu.R[2] == 130000 );
}

BOOST_DATA_TEST_CASE( to_an, ea_write2, ea ) {
	cpu.S = true;
	cpu.R[8+5] = 0x1000;
	cpu.DFC = 3;
	test_ea_l(cpu, { 0007200, 0120000 }, 130000, ea, 130000 );
	BOOST_TEST( cpu.R[8+2] == 130000 );
}


BOOST_AUTO_TEST_SUITE_END();
