#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(ftrapcc, fixture);
BOOST_DATA_TEST_CASE( no_word, bdata::xrange(2), tf )  {
	try {
		test_cpu( { 0171174, (uint16_t)(tf ? 0 : 16) });					 
	} catch( ... ) {}
	BOOST_TEST( cpu.PC == 4 );
}
BOOST_DATA_TEST_CASE( word1, bdata::xrange(2), tf )  {
	try {
		test_cpu( { 0171172, (uint16_t)(tf ? 0 : 16), 0 });					 
	} catch( ... ) {}
	BOOST_TEST( cpu.PC == 6 );
}

BOOST_DATA_TEST_CASE( dword, bdata::xrange(2), tf )  {
	try {
		test_cpu( { 0171173, (uint16_t)(tf ? 0 : 16), 0, 0 });					 
	} catch( ... ) {}
	BOOST_TEST( cpu.PC == 8 );
}
#if 0


BOOST_DATA_TEST_CASE(t, t_data, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	BOOST_CHECK_THROW( test_cpu( { (uint16_t)(0x50FC | (cond.cc << 8) ) }),
					   Trapcc );
}

BOOST_DATA_TEST_CASE(f, f_data, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0x50FC | (cond.cc << 8) ) })
		);
}

#endif

BOOST_AUTO_TEST_SUITE_END();
