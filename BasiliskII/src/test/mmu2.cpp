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

BOOST_FIXTURE_TEST_SUITE(mmu_3level, fixture);
BOOST_AUTO_TEST_CASE( B)  {
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0xffff0002); // ROOT+1
	uint32_t c = mmu.ptest(adr, 4, false, 1);
	BOOST_TEST( c == 1 << 11 );
}

BOOST_DATA_TEST_CASE( result, bdata::xrange(2), w)  {	
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40301); // PD
	uint32_t c = mmu.ptest(adr, 4, w, 1);
	BOOST_TEST( c == 0x40301 );
	BOOST_TEST( test_read32(0x10004) ==0x2000A );
	BOOST_TEST( test_read32(0x20008) ==0x3000A );
	BOOST_TEST( test_read32(0x3000C) ==(0x40309|w<<4) );
}

BOOST_AUTO_TEST_CASE( W1)  {
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40305); // PD
	uint32_t c = mmu.ptest(adr, 4, false, 1);
	BOOST_TEST( c == 0x40305 );
	BOOST_TEST( test_read32(0x10004) ==0x2000A );
	BOOST_TEST( test_read32(0x20008) ==0x3000A );
	BOOST_TEST( test_read32(0x3000C) ==0x4030D );
}

BOOST_AUTO_TEST_CASE( W2)  {
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30006); // PTD+2
	test_write32(0x3000C, 0x40301); // PD
	uint32_t c = mmu.ptest(adr, 4, false, 1);
	BOOST_TEST( c == 0x40305 );
	BOOST_TEST( test_read32(0x10004) ==0x2000A );
	BOOST_TEST( test_read32(0x20008) ==0x3000E );
	BOOST_TEST( test_read32(0x3000C) ==0x40309 );
}

BOOST_AUTO_TEST_CASE( W3)  {
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20006); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40301); // PD
	uint32_t c = mmu.ptest(adr, 4, false, 1);
	BOOST_TEST( c == 0x40305 );
	BOOST_TEST( test_read32(0x10004) ==0x2000E );
	BOOST_TEST( test_read32(0x20008) ==0x3000A );
	BOOST_TEST( test_read32(0x3000C) ==0x40309 );
}

BOOST_AUTO_TEST_CASE( S)  {	
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40381); // PD
	uint32_t c = mmu.ptest(adr, 4, false, 1);
	BOOST_TEST( c == 0x40381 );
	BOOST_TEST( test_read32(0x10004) ==0x2000A );
	BOOST_TEST( test_read32(0x20008) ==0x3000A );
	BOOST_TEST( test_read32(0x3000C) ==0x40389 );
}

BOOST_AUTO_TEST_CASE( srp)  {	
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.SRP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40381); // PD
	uint32_t c = mmu.ptest(adr, 4, false, 5);
	BOOST_TEST( c == 0x40381 );
	BOOST_TEST( test_read32(0x10004) ==0x2000A );
	BOOST_TEST( test_read32(0x20008) ==0x3000A );
	BOOST_TEST( test_read32(0x3000C) ==0x40389 );
}


BOOST_AUTO_TEST_CASE( read)  {	
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40301); // PD
	test_write32(0x40020, 0x12345678 ); // DATA
	BOOST_TEST( mmu.read_l(adr | 0x20) == 0x12345678 );
}


BOOST_AUTO_TEST_CASE( write )  {	
	uint32_t adr = 1 << 25 | 2 << 18 | 3 << 12;
	mmu.caches.clear();
	mmu.E = true;
	mmu.ITTR[0] = 0;
	mmu.ITTR[1] = 0;
	mmu.DTTR[0] = 0;
	mmu.DTTR[1] = 0;
	mmu.URP = 0x10000;
	test_write32(0x10004, 0x20002); // ROOT+1
	test_write32(0x20008, 0x30002); // PTD+2
	test_write32(0x3000C, 0x40301); // PD
	mmu.write_l(adr | 0x20, 0x12345678 );
	BOOST_TEST( test_read32(0x40020) == 0x12345678 );
}

BOOST_AUTO_TEST_SUITE_END();
