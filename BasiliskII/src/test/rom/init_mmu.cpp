#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROMWrapper;

BOOST_AUTO_TEST_CASE( M68040 )  {
	fixture f;
	cpu.S = true;
	MMU* mp = mmu;
	static MC68040_MMU test_mmu;
	mmu = &test_mmu;
	TEST_ROM( 04052 );
	BOOST_CHECK_EQUAL( test_mmu.movec_from(2), 0 );
	BOOST_CHECK_EQUAL( test_mmu.movec_from(3), 0 );
	BOOST_CHECK_EQUAL( test_mmu.movec_from(6), 0x807FC040 );
	BOOST_CHECK_EQUAL( test_mmu.movec_from(7), 0x500FC040 );
	mmu = mp;
}
