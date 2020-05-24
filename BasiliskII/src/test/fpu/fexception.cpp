#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

BOOST_FIXTURE_TEST_SUITE(f_exception, fixture);

BOOST_AUTO_TEST_CASE(bsun) {
	FPU_R.FPSR_cond = 1;
	FPU_R.FPCR = 1 << 15;
	test_cpu( { 0171102, 31 } );
	test_in_exception(48);
}

BOOST_AUTO_TEST_CASE(snan)
{
	FPU_R.FPCR = 1 << 14;
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 0x7fff);
	test_write16(0x1002, 0);
	test_write32(0x1004, 0x80000001);
	test_write32(0x1008, 0);
	test_cpu( { 0171032, (uint16_t)(1 << 14 | 2 << 10 | 3 << 7 | 0x0 ) });
	test_in_exception(54);
}



BOOST_AUTO_TEST_SUITE_END();
