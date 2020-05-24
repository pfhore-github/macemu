#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <fenv.h>

namespace bdata = boost::unit_test::data;

// nan,N,Z
bool cond_map[2][2][2][8] = {
	// T  EQ  GT  GE  LT  LE  GL  GLE
	{
		{
			{ false, false, true, true, false, false, true, true },
			{ false, true, false, true, false, true, false, true },
		},
		{
			{ false, false, false, false, true, true, true, true },
			{ false, true, false, true, false, true, false, true },
		},
	},
	{
		{
			{ false, false, false, false, false, false, false, false },
			{ false, true, false, true, false, true, false, false },
		},
		{
			{ false, false, false, false, false, false, false, false },
			{ false, true, false, true, false, true, false, false },
		},
	},
};

BOOST_FIXTURE_TEST_SUITE(fscc, fixture);
BOOST_DATA_TEST_CASE(cond, bdata::xrange(8)*
					 bdata::xrange(2)* bdata::xrange(2) * bdata::xrange(2),
					 cond, nan,n,z) {
	cpu.R[2] = 1;
	FPU_R.FPSR_cond = nan | n << 3 | z << 2;
	FPU_R.FPSR_ex &= ~ EX_BSUN;
	test_cpu( { 0171102, (uint16_t)cond } );
	BOOST_TEST( !( FPU_R.FPSR_ex & EX_BSUN) );
	BOOST_TEST( cpu.R[2] == (cond_map[nan][n][z][cond] ? 0xff : 0) );
}

BOOST_DATA_TEST_CASE(cond_r, bdata::xrange(8)*
					 bdata::xrange(2)* bdata::xrange(2) * bdata::xrange(2),
					 cond, nan,n,z) {
	cpu.R[2] = 1;
	FPU_R.FPSR_cond = nan | n << 3 | z << 2;
	FPU_R.FPSR_ex &= ~ EX_BSUN;
	test_cpu( { 0171102, (uint16_t)(15-cond) } );
	BOOST_TEST( !( FPU_R.FPSR_ex & EX_BSUN) );
	BOOST_TEST( cpu.R[2] == (cond_map[nan][n][z][cond] ? 0 : 0xff) );
}

BOOST_DATA_TEST_CASE(cond_bsun, bdata::xrange(8)*
					 bdata::xrange(2)* bdata::xrange(2) * bdata::xrange(2),
					 cond, nan,n,z) {
	cpu.R[2] = 1;
	FPU_R.FPSR_cond = nan | n << 3 | z << 2;
	FPU_R.FPSR_ex &= ~ EX_BSUN;
	test_cpu( { 0171102, (uint16_t)(cond+16) } );
	BOOST_TEST( !!( FPU_R.FPSR_ex & EX_BSUN) == nan);
	BOOST_TEST( cpu.R[2] == (cond_map[nan][n][z][cond] ? 0xff : 0) );
}

BOOST_DATA_TEST_CASE(cond_bsun_r, bdata::xrange(8)*
					 bdata::xrange(2)* bdata::xrange(2) * bdata::xrange(2),
					 cond, nan,n,z) {
	cpu.R[2] = 1;
	FPU_R.FPSR_cond = nan | n << 3 | z << 2;
	FPU_R.FPSR_ex &= ~ EX_BSUN;
	test_cpu( { 0171102, (uint16_t)(31-cond) } );
	BOOST_TEST( !!( FPU_R.FPSR_ex & EX_BSUN) == nan);
	BOOST_TEST( cpu.R[2] == (cond_map[nan][n][z][cond] ? 0 : 0xff) );
}


BOOST_AUTO_TEST_SUITE_END();
