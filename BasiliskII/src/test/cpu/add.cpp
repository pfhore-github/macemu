#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(add_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[1] = 34;
	test_ea_b(cpu, { 0151000 }, 23, DR, 23 );
	BOOST_TEST( cpu.R[1] == ( 23 + 34 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[1] = xc;
	test_ea_b(cpu, { 0151000 }, 0xff, DR, 0xff );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = v;
	test_ea_b(cpu, { 0151000 }, 0x7f, DR, 0x7f );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x7f + v;
	test_ea_b(cpu, { 0151000 }, 0x80, DR, 0x80 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf0 : 1;
	test_ea_b(cpu, { 0151000 }, 1, DR, 1 );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = !z;
	test_ea_b(cpu, { 0151000 }, 0, DR, 0 );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(to_reg_dn)
{
	cpu.R[5] = 34 | 0xffffff00;
	test_ea_b(cpu, { 0155000 }, 23, DR, 23 );
	BOOST_TEST( cpu.R[5] == ( 57 |0xffffff00) );
}

BOOST_AUTO_TEST_CASE(from_reg_dn)
{
	cpu.R[5] = 34;
	test_ea_b(cpu, { 0155400 }, 23, MEM, 57 );
}


BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[5] = 34;
	test_ea_b(cpu, { 0155000 }, 23, ea, 23 );
	BOOST_TEST( cpu.R[5] == 57 );
}

BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[5] = 34;
	test_ea_b(cpu, { 0155400 }, 23, ea, 57 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(add_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[1] = 3400;
	test_ea_w(cpu, { 0151100 }, 2300, DR, 2300 );
	BOOST_TEST( cpu.R[1] == ( 2300 + 3400 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[1] = xc;
	test_ea_w(cpu, { 0151100 }, 0xffff, DR, 0xffff );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = v;
	test_ea_w(cpu, { 0151100 }, 0x7fff, DR, 0x7fff );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x7fff + v;
	test_ea_w(cpu, { 0151100 }, 0x8000, DR, 0x8000 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf000 : 1;
	test_ea_w(cpu, { 0151100 }, 1, DR, 1 );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = ! z;
	test_ea_w(cpu, { 0151100 }, 0, DR, 0 );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(to_reg_dn)
{
	cpu.R[5] = 3400 | 0xffff0000;
	test_ea_w(cpu, { 0155100 }, 2300, DR, 2300 );
	BOOST_TEST( cpu.R[5] == ( 5700 |0xffff0000) );
}

BOOST_AUTO_TEST_CASE(from_reg_dn)
{
	
	cpu.R[5] = 3400;
	test_ea_w(cpu, { 0155500 }, 2300, MEM, 5700 );
}


BOOST_DATA_TEST_CASE(from_dn, ea_all, ea)
{
	cpu.R[5] = 3400;
	test_ea_w(cpu, { 0155100 }, 2300, ea, 2300 );
	BOOST_TEST( cpu.R[5] == 5700 );
}

BOOST_DATA_TEST_CASE(to_mem, ea_write2, ea)
{
	cpu.R[5] = 3400;	
	test_ea_w(cpu, { 0155500 }, 2300, ea, 5700 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(add_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[1] = 340000;
	test_ea_l(cpu, { 0151200 }, 230000, DR, 230000 );
	BOOST_TEST( cpu.R[1] == ( 230000 + 340000 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[1] = xc;
	test_ea_l(cpu, { 0151200 }, -1, DR, -1 );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = v;
	test_ea_l(cpu, { 0151200 }, 0x7fffffff, DR, 0x7fffffff );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x7fffffffU + v;
	test_ea_l(cpu, { 0151200 }, 0x80000000, DR, 0x80000000 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf0000000 : 1;
	test_ea_l(cpu, { 0151200 }, 1, DR, 1 );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = ! z;
	test_ea_l(cpu, { 0151200 }, 0, DR, 0 );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(from_reg_dn)
{
	cpu.R[5] = 340000;
	test_ea_l(cpu, { 0155600 }, 230000, MEM, 570000 );
}


BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	cpu.R[5] = 340000;
	test_ea_l(cpu, { 0155200 }, 230000, ea, 230000 );
	BOOST_TEST( cpu.R[5] == 570000 );
}

BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[5] = 340000;
	test_ea_l(cpu, { 0155600 }, 230000, ea, 570000 );
}


BOOST_AUTO_TEST_SUITE_END();
