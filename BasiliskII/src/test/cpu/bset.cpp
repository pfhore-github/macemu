#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bset_byte, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	test_write8(0x1000, 0x80 | ((!z) << 4));
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004320, 4});
	BOOST_TEST( test_read8(0x1000) == 0x90 );
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	test_write8(0x1000, 0x80 );
	cpu.R[3] = 4;
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0003720 });
	BOOST_TEST( test_read8(0x1000) == 0x90 );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	test_write8(0x1000, 0x80);
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004320, 3});
	BOOST_TEST( test_read8(0x1000) == 0x88 );
}

BOOST_DATA_TEST_CASE(ea_by_reg, ea_writedn, ea) {
	cpu.R[4] = 3;
	test_ea_b(cpu, { 0004700 }, 0x80, ea, 0x88 );
}

BOOST_DATA_TEST_CASE(ea_by_imm, ea_writedn, ea) {
	test_ea_b(cpu, { 0004300, 3 }, 0x80 | 1 << 3, ea, 0x88 );
}



BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(bset_long, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = (1 | ((!z) << 22));
	test_cpu({ 0004300, 22});
	BOOST_TEST( cpu.R[0] == (1 | 1 << 22) );
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	cpu.R[3] = 1;
	cpu.R[2] = 22;
	test_cpu({ 0002703 });
	BOOST_TEST( cpu.R[3] == ( 1 | 1 << 22 ));
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	cpu.R[3] = 1 ;
	test_cpu({ 0004303, 22});
	BOOST_TEST( cpu.R[3] == ( 1 | 1 << 22 ) );
}
BOOST_AUTO_TEST_SUITE_END();
