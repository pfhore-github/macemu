#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(dbcc, fixture);
BOOST_DATA_TEST_CASE(t, bdata::xrange(8) * t_data,
					 r, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x50C8 | (cond.cc << 8) | r), 0x1000 } );
	cpu.R[r] = 0;
	BOOST_TEST( cpu.PC == 4 );
	BOOST_TEST( cpu.R[r] == 0 );
}
BOOST_DATA_TEST_CASE(f_j, bdata::xrange(8) * f_data,
					 r, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	cpu.R[r] = 2;
	test_cpu( { (uint16_t)(0x50C8 | (cond.cc << 8) | r), 0x1000 } );
	BOOST_TEST( cpu.PC == 0x1002 );
	BOOST_TEST( cpu.R[r] == 1 );
}
BOOST_DATA_TEST_CASE(f_nj, bdata::xrange(8) * f_data,
					 r, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	cpu.R[r] = 0;
	test_cpu( { (uint16_t)(0x50C8 | (cond.cc << 8 ) | r), 0x1000 } );
	BOOST_TEST( cpu.PC == 4 );
	BOOST_TEST( (uint16_t)cpu.R[r] == 0xffff );
}

BOOST_AUTO_TEST_SUITE_END();
