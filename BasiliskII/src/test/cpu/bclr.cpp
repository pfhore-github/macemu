#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bclr_byte, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	test_write8(0x1000, 0x80 | ((!z) << 4));
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004220, 4});
	BOOST_TEST( test_read8(0x1000) == 0x80 );
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	test_write8(0x1000, 0x80 | 1 << 4);
	cpu.R[3] = 4;
	cpu.R[8+1] = 0x1000; 
	test_cpu({ 0003621 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	test_write8(0x1000, 0x80 |  1 << 3);
	cpu.R[8+1] = 0x1000; 
	test_cpu({ 0004221, 3});
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_DATA_TEST_CASE(ea_by_reg, ea_writedn, ea) {
	cpu.R[4] = 3;
	test_ea_b(cpu, { 0004500 }, 0x80 | 1 << 3, ea, 0x80 );
}

BOOST_DATA_TEST_CASE(ea_by_imm, ea_writedn, ea) {
	test_ea_b(cpu, { 0004200, 3 }, 0x80 | 1 << 3, ea, 0x80 );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(bclr_long, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = (1 | ((!z) << 22));
	test_cpu({ 0004200, 22});
	BOOST_TEST( cpu.R[0] == 1 );
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	cpu.R[3] = 1 | 1 << 22;
	cpu.R[2] = 22;
	test_cpu({ 0002603 });
	BOOST_TEST( cpu.R[3] == 1 );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	cpu.R[3] = 1 | 1 << 22;
	test_cpu({ 0004203, 22});
	cpu.R[3] = 1;

}
BOOST_AUTO_TEST_SUITE_END();
