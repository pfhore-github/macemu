#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"
extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(tas_byte, fixture);
BOOST_AUTO_TEST_CASE(result) {
	cpu.R[0] = 0;
	cpu.V = cpu.C = true;
	test_cpu( { 0045300 } ); 
	BOOST_TEST( cpu.R[0] == 0x80 );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 0;
	test_cpu( { 0045300 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0045300 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_b(cpu, { 0045300 }, 0, ea, 0x80 );
}

BOOST_AUTO_TEST_SUITE_END();
