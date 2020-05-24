#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <ios>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(fmove, fixture);
BOOST_AUTO_TEST_CASE(from_b)
{
	cpu.R[8+2] = 0x1000;
	test_write8(0x1000, 23);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 6 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1001 );
	BOOST_TEST( (double)FPU_R.FP[3] == 23.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(from_w)
{
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 2300);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 4 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1002 );
	BOOST_TEST( (double)FPU_R.FP[3] == 2300.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(from_l)
{
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 230000);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 0 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	BOOST_TEST( (double)FPU_R.FP[3] == 2.3e5, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(from_s)
{
	cpu.R[8+2] = 0x1000;
	union iv_t {
		uint32_t i;
		float f;
	};
	iv_t i ;
	i.f = 0.4;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, i.i);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 1 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	BOOST_TEST( (double)FPU_R.FP[3] == 0.4f, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(from_d)
{
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x4004cccc);
	test_write32(0x1004, 0xcccccccc);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 5 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1008 );
	BOOST_TEST( (double)FPU_R.FP[3] == 2.6, boost::test_tools::tolerance(1e-4) );
}

BOOST_AUTO_TEST_CASE(from_ld)
{
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 0x3fff);
	test_write16(0x1002, 0);
	test_write32(0x1004, 0xA6666666);
	test_write32(0x1008, 0x66666666);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 2 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 1.3, boost::test_tools::tolerance(1e-4) );
	BOOST_TEST( cpu.R[8+2] == 0x100C );
}

BOOST_AUTO_TEST_CASE(from_p)
{
	// PREC = EXTENDED;
	FPU_R.FPCR = FPU_R.FPCR &~ (3 << 6);
	cpu.R[8+2] = 0x1000;
	test_write8(0x1000, 0x00);
	test_write8(0x1001, 0x07);
	test_write8(0x1002, 0x00);
	test_write8(0x1003, 0x01);
	test_write8(0x1004, 0x30);
	test_write8(0x1005, 0);
	test_write8(0x1006, 0);
	test_write8(0x1007, 0);
	test_write8(0x1008, 0);
	test_write8(0x1009, 0);
	test_write8(0x100A, 0);
	test_write8(0x100B, 0);
	
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 3 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( cpu.R[8+2] == 0x100C );
	BOOST_TEST( (double)FPU_R.FP[3] == 1.3e+7, boost::test_tools::tolerance(1e-4) );
}


BOOST_AUTO_TEST_CASE(from_reg)
{
	FPU_R.FP[2] = 1.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 1.2, boost::test_tools::tolerance(1e-5) );
}

BOOST_DATA_TEST_CASE(nan, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? NAN : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_nan) == tf );
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? INFINITY : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_i) == tf );
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? 0.0 : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond && cc_z) == tf );
}

BOOST_DATA_TEST_CASE(n, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? -1.0 : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x0 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond && cc_n) == tf );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fsmove, fixture);
BOOST_AUTO_TEST_CASE(from_b)
{
	cpu.R[8+2] = 0x1000;
	test_write8(0x1000, 23);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 6 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1001 );
	BOOST_TEST( (float)FPU_R.FP[3] == 23.0f, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_w)
{
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 2300);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 4 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1002 );
	BOOST_TEST( (float)FPU_R.FP[3] == 2300.0f, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_l)
{
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 230000);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 0 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	BOOST_TEST( (float)FPU_R.FP[3] == 2.3e5f, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_s)
{
	cpu.R[8+2] = 0x1000;
		union iv_t {
		uint32_t i;
		float f;
	};
	iv_t i ;
	i.f = 0.4;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, i.i);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 1 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	BOOST_TEST( (float)FPU_R.FP[3] == 0.4f, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_d)
{
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x4004cccc);
	test_write32(0x1004, 0xcccccccc);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 5 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1008 );
	BOOST_TEST( (float)FPU_R.FP[3] == 2.6f, boost::test_tools::tolerance(1e-4) );
}

BOOST_AUTO_TEST_CASE(from_ld)
{
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 0x3fff);
	test_write16(0x1002, 0);
	test_write32(0x1004, 0xA6666666);
	test_write32(0x1008, 0x66666666);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 2 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( (float)FPU_R.FP[3] == 1.3f, boost::test_tools::tolerance(1e-4) );
	BOOST_TEST( cpu.R[8+2] == 0x100C );
}

