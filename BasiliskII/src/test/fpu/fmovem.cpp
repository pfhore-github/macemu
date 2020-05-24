#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <fenv.h>

long double read_ld(uint32_t addr);
void write_ld(uint32_t addr, long double ld);
void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(fmovem, fixture);
BOOST_AUTO_TEST_CASE(to_static)
{
	cpu.R[8+3] = 0x1000;
	FPU_R.FP[3] = 1.2;
	FPU_R.FP[5] = 2.3;
	test_cpu( {0171023, (uint16_t)(0xf000 | 0b00010100 ) } );
	BOOST_TEST( (double)read_ld( 0x1000) == 1.2 );	
	BOOST_TEST( (double)read_ld( 0x100C) == 2.3 );	
}

BOOST_AUTO_TEST_CASE(to_static_decr)
{
	cpu.R[8+3] = 0x1000 + 12*2;
	FPU_R.FP[2] = 1.2;
	FPU_R.FP[4] = 2.3;
	test_cpu( {0171043, (uint16_t)(0xe000 | 0b00010100 ) });
	BOOST_TEST( (double)read_ld(0x1000) == 1.2 );	
	BOOST_TEST( (double)read_ld(0x100C) == 2.3 );
	BOOST_TEST( cpu.R[8+3] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(to_dynamic)
{
	cpu.R[8+3] = 0x1000;
	FPU_R.FP[3] = 1.2;
	FPU_R.FP[5] = 2.3;
	cpu.R[2] = 0b00010100;
	test_cpu( {0171023, (uint16_t)(0xf800 | 2 << 4 ) } );
	BOOST_TEST( (double)read_ld(0x1000) == 1.2 );	
	BOOST_TEST( (double)read_ld(0x100C) == 2.3 );	
}

BOOST_AUTO_TEST_CASE(from_static)
{
	cpu.R[8+3] = 0x1000;
	write_ld(0x1000, 1.2);
	write_ld(0x100C, 2.3);
	test_cpu( {0171023, (uint16_t)(0xd000 | 0b00010100 ) } );
	BOOST_TEST( FPU_R.FP[3] == 1.2 );
	BOOST_TEST( FPU_R.FP[5] == 2.3 );
}
BOOST_AUTO_TEST_CASE(from_static_incr)
{
	cpu.R[8+3] = 0x1000;
	write_ld(0x1000, 1.2);
	write_ld(0x100C, 2.3);
	test_cpu( {0171033, (uint16_t)(0xd000 | 0b00010100 ) });
	BOOST_TEST( FPU_R.FP[3] == 1.2 );
	BOOST_TEST( FPU_R.FP[5] == 2.3 );
	BOOST_TEST( cpu.R[8+3] == 0x1000 + 12 * 2 );
}

BOOST_AUTO_TEST_CASE(from_dynamic)
{
	cpu.R[8+3] = 0x1000;
	write_ld(0x1000, 1.2);
	write_ld(0x100C, 2.3);
	cpu.R[2] = 0b00010100;
	test_cpu( {0171023, (uint16_t)(0xd800 | 2 << 4 ) } );
	BOOST_TEST( FPU_R.FP[3] == 1.2 );
	BOOST_TEST( FPU_R.FP[5] == 2.3 );
}

BOOST_AUTO_TEST_SUITE_END();
