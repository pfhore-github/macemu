#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"
extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(negx_byte, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), old_x) {
	cpu.X = old_x;
	cpu.R[0] = 23;
	test_cpu( { 0040000 } );
	BOOST_TEST( (int8_t)cpu.R[0] == -23 - old_x);
}
BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 is_0
	)
{
	cpu.R[0] = ! is_0;
	test_cpu( { 0040000 } );
	BOOST_TEST( cpu.C == ! is_0 );
	BOOST_TEST( cpu.X == ! is_0 );
}
BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v * 0x80;
	test_cpu( { 0040000 } );
	BOOST_TEST( cpu.V == v );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n;
	test_cpu( { 0040000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 old_z, new_z
	)
{
	cpu.Z = old_z;
	cpu.R[0] = ! new_z;
	test_cpu( { 0040000 } );
	BOOST_TEST( cpu.Z == (old_z & new_z) );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_b(cpu, { 0042000 }, 22, ea, -22 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(negx_word, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), old_x) {
	cpu.X = old_x;
	cpu.R[0] = 2323;
	test_cpu( { 0040100 } );
	BOOST_TEST( (int16_t)cpu.R[0] == -2323 - old_x );
}
BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 is_0
	)
{
	cpu.R[0] = ! is_0;
	test_cpu( { 0040100 } );
	BOOST_TEST( cpu.C == ! is_0 );
	BOOST_TEST( cpu.X == ! is_0 );
}
BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v * 0x8000;
	test_cpu( { 0040100 } );
	BOOST_TEST( cpu.V == v );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n;
	test_cpu( { 0040100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 old_z, new_z
	)
{
	cpu.Z = old_z;
	cpu.R[0] = ! new_z;
	test_cpu( { 0040100 } );
	BOOST_TEST( cpu.Z == (old_z & new_z) );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_w(cpu, { 0040100 }, 2222, ea, -2222 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(negx_long, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), old_x) {
	cpu.X = old_x;
	cpu.R[0] = 232323;
	test_cpu( { 0040200 } );
	BOOST_TEST( (int32_t)cpu.R[0] == -232323 - old_x);
}
BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 is_0
	)
{
	cpu.R[0] = ! is_0;
	test_cpu( { 0040200 } );
	BOOST_TEST( cpu.C == ! is_0 );
	BOOST_TEST( cpu.X == ! is_0 );
}
BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v * 0x80000000;
	test_cpu( { 0040200 } );
	BOOST_TEST( cpu.V == v );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n;
	test_cpu( { 0040200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 old_z, new_z
	)
{
	cpu.Z = old_z;
	cpu.R[0] = ! new_z;
	test_cpu( { 0040200 } );
	BOOST_TEST( cpu.Z == (old_z & new_z) );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_l(cpu, { 0040200 }, 222222, ea, -222222 );
}

BOOST_AUTO_TEST_SUITE_END();
