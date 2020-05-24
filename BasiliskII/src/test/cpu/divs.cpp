#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(divs_w, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2) * bdata::xrange(2),
					numer_sign, denom_sign) {
	int16_t nm = numer_sign ? -76 : 76;
	int16_t dn = denom_sign ? -14 : 14;
	cpu.R[0] = nm;
	cpu.R[1] = dn;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0100701 } );
	uint32_t v = cpu.R[0];
	BOOST_TEST( (int16_t)v == ( nm / dn ) );
	BOOST_TEST( (int16_t)(v >> 16) == ( nm % dn ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == (numer_sign != denom_sign) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[0] = 0x10000;
	cpu.R[1] = 1;
	test_cpu( { 0100701 } );
	BOOST_TEST( cpu.R[0] == 0x10000 );
	BOOST_TEST( cpu.V == true );
}

BOOST_AUTO_TEST_CASE(div0) {
	cpu.R[0] = 1;
	cpu.R[1] = 0;
	test_cpu( { 0100701 } );
	test_in_exception(5);
}

BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[1] = 3;
	test_cpu( { 0100701 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[2] = 10;
	test_ea_w(cpu, { 0102700 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[2] == 0x10003 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(divs_l, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2) * bdata::xrange(2),
					numer_sign, denom_sign) {
	int32_t nm = (numer_sign ? -1 : 1) * 100000;
	int32_t dn = denom_sign ? -14 : 14;
	cpu.R[1] = nm;
	cpu.R[2] = dn;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046102, 0014000  } );
	int32_t r = cpu.R[0];
	int32_t q = cpu.R[1];
	BOOST_TEST( q == ( nm / dn ) );
	BOOST_TEST( r == ( nm % dn ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == (numer_sign != denom_sign) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[0] = 0xdeadbeaf;
	cpu.R[1] = 0x80000000;
	cpu.R[2] = -1;
	test_cpu( { 0046102, 0014000  } );
	BOOST_TEST( cpu.R[0] == 0xdeadbeaf );
	BOOST_TEST( cpu.R[1] == 0x80000000 );
	BOOST_TEST( cpu.V == true );
}

BOOST_AUTO_TEST_CASE(div0) {
	cpu.R[1] = 1;
	cpu.R[2] = 0;
	test_cpu( { 0046102, 0014000 } );
	test_in_exception(5);
}

BOOST_AUTO_TEST_CASE(z) {
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046102, 0014000  } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_CASE(no_rem) {
	cpu.R[0] = 10;
	cpu.R[2] = 3;
	test_cpu( { 0046102, 0004000  } );
	BOOST_TEST( cpu.R[0] == 3 );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	cpu.R[0] = 10;
	test_ea_l(cpu, { 0046100, 0004001 }, 3, ea, 3 );
	BOOST_TEST( cpu.R[0] == 3 );
	BOOST_TEST( cpu.R[1] == 1 );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(divs_ll, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2) * bdata::xrange(2),
					numer_sign, denom_sign) {
	int64_t nm = (numer_sign ? -1 : 1) * 0x200000000l;
	int32_t dn = denom_sign ? -14 : 14;
	cpu.R[0] = nm >> 32;
	cpu.R[1] = (uint32_t)nm;
	cpu.R[2] = dn;
	cpu.C = true;
	cpu.V = true;
	test_cpu( { 0046102, 0016000  } );
	int32_t r = cpu.R[0];
	int32_t q = cpu.R[1];
	BOOST_TEST( q == ( nm / dn ) );
	BOOST_TEST( r == ( nm % dn ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.N == (numer_sign != denom_sign) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(overflow) {
	cpu.R[0] = 1;
	cpu.R[1] = 0;
	cpu.R[2] = 1;
	test_cpu( { 0046102, 0016000 } );
	BOOST_TEST( cpu.R[0] == 1 );
	BOOST_TEST( cpu.R[1] == 0 );
	BOOST_TEST( cpu.V == true );
}

BOOST_AUTO_TEST_CASE(div0) {
	cpu.R[0] = 0;
	cpu.R[1] = 1;
	cpu.R[2] = 0;
	test_cpu( { 0046102, 0016000 } );
	test_in_exception(5);
}
BOOST_AUTO_TEST_CASE(z) {
	cpu.R[0] = 0;
	cpu.R[1] = 0;
	cpu.R[2] = 3;
	test_cpu( { 0046102, 0016000  } );
	BOOST_TEST( cpu.Z == true );
}


BOOST_AUTO_TEST_SUITE_END();
