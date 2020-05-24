#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(pea, fixture);

BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+7] = 0x2000;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0044122 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1000 );
}
BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+7] = 0x2000;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0044152, 0x100 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1100 );
}
BOOST_AUTO_TEST_CASE(d8anxn)
{
	cpu.R[8+7] = 0x2000;
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 0x10;
	test_cpu( { 0044162, 0x0830 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1040 );
}

BOOST_AUTO_TEST_CASE(xxx_w)
{
	cpu.R[8+7] = 0x2000;
	test_cpu( { 0044170, 0x1000 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1000 );
}
BOOST_AUTO_TEST_CASE(xxx_l)
{
	cpu.R[8+7] = 0x2000;
	test_cpu( { 0044171, 0, 0x1000 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1000 );
}
BOOST_AUTO_TEST_CASE(d16pc)
{
	cpu.R[8+7] = 0x2000;
	test_cpu( { 0044172, 0x1000 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1002 );
}

BOOST_AUTO_TEST_CASE(d8pcxn)
{
	cpu.R[8+7] = 0x2000;
	cpu.R[0] = 0x1000;
	test_cpu( { 0044173, 0x0830 } );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 0x1032 );
}

BOOST_AUTO_TEST_SUITE_END();
