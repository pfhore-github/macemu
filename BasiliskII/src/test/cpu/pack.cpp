#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"
extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(pack, fixture);
BOOST_AUTO_TEST_CASE(dx_dy)
{
	cpu.R[3] = 0x1234;
	test_cpu( { 0104503, 0 } );
	BOOST_TEST( (uint8_t)cpu.R[4] == 0x24);
}


BOOST_AUTO_TEST_CASE(ax_ay)
{
	cpu.R[8+3] = 0x1002;
	cpu.R[8+4] = 0x1101;
	test_write16(0x1000, 0x1234);
	
	test_cpu( { 0104513, 0 } );
	BOOST_TEST( test_read8(0x1100) == 0x24 );
	BOOST_TEST( cpu.R[8+3] = 0x1000 );
	BOOST_TEST( cpu.R[8+4] = 0x1100 );
}

BOOST_AUTO_TEST_CASE(adjust)
{
	cpu.R[3] = 0x1234;
	test_cpu( { 0104503, 0x200 } );
	BOOST_TEST( (uint8_t)cpu.R[4] == 0x44);
}

BOOST_AUTO_TEST_SUITE_END();
