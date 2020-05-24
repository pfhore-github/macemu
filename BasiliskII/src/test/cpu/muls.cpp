#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


	

BOOST_FIXTURE_TEST_SUITE(muls_w, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2) * bdata::xrange(2),
					 a_sign, b_sign) {
	int16_t a = a_sign ? -1257 : 1257;
	int16_t b = b_sign ? -3339 : 3339;
	cpu.R[0] = a;
	cpu.R[1] = b;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0140701 } );
	uint32_t v = cpu.R[0];
	BOOST_TEST( (int32_t)v == (a * b) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == (a_sign != b_sign) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}


BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[1] = 3;
	test_cpu( { 0140701 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[2] = 10;
	test_ea_w(cpu, { 0142700 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[2] == 30 );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(muls_l, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2) * bdata::xrange(2),
					a_sign, b_sign) {
	int32_t a = (a_sign ? -1 : 1) * 1000;
	int32_t b = b_sign ? -5000 : 5000;
	cpu.R[1] = a;
	cpu.R[2] = b;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046002, 0014000  } );
	BOOST_TEST( (int32_t)(cpu.R[1]) == ( a * b ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == (a_sign != b_sign) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[1] = 0x70000000;
	cpu.R[2] = 0x60000000;
	test_cpu( { 0046002, 0014000  } );
	BOOST_TEST( cpu.V == true );
}


BOOST_AUTO_TEST_CASE(z) {
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046002, 0014000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[1] = 10;
	test_ea_l(cpu, { 0046000, 0014000 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[1] == 30 );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(muls_ll, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2) * bdata::xrange(2),
					a_sign, b_sign) {
	int64_t a = (a_sign ? -1 : 1) * 0x50000000ll;
	int32_t b = b_sign ? -14 : 14;
	cpu.R[0] = (uint32_t)a;
	cpu.R[2] = b;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046002, 0006001  } );
	int64_t v = a * b;
	uint32_t l = cpu.R[0];
	uint32_t h = cpu.R[1];
	BOOST_TEST( l == (v & 0xffffffff));
	BOOST_TEST( h == (uint32_t)(v >> 32) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == (a_sign != b_sign) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046002, 0016000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_SUITE_END();
