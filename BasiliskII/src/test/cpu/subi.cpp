#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(subi_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 23 | 0xffffff00;
	test_cpu( { 0002000, 11 } );
	BOOST_TEST( cpu.R[0] == (( 23 -11 )|0xffffff00) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = ! xc;
	test_cpu( { 0002000, 1 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x81 - v;
	test_cpu( { 0002000, 1 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	test_cpu( { 0002000, 0x81 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	cpu.R[1] = 1;
	test_cpu( { 0002000, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0002000, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_writedn, ea)
{
	test_ea_b(cpu, { 0002000, 23 }, 34, ea, 11 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(subi_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 2300 | 0xffff0000;
	test_cpu( { 0002100, 1100 } );
	BOOST_TEST( cpu.R[0] == (( 2300 - 1100 )|0xffff0000) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = !xc;
	test_cpu( { 0002100, 1 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x8001 - v;
	test_cpu( { 0002100, 1 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	test_cpu( { 0002100, 0x8001 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	test_cpu( { 0002100, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0002100, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_writedn, ea)
{
	test_ea_w(cpu, { 0002100, 2300 }, 3400, ea, 1100 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(subi_long, fixture);
constexpr uint16_t HIGH = uint16_t(110000>>16);
constexpr uint16_t LOW = uint16_t(110000 & 0xffff);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 230000;
	test_cpu( { 0002200, HIGH, LOW } );
	BOOST_TEST( cpu.R[0] == ( 230000 - 110000 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = !xc;
	test_cpu( { 0002200, 0, 1 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80000001 - v;
	test_cpu( { 0002200, 0, 1 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	test_cpu( { 0002200, 0x8000, 1 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0002200, 0, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0002200, 0, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_writedn, ea)
{
	test_ea_l(cpu, { 0002200, HIGH, LOW }, 230000, ea, 120000 );
}


BOOST_AUTO_TEST_SUITE_END();
