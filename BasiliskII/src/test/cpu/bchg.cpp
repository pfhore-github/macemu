#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bchg_byte, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	test_write8(0x1000, 0x80 | ((!z) << 4));
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004120, 4});
	BOOST_TEST( test_read8(0x1000) == ( 0x80 | (z << 4)) );
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	test_write8(0x1000, 0x80 | 1 << 4);
	cpu.R[4] = 4;
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004520 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	test_write8(0x1000, 0x80 |  1 << 3);
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004120, 3});
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}
BOOST_DATA_TEST_CASE(ea_by_reg, ea_writedn, ea) {
	cpu.R[4] = 3;
	test_ea_b(cpu, { 0004500 }, 0x80 | 1 << 3, ea, 0x80 );
}

BOOST_DATA_TEST_CASE(ea_by_imm, ea_writedn, ea) {
	test_ea_b(cpu, { 0004100, 3 }, 0x80 | 1 << 3, ea, 0x80 );
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(bchg_long, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	cpu.X = cpu.N = cpu.V = cpu.C = true;
	cpu.R[0] = (1 | ((!z) << 22));
	test_cpu({ 0004100, 22});
	BOOST_TEST( cpu.R[0] == (1 | (z << 22)) );
	BOOST_TEST(cpu.X == true );
	BOOST_TEST(cpu.N == true );
	BOOST_TEST(cpu.V == true );
	BOOST_TEST(cpu.C == true );
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	cpu.R[5] = 1 | 1 << 22;
	cpu.R[3] = 22;
	test_cpu({ 0003505 });
	BOOST_TEST( cpu.R[5] == 1 );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	cpu.R[5] = 1 | 1 << 22;
	test_cpu({ 0004105, 22});
	cpu.R[5] = 1;
}
BOOST_AUTO_TEST_SUITE_END();
