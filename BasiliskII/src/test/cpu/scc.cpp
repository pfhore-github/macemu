#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(scc, fixture);

BOOST_DATA_TEST_CASE(t, t_data, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x50C0 | (cond.cc << 8) ) } );
	BOOST_TEST( cpu.R[0] == 0xff );
}

BOOST_DATA_TEST_CASE(f, f_data, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x50C0 | (cond.cc << 8) ) } );
	BOOST_TEST( cpu.R[0] == 0 );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_b(cpu, { 0x50C0 }, 0, ea, 0xff );
}


BOOST_AUTO_TEST_SUITE_END();