BOOST_AUTO_TEST_CASE(from_p)
{
	cpu.R[8+2] = 0x1000;
	test_write8(0x1000, 0x00);
	test_write8(0x1001, 0x07);
	test_write8(0x1002, 0x00);
	test_write8(0x1003, 0x01);
	test_write8(0x1004, 0x30);
	test_write8(0x1005, 0);
	test_write8(0x1006, 0);
	test_write8(0x1007, 0);
	test_write8(0x1008, 0);
	test_write8(0x1009, 0);
	test_write8(0x100A, 0);
	test_write8(0x100B, 0);
	
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 3 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( cpu.R[8+2] == 0x100C );
	BOOST_TEST( (float)FPU_R.FP[3] == 1.3e+7f, boost::test_tools::tolerance(1e-4) );
}


BOOST_AUTO_TEST_CASE(from_reg)
{
	FPU_R.FP[2] = 1.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( (float)FPU_R.FP[3] == 1.2f, boost::test_tools::tolerance(1e-3) );
}

BOOST_DATA_TEST_CASE(nan, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? NAN : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_nan) == tf );
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? INFINITY : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_i) == tf );
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? 0.0 : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_z) == tf );
}

BOOST_DATA_TEST_CASE(n, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? -1.0 : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x40 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_n) == tf );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fdmove, fixture);
BOOST_AUTO_TEST_CASE(from_b)
{
	cpu.R[8+2] = 0x1000;
	test_write8(0x1000, 23);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 6 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1001 );
	BOOST_TEST( (double)FPU_R.FP[3] == 23.0, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_w)
{
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 2300);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 4 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1002 );
	BOOST_TEST( (double)FPU_R.FP[3] == 2300.0, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_l)
{
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 230000);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 0 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	BOOST_TEST( (double)FPU_R.FP[3] == 2.3e5, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_s)
{
	cpu.R[8+2] = 0x1000;
		union iv_t {
		uint32_t i;
		float f;
	};
	iv_t i ;
	i.f = 0.4;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, i.i);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 1 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1004 );
	BOOST_TEST( (double)FPU_R.FP[3] == 0.4, boost::test_tools::tolerance(1e-3) );
}

BOOST_AUTO_TEST_CASE(from_d)
{
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x4004cccc);
	test_write32(0x1004, 0xcccccccc);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 5 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( cpu.R[8+2] == 0x1008 );
	BOOST_TEST( (double)FPU_R.FP[3] == 2.6, boost::test_tools::tolerance(1e-4) );
}

BOOST_AUTO_TEST_CASE(from_ld)
{
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 0x3fff);
	test_write16(0x1002, 0);
	test_write32(0x1004, 0xA6666666);
	test_write32(0x1008, 0x66666666);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 2 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 1.3, boost::test_tools::tolerance(1e-4) );
	BOOST_TEST( cpu.R[8+2] == 0x100C );
}

BOOST_AUTO_TEST_CASE(from_p)
{
	cpu.R[8+2] = 0x1000;
	test_write8(0x1000, 0x00);
	test_write8(0x1001, 0x07);
	test_write8(0x1002, 0x00);
	test_write8(0x1003, 0x01);
	test_write8(0x1004, 0x30);
	test_write8(0x1005, 0);
	test_write8(0x1006, 0);
	test_write8(0x1007, 0);
	test_write8(0x1008, 0);
	test_write8(0x1009, 0);
	test_write8(0x100A, 0);
	test_write8(0x100B, 0);
	
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 3 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( cpu.R[8+2] == 0x100C );
	BOOST_TEST( (double)FPU_R.FP[3] == 1.3e+7, boost::test_tools::tolerance(1e-4) );
}


BOOST_AUTO_TEST_CASE(from_reg)
{
	FPU_R.FP[2] = 1.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 1.2, boost::test_tools::tolerance(1e-3) );
}

BOOST_DATA_TEST_CASE(nan, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? NAN : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_nan) == tf );
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? INFINITY : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_i) == tf );
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? 0.0 : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( (FPU_R.FPSR_cond >> 2 & 1) == tf );
}

BOOST_DATA_TEST_CASE(n, bdata::xrange(2), tf)
{
	FPU_R.FP[2] = tf ? -1.0 : 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x44 ) });
	BOOST_TEST( (FPU_R.FPSR_cond >> 3 & 1) == tf );
}

BOOST_AUTO_TEST_SUITE_END();
