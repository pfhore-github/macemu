#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(sub_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[1] = 34;
	test_ea_b(cpu, { 0111000 }, 23, DR, 23 );
	BOOST_TEST( cpu.R[1] == ( 34 - 23 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[1] = ! xc;
	test_ea_b(cpu, { 0111000 }, 1, DR, 1 );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x81 - v;
	test_ea_b(cpu, { 0111000 }, 1, DR, 1 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = v;
	test_ea_b(cpu, { 0111000 }, 0x81, DR, 0x81 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf0 : 1;
	test_ea_b(cpu, { 0111000 }, 1, DR, 1 );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = !z;
	test_ea_b(cpu, { 0111000 }, 0, DR, 0 );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(to_reg_dn)
{
	cpu.R[5] = 34 | 0xffffff00;
	test_ea_b(cpu, { 0115000 }, 23, DR, 23 );
	BOOST_TEST( cpu.R[5] == ( 11 |0xffffff00) );
}

BOOST_AUTO_TEST_CASE(from_reg_dn)
{
	cpu.R[5] = 34;
	test_ea_b(cpu, { 0115400 }, 23, MEM, -11 );
}


BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[5] = 34;
	test_ea_b(cpu, { 0115000 }, 23, ea, 23 );
	BOOST_TEST( cpu.R[5] == 11 );
}

BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[5] = 34;
	test_ea_b(cpu, { 0115400 }, 23, ea, -11 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(sub_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[1] = 3400;
	test_ea_w(cpu, { 0111100 }, 2300, DR, 2300 );
	BOOST_TEST( cpu.R[1] == ( 3400 - 2300 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[1] = ! xc;
	test_ea_w(cpu, { 0111100 }, 1, DR, 1 );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x8001 - v;
	test_ea_w(cpu, { 0111100 }, 1, DR, 1 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = v;
	test_ea_w(cpu, { 0111100 }, 0x8001, DR, 0x8001 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf000 : 1;
	test_ea_w(cpu, { 0111100 }, 1, DR, 1 );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = ! z;
	test_ea_w(cpu, { 0111100 }, 0, DR, 0 );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(to_reg_dn)
{
	cpu.R[5] = 3400 | 0xffff0000;
	test_ea_w(cpu, { 0115100 }, 2300, DR, 2300 );
	BOOST_TEST( cpu.R[5] == ( 1100 |0xffff0000) );
}

BOOST_AUTO_TEST_CASE(from_reg_dn)
{
	
	cpu.R[5] = 3400;
	test_ea_w(cpu, { 0115500 }, 2300, MEM, -1100 );
}


BOOST_DATA_TEST_CASE(from_dn, ea_all, ea)
{
	cpu.R[5] = 3400;
	test_ea_w(cpu, { 0115100 }, 2300, ea, 2300 );
	BOOST_TEST( cpu.R[5] == 1100 );
}

BOOST_DATA_TEST_CASE(to_mem, ea_write2, ea)
{
	cpu.R[5] = 3400;	
	test_ea_w(cpu, { 0115500 }, 2300, ea, -1100 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(sub_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[1] = 340000;
	test_ea_l(cpu, { 0111200 }, 230000, DR, 230000 );
	BOOST_TEST( cpu.R[1] == ( 340000 - 230000 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[1] = !xc;
	test_ea_l(cpu, { 0111200 }, 1, DR, 1 );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x80000001 - v;
	test_ea_l(cpu, { 0111200 }, 1, DR, 1 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = v;
	test_ea_l(cpu, { 0111200 }, 0x80000001, DR, 0x80000001 );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf0000000 : 1;
	test_ea_l(cpu, { 0111200 }, 1, DR, 1 );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = ! z;
	test_ea_l(cpu, { 0111200 }, 0, DR, 0 );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(from_reg_dn)
{
	cpu.R[5] = 340000;
	test_ea_l(cpu, { 0115600 }, 230000, MEM, -110000 );
}


BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	cpu.R[5] = 340000;
	test_ea_l(cpu, { 0115200 }, 230000, ea, 230000 );
	BOOST_TEST( cpu.R[5] == 110000 );
}

BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[5] = 340000;
	test_ea_l(cpu, { 0115600 }, 230000, ea, -110000 );
}


BOOST_AUTO_TEST_SUITE_END();
