#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(suba_word, fixture);
BOOST_DATA_TEST_CASE( result,
					  bdata::xrange(2),
					  sign )  {
	cpu.R[8+0] = 0x200000;
	cpu.R[0] = sign ? -(0x1000) : 0x1000;
	test_cpu( { 0110300 } );
	BOOST_TEST( cpu.R[8+0] == 0x200000 - ( sign ? -(0x1000) : 0x1000 ) );
}

BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	cpu.R[8+1] = 0x300000;
	test_ea_w(cpu, { 0111300 }, 0x2000, ea, 0x2000 );
	BOOST_TEST( cpu.R[8+1] == 0x300000 - 0x2000 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(suba_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[8+0] = 0x20007000;
	cpu.R[0] = 0x5000;
	test_cpu( { 0110700 } );
	BOOST_TEST( cpu.R[8+0] == 0x20002000 );
}

BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	cpu.R[8+1] = 0x3000000;
	test_ea_l(cpu, { 0111700 }, 0x2000000, ea, 0x2000000 );
	BOOST_TEST( cpu.R[8+1] == 0x1000000 );
}

BOOST_AUTO_TEST_SUITE_END();
