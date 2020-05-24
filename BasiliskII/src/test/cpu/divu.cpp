#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(divu_w, fixture);
BOOST_AUTO_TEST_CASE(result) {
	int16_t nm = 76;
	int16_t dn = 14;
	cpu.R[0] = nm;
	cpu.R[1] = dn;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0100301 } );
	uint32_t v = cpu.R[0];
	BOOST_TEST( (uint16_t)v == ( nm / dn ) );
	BOOST_TEST( (uint16_t)(v >> 16) == ( nm % dn ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[0] = 0x10000;
	cpu.R[1] = 1;
	test_cpu( { 0100301 } );
	BOOST_TEST( cpu.R[0] == 0x10000 );
	BOOST_TEST( cpu.V == true );
}


BOOST_AUTO_TEST_CASE(div0) {
	cpu.R[0] = 1;
	cpu.R[1] = 0;
	test_cpu( { 0100301 } );
	test_in_exception(5);
}

BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[1] = 3;
	test_cpu( { 0100301 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_CASE(n) {
	cpu.R[0] = 0x8000;
	cpu.R[1] = 1;
	test_cpu( { 0100301 } );
	BOOST_TEST( cpu.N == true );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[3] = 10;
	test_ea_w(cpu, { 0103300 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[3] == 0x10003 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(divu_l, fixture);
BOOST_AUTO_TEST_CASE(result) {
	int32_t nm = 100000;
	int32_t dn = 14;
	cpu.R[1] = nm;
	cpu.R[2] = dn;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046102, 0010000  } );
	int32_t r = cpu.R[0];
	int32_t q = cpu.R[1];
	BOOST_TEST( q == ( nm / dn ) );
	BOOST_TEST( r == ( nm % dn ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(div0) {
	cpu.R[1] = 1;
	cpu.R[2] = 0;
	test_cpu( { 0046102, 0010000 } );
	test_in_exception(5);
}

BOOST_AUTO_TEST_CASE(z) {
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046102, 0010000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_CASE(n) {
	cpu.R[1] = 0x80000000;
	cpu.R[2] = 1;
	test_cpu( { 0046102, 0010000  } );
	BOOST_TEST( cpu.N == true );
}

BOOST_AUTO_TEST_CASE(no_rem) {
	cpu.R[0] = 10;
	cpu.R[2] = 3;
	test_cpu( { 0046102, 0000000  } );
	BOOST_TEST( cpu.R[0] == 3 );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[0] = 10;
	test_ea_l(cpu, { 0046100, 0000001 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[0] == 3 );
	BOOST_TEST( cpu.R[1] == 1 );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(divu_ll, fixture);
BOOST_AUTO_TEST_CASE(result) {
	int64_t nm = 0x200000000l;
	int32_t dn = 14;
	cpu.R[0] = nm >> 32;
	cpu.R[1] = (uint32_t)nm;
	cpu.R[2] = dn;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046102, 0012000  } );
	int32_t r = cpu.R[0];
	int32_t q = cpu.R[1];
	BOOST_TEST( q == ( nm / dn ) );
	BOOST_TEST( r == ( nm % dn ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[0] = 1;
	cpu.R[1] = 0;
	cpu.R[2] = 1;
	test_cpu( { 0046102, 0012000 } );
	BOOST_TEST( cpu.R[0] == 1 );
	BOOST_TEST( cpu.R[1] == 0 );
	BOOST_TEST( cpu.V == true );
}

BOOST_AUTO_TEST_CASE(div0) {
	cpu.R[0] = 0;
	cpu.R[1] = 1;
	cpu.R[2] = 0;
	test_cpu( { 0046102, 0012000 } );
	test_in_exception(5);
}
BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046102, 0012000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_CASE(n) {
	cpu.R[0] = 1;
	cpu.R[1] = 0;
	cpu.R[2] = 2;
	test_cpu( { 0046102, 0012000  } );
	BOOST_TEST( cpu.N == true );
}

BOOST_AUTO_TEST_SUITE_END();
