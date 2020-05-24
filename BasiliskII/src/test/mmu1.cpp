#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "registers.hpp"
#include "exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

extern MC68040_MMU mmu;
void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(mmu_ittr, fixture);
BOOST_AUTO_TEST_CASE( UX )  {
	mmu.E = true;
	mmu.ITTR[0] = 0x0000C200;
	uint32_t c = mmu.ptest(0x2000, 4, false, 2);
	BOOST_TEST( c == 0x2203 );
}
BOOST_AUTO_TEST_CASE( base )  {
	mmu.E = true;
	mmu.ITTR[0] = 0x0300C000;
	mmu.ITTR[1] = 0x0000C000;
	uint32_t c = mmu.ptest(0x2000, 4, false, 2);
	BOOST_TEST( c == 0x2003 );
}

BOOST_AUTO_TEST_CASE( mask )  {
	mmu.E = true;
	mmu.ITTR[0] = 0xfffeC000;
	uint32_t c = mmu.ptest(0x01000000, 4, false, 2);
	BOOST_TEST( c == 0x01000003 );
}

BOOST_AUTO_TEST_CASE( W)  {
	mmu.E = true;
	mmu.ITTR[0] = 0x0000C004;
	uint32_t c = mmu.ptest(0x2000, 4, true, 2);
	BOOST_TEST( c == 0x2007 );
}

BOOST_AUTO_TEST_CASE( S)  {
	mmu.E = true;
	mmu.ITTR[0] = 0x0000A000;
	mmu.ITTR[1] = 0x0000C200;
	uint32_t c = mmu.ptest(0x2000, 4, true, 2);
	BOOST_TEST( c == 0x2203 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(mmu_dttr, fixture);
BOOST_AUTO_TEST_CASE( UX )  {
	mmu.E = true;
	mmu.DTTR[0] = 0x0000C200;
	uint32_t c = mmu.ptest(0x2000, 4, false, 1);
	BOOST_TEST( c == 0x2203 );
}
BOOST_AUTO_TEST_CASE( base )  {
	mmu.E = true;
	mmu.DTTR[0] = 0x0300C000;
	mmu.DTTR[1] = 0x0000C000;
	uint32_t c = mmu.ptest(0x2000, 4, false, 1);
	BOOST_TEST( c == 0x2003 );
}

BOOST_AUTO_TEST_CASE( mask )  {
	mmu.E = true;
	mmu.DTTR[0] = 0xfffeC000;
	uint32_t c = mmu.ptest(0x01000000, 4, false, 1);
	BOOST_TEST( c == 0x01000003 );
}

BOOST_AUTO_TEST_CASE( W)  {
	mmu.E = true;
	mmu.DTTR[0] = 0x0000C004;
	uint32_t c = mmu.ptest(0x2000, 4, true, 1);
	BOOST_TEST( c == 0x2007 );
}

BOOST_AUTO_TEST_CASE( S)  {
	mmu.E = true;
	mmu.DTTR[0] = 0x0000A000;
	mmu.DTTR[1] = 0x0000C200;
	uint32_t c = mmu.ptest(0x2000, 4, true, 1);
	BOOST_TEST( c == 0x2203 );
}
BOOST_AUTO_TEST_SUITE_END();
