#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(addq_byte, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.R[3] = 3 | 0xffffff00;
	test_cpu( { 0052003 } );
	BOOST_TEST( cpu.R[3] == ( 5 | 0xffffff00) );
}

BOOST_AUTO_TEST_CASE(data0)
{
	cpu.R[3] = 3;
	test_cpu( { 0050003 } );
	BOOST_TEST( cpu.R[3] == 11 );
}


BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0xfe + xc;
	test_cpu( { 0051000 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7e + v;
	test_cpu( { 0051000 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	test_cpu( { 0051000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z * 0xff;
	test_cpu( { 0051000 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write, ea)
{
	test_ea_b(cpu, { 0051000 }, 34, ea, 35);
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addq_word, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.R[3] = 300 | 0xffff0000;
	test_cpu( { 0052103 } );
	BOOST_TEST( cpu.R[3] == ( 302 | 0xffff0000 )  );
}

BOOST_AUTO_TEST_CASE(result_an_1)
{
	cpu.R[8+3] = 0xffff;
	test_cpu( { 0052113 } );
	BOOST_TEST( cpu.R[8+3] == 0x10001 );
}

BOOST_AUTO_TEST_CASE(result_an_2)
{
	cpu.Z = false;
	cpu.R[8+3] = 0xffffffff;
	test_cpu( { 0051113 } );
	BOOST_TEST( cpu.Z == false );
}


BOOST_AUTO_TEST_CASE(data0)
{
	cpu.R[3] = 300;
	test_cpu( { 0050103 } );
	BOOST_TEST( cpu.R[3] == 308 );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0xfffe + xc;
	test_cpu( { 0051100 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7ffe + v;
	test_cpu( { 0051100 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	test_cpu( { 0051100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z * 0xffff;
	test_cpu( { 0051100 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(dn)
{
	cpu.R[1] = 2300;
	test_cpu( { 0051101 } );
	BOOST_TEST( cpu.R[1] == 2301 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write, ea)
{
	test_ea_w(cpu, { 0051100 }, 3400, ea, 3401);
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(addq_long, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.R[3] = 300000;
	test_cpu( { 0052203 } );
	BOOST_TEST( cpu.R[3] == 300002 );
}

BOOST_AUTO_TEST_CASE(data0)
{
	cpu.R[3] = 300000;
	test_cpu( { 0050203 } );
	BOOST_TEST( cpu.R[3] == 300008 );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0xfffffffe + xc;
	test_cpu( { 0051200 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7ffffffe + v;
	test_cpu( { 0051200 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0051200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z * 0xffffffff;
	test_cpu( { 0051200 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write, ea)
{
	test_ea_l(cpu, { 0051200 }, 340000, ea, 340001);
}

BOOST_AUTO_TEST_CASE(result_an_2)
{
	cpu.Z = false;
	cpu.R[8+3] = 0xffffffff;
	test_cpu( { 0051213 } );
	BOOST_TEST( cpu.Z == false );
}


BOOST_AUTO_TEST_SUITE_END();
