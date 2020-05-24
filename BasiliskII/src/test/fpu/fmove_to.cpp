#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <ios>
namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(fmove, fixture);
BOOST_AUTO_TEST_CASE(to_b)
{
	FPU_R.FP[3] = 2.0;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (6<<10)|(3<<7) ) } );
	BOOST_TEST( test_read8(0x1000) == 2 );
	BOOST_TEST( cpu.R[8+2] == 0x1001 );
}
BOOST_AUTO_TEST_CASE(to_w)
{
	FPU_R.FP[3] = 1120.0;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (4<<10)|(3<<7) ) } );
	BOOST_TEST( test_read16(0x1000) == 1120 );
	BOOST_TEST( cpu.R[8+2] == 0x1002 );
}
BOOST_AUTO_TEST_CASE(to_l)
{
	FPU_R.FP[3] = 123456.0;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (0<<10)|(3<<7) ) } );
	BOOST_TEST( test_read32(0x1000) == 123456 );
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
}

BOOST_AUTO_TEST_CASE(to_s)
{
	FPU_R.FP[3] = 3.0f;	
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (1<<10)|(3<<7) ) } );
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	union iv_t {
		uint32_t i;
		float f;
	};
	iv_t v;
	v.i = test_read32(0x1000);
	BOOST_TEST( v.f == 3.0f );
}

BOOST_AUTO_TEST_CASE(to_d)
{
	FPU_R.FP[3] = 0x1.4ccccccccccccp+1;	
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (5<<10)|(3<<7) ) } );
	BOOST_TEST( cpu.R[8+2] == 0x1008 );
	BOOST_TEST( test_read32(0x1000) == 0x4004cccc);
	BOOST_TEST( test_read32(0x1004) == 0xcccccccc);
}

BOOST_AUTO_TEST_CASE(to_ld)
{
	FPU_R.FP[3] = 1.3l;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (2<<10)|(3<<7) ) } );
	BOOST_TEST( cpu.R[8+2] == 0x100C );
	BOOST_TEST( test_read16(0x1000) == 0x3fff);
	BOOST_TEST( test_read32(0x1004) == 0xa6666666 );
	BOOST_TEST( test_read32(0x1008) == 0x66666666 );
}

BOOST_DATA_TEST_CASE(to_p_inf, bdata::xrange(2), sign)
{
	FPU_R.FP[3] = sign ? -INFINITY : INFINITY;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( test_read16(0x1000) == (0x7fff | (sign << 15)));
	for(int i = 4; i < 12; ++i ) {
		BOOST_TEST(test_read8(0x1000+i) == 0);
	}
}

BOOST_DATA_TEST_CASE(to_p_nan, bdata::xrange(2), sign)
{
	FPU_R.FP[3] = sign ? -NAN : NAN;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( test_read16(0x1000) == (0x7fff | (sign << 15)));
	uint64_t payload = (uint64_t)(test_read32(0x1004)) << 32 |
		test_read32(0x1008);
	BOOST_TEST_MESSAGE("payload: " << std::hex << payload);
	BOOST_TEST( payload != 0 ); 
}

BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sign)
{
	FPU_R.FP[3] = sign ? -0.0 : 0.0;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( test_read16(0x1000) == sign << 15 );
	for(int i = 4; i < 12; ++i ) {
		BOOST_TEST(test_read8(0x1000+i) == 0);
	}
}

BOOST_DATA_TEST_CASE(SE, bdata::xrange(2), sign)
{
	FPU_R.FP[3] = sign ? -1e-4 : 1e4;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( (test_read16(0x1000) >> 14 & 1) == sign );
}

BOOST_AUTO_TEST_CASE(exp2)
{
	FPU_R.FP[3] = 1e11;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( (test_read16(0x1000) & 0xfff) == 0x11 );
}

BOOST_AUTO_TEST_CASE(exp3)
{
	FPU_R.FP[3] = 1e123;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( (test_read16(0x1000) & 0xfff) == 0x123 );
}

BOOST_AUTO_TEST_CASE(exp4)
{
	FPU_R.FP[3] = 1e1234L;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( (test_read16(0x1000) & 0xfff) == 0x234 );
	BOOST_TEST( (test_read8(0x1002) & 0xf0) == 0x10 );

}

BOOST_AUTO_TEST_CASE(k_5)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 5) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23460000 );
	BOOST_TEST( cpu.R[8+2] == 0x100C );
}

BOOST_AUTO_TEST_CASE(k_3)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 3) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23000000 );
}

BOOST_AUTO_TEST_CASE(k_1)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 1) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0 );
}

BOOST_AUTO_TEST_CASE(k_0)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 0) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23460000 );
}

BOOST_AUTO_TEST_CASE(k__1)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| ((-1)&0x7f)) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23457000 );
}

BOOST_AUTO_TEST_CASE(k__3)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| ((-3)&0x7f)) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23456790 );
}

BOOST_AUTO_TEST_CASE(k__5)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| ((-5)&0x7f)) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23456787 );
	BOOST_TEST( test_read8(0x1008) == 0x70 );
}

BOOST_AUTO_TEST_CASE(k_3r)
{

	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	cpu.R[2] = 3;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (7<<10)|(3<<7)| (2 << 4)) } );
	BOOST_TEST( (test_read8(0x1003) & 0xf) == 1 );
	BOOST_TEST( test_read32(0x1004) == 0x23000000 );
}

BOOST_AUTO_TEST_CASE(k_err)
{
	FPU_R.FP[3] = 12345.678765;
	cpu.R[8+2] = 0x1000;
	test_cpu( {0171032, (uint16_t)( 0x6000 | (3<<10)|(3<<7)| 20) } );
	BOOST_TEST( FPU_R.FPSR_ex & 1 << 5 );
}

BOOST_AUTO_TEST_SUITE_END();
