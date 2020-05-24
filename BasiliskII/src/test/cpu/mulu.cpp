#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


	

BOOST_FIXTURE_TEST_SUITE(mulu_w, fixture);
BOOST_AUTO_TEST_CASE(result) {
	uint16_t a = 1257;
	uint16_t b = 3339;
	cpu.R[0] = a;
	cpu.R[1] = b;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0140301 } );
	uint32_t v = cpu.R[0];
	BOOST_TEST( v == (a * b) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}


BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[1] = 3;
	test_cpu( { 0140301 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_DATA_TEST_CASE(n, bdata::xrange(2), n) {
	cpu.R[0] = 0xffff;
	cpu.R[1] = 0xffff * n;
	test_cpu( { 0140301 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[2] = 10;
	test_ea_w(cpu, { 0142300 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[2] == 30 );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(mulu_l, fixture);
BOOST_AUTO_TEST_CASE(result) {
	uint32_t a = 1000;
	uint32_t b = 5000;
	cpu.R[1] = a;
	cpu.R[2] = b;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046002, 0010000  } );
	BOOST_TEST( cpu.R[1] == ( a * b ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[1] = 0x70000000;
	cpu.R[2] = 0x60000000;
	test_cpu( { 0046002, 0010000  } );
	BOOST_TEST( cpu.V == true );
}


BOOST_AUTO_TEST_CASE(z) {
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046002, 0010000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_DATA_TEST_CASE(n, bdata::xrange(2), n) {
	cpu.R[1] = 0x80000000 * n;
	cpu.R[2] = 1;
	test_cpu( { 0046002, 0010000  } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[1] = 10;
	test_ea_l(cpu, { 0046000, 0010000 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[1] == 30 );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(mulu_ll, fixture);
BOOST_AUTO_TEST_CASE(result) {
	int64_t a = 0x50000000ll;
	int32_t b = 14;
	cpu.R[0] = (uint32_t)a;
	cpu.R[2] = b;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046002, 0002001  } );
	int64_t v = a * b;
	uint32_t l = cpu.R[0];
	uint32_t h = cpu.R[1];
	BOOST_TEST( l == (v & 0xffffffff));
	BOOST_TEST( h == (uint32_t)(v >> 32) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(z) {
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046002, 0012000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_CASE(n) {
	cpu.R[1] = 0xffffffff;
	cpu.R[2] = 0xffffffff*2;
	test_cpu( { 0046002, 0012000  } );
	BOOST_TEST( cpu.N == true );
}

BOOST_AUTO_TEST_SUITE_END();
