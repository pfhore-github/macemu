#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(btst_byte, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	test_write8(0x1000, 0x80 | ((!z) << 4));
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004020, 4});
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	test_write8(0x1000, 0xff &~ (1 << 4));
	cpu.R[3] = 4;
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0003420 });
	BOOST_TEST(cpu.Z == true  );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	test_write8(0x1000, 0xff &~ (1 << 3));
	cpu.R[8+0] = 0x1000; 
	test_cpu({ 0004020, 3});
	BOOST_TEST(cpu.Z == true  );
}

BOOST_DATA_TEST_CASE(ea_by_reg, ea_writedn, ea) {
	cpu.R[4] = 3;
	uint8_t v = 0xff &~ (1<<3);
	test_ea_b(cpu, { 0004400 }, v, ea, v );
	BOOST_TEST(cpu.Z == true  );
}

BOOST_DATA_TEST_CASE(ea_by_imm, ea_writedn, ea) {
	uint8_t v = 0xff &~ (1<<3);
	test_ea_b(cpu, { 0004000, 3 }, v, ea, v );
	BOOST_TEST(cpu.Z == true  );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(btst_long, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = (1 | ((!z) << 22));
	test_cpu({ 0004000, 22});
	BOOST_TEST(cpu.Z == z  );
}

BOOST_AUTO_TEST_CASE(test_by_reg)
{
	cpu.R[3] = 0xffffffff &~ (1 << 22);
	cpu.R[2] = 22;
	test_cpu({ 0002403 });
	BOOST_TEST(cpu.Z == true  );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	cpu.R[3] = 0xffffffff &~ (1 << 22);
	test_cpu({ 0004003, 22});
	BOOST_TEST(cpu.Z == true  );

}
BOOST_AUTO_TEST_SUITE_END();
