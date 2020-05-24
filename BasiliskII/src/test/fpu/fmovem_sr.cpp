#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <fenv.h>
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(fmovem_cc, fixture);
BOOST_DATA_TEST_CASE(from, bdata::xrange(2)*bdata::xrange(2)*bdata::xrange(2),
					 fpcr, fpsr, fpiar)
{
	cpu.R[8+2] = 0x1000;
	// FPCR
	fesetround( FE_DOWNWARD ); // 2[4]
	FPU_R.PREC = SINGLE; // 1[6]
	FPU_R.ex_enable.DZ = FPU_R.ex_enable.snan = true; //10;14

	// FPSR
	FPU_R.I = true; // 25
	FPU_R.quotient = 12; // 16
	FPU_R.ex.OVFL = true; // 12
	FPU_R.acc.DZ = true; // 4
	cpu.R[8+2] = 0x1000;	
	test_cpu( {0171032, (uint16_t)( 0xA000 | fpcr << 12 | fpsr << 11 | fpiar << 10) } );
	BOOST_TEST( cpu.R[8+2] == 0x1000 + 4 * ( fpcr+fpsr+fpiar) );
	uint32_t addr = 0x1000;
	if( fpcr ) {
		BOOST_TEST( test_read32(addr) == (1<<14|1<<10|1<<6|2<<4));
		addr += 4;
	}
	if( fpsr ) {
		BOOST_TEST( test_read32(addr) == (1<<25|12<<16|1<<12|1<<4));
		addr += 4;
	}
	if( fpiar ) {
		BOOST_TEST( test_read32(addr) == 0);
	}
		
}

BOOST_DATA_TEST_CASE(to, bdata::xrange(2)*bdata::xrange(2)*bdata::xrange(2),
					 fpcr, fpsr, fpiar)
{
	cpu.R[8+2] = 0x1000;
	uint32_t addr = 0x1000;
	if( fpcr ) {
		test_write32(addr, (1<<14|1<<10|1<<6|2<<4));
		addr += 4;
	}
	if( fpsr ) {
		test_write32(addr, (1<<25|12<<16|1<<12|1<<4));
		addr += 4;
	}
	if( fpiar ) {
		test_write32(addr, 0x1200);
	}

	// FPCR
	fesetround( FE_UPWARD ); // 2[4]
	FPU_R.PREC = DOUBLE; // 1[6]
	FPU_R.ex_enable.DZ = FPU_R.ex_enable.snan = false; //10;14

	// FPSR
	FPU_R.I = false; // 25
	FPU_R.quotient = 4; // 16
	FPU_R.ex.OVFL = false; // 12
	FPU_R.acc.DZ = false; // 4

	test_cpu( {0171032, (uint16_t)( 0x8000 | fpcr << 12 | fpsr << 11 | fpiar << 10) } );
	BOOST_TEST( cpu.R[8+2] == 0x1000 + 4 * ( fpcr+fpsr+fpiar) );

	// FPCR
	if( fpcr ) {
		BOOST_TEST( fegetround() == FE_DOWNWARD ); // 2[4]
		BOOST_TEST( FPU_R.PREC == SINGLE); // 1[6]
		BOOST_TEST( FPU_R.ex_enable.DZ == true );
		BOOST_TEST( FPU_R.ex_enable.snan == true); //10;14
	}
	// FPSR
	if( fpsr ) {
		BOOST_TEST( FPU_R.I == true); // 25
		BOOST_TEST( FPU_R.quotient == 12); // 16
		BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL); // 12
		BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_DZ); // 4
	}
	if(fpiar) {
		BOOST_TEST( FPU_R.FPIAR == 0x1200 );
	}
}

BOOST_AUTO_TEST_SUITE_END();
