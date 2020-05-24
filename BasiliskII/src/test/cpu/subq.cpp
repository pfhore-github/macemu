#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(subq_byte, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.R[3] = 3 | 0xffffff00;
	test_cpu( { 0052403 } );
	BOOST_TEST( cpu.R[3] == ( 1 | 0xffffff00) );
}

BOOST_AUTO_TEST_CASE(data0)
{
	cpu.R[3] = 12;
	test_cpu( { 0050403 } );
	BOOST_TEST( cpu.R[3] == 4 );
}


BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = !xc;
	test_cpu( { 0051400 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x81 - v;
	test_cpu( { 0051400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	test_cpu( { 0051400 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z;
	test_cpu( { 0051400 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write, ea)
{
	test_ea_b(cpu, { 0051400 }, 34, ea, 33);
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(subq_word, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.R[3] = 300 | 0xffff0000;
	test_cpu( { 0052503 } );
	BOOST_TEST( cpu.R[3] == ( 298 | 0xffff0000 )  );
}

BOOST_AUTO_TEST_CASE(result_an_1)
{
	cpu.R[8+3] = 0;
	test_cpu( { 0051513 } );
	BOOST_TEST( cpu.R[8+3] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(result_an_2)
{
	cpu.Z = false;
	cpu.R[8+3] = 1;
	test_cpu( { 0051513 } );
	BOOST_TEST( cpu.Z == false );
}

BOOST_AUTO_TEST_CASE(data0)
{
	cpu.R[3] = 300;
	test_cpu( { 0050503 } );
	BOOST_TEST( cpu.R[3] == 292 );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = !xc;
	test_cpu( { 0051500 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x8001 - v;
	test_cpu( { 0051500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	test_cpu( { 0051500 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z;
	test_cpu( { 0051500 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(dn)
{
	cpu.R[1] = 2300;
	test_cpu( { 0051501 } );
	BOOST_TEST( cpu.R[1] == 2299 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write, ea)
{
	test_ea_w(cpu, { 0051500 }, 3400, ea, 3399);
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(subq_long, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.R[3] = 300000;
	test_cpu( { 0052603 } );
	BOOST_TEST( cpu.R[3] == 299998 );
}

BOOST_AUTO_TEST_CASE(data0)
{
	cpu.R[3] = 300000;
	test_cpu( { 0050603 } );
	BOOST_TEST( cpu.R[3] == 299992 );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = ! xc;
	test_cpu( { 0051600 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80000001 - v;
	test_cpu( { 0051600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0051600 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z;
	test_cpu( { 0051600 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write, ea)
{
	test_ea_l(cpu, { 0051600 }, 340000, ea, 339999);
}

BOOST_AUTO_TEST_CASE(result_an_1)
{
	cpu.R[8+3] = 0;
	test_cpu( { 0051613 } );
	BOOST_TEST( cpu.R[8+3] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(result_an_2)
{
	cpu.Z = false;
	cpu.R[8+3] = 1;
	test_cpu( { 0051613 } );
	BOOST_TEST( cpu.Z == false );
}


BOOST_AUTO_TEST_SUITE_END();
