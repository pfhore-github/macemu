#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "../registers.hpp"
#include "../exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
extern CPU xcpu;
void test_cpu(const std::vector<uint16_t>& c);
BOOST_TEST_GLOBAL_FIXTURE(init);
BOOST_FIXTURE_TEST_SUITE(bclr_byte, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.X = xcpu.N = xcpu.V = xcpu.C = true;
	test_write8(0x1000, 0x80 | ((!z) << 4));
	xcpu.A[0] = 0x1000; 
	test_cpu({ 0004220, 4});
	BOOST_TEST( test_read8(0x1000) == 0x80 );
	BOOST_TEST(xcpu.X == true );
	BOOST_TEST(xcpu.N == true );
	BOOST_TEST(xcpu.V == true );
	BOOST_TEST(xcpu.C == true );
	BOOST_TEST(xcpu.Z == z  );
}

BOOST_DATA_TEST_CASE(test_by_reg,
					 bdata::xrange(8),
					 r
	)
{
	test_write8(0x1000, 0x80 | 1 << 4);
	xcpu.D[r] = 4;
	xcpu.A[0] = 0x1000; 
	test_cpu({ (uint16_t)( 0000620 | r << 9 )});
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_AUTO_TEST_CASE(test_by_imm)
{
	test_write8(0x1000, 0x80 |  1 << 3);
	xcpu.A[0] = 0x1000; 
	test_cpu({ 0004220, 3});
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_DATA_TEST_CASE(mem_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 sc, dst
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000; 
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000620 | sc << 9 | dst) });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_DATA_TEST_CASE(inc_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 sc, dst
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000; 
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000630 | sc << 9 | dst) });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001 ) );
}

BOOST_DATA_TEST_CASE(dec_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 sc, dst
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001; 
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000640 | sc << 9 | dst) });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 sc, dst
	)
{
	test_write8(0x1010, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000; 
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000650 | sc << 9 | dst), 0x10 });
	BOOST_TEST( test_read8(0x1010) == 0x80 );
}

BOOST_DATA_TEST_CASE(d8anxn_by_reg,
					 bdata::xrange(1,8) * bdata::xrange(8),
					 sc, dst
	)
{
	test_write8(0x1040, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000660 | sc << 9 | dst), 0x0830 });
	BOOST_TEST( test_read8(0x1040) == 0x80 );
}

BOOST_DATA_TEST_CASE(xxxw_by_reg,
					 bdata::xrange(8),
					 sc
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000670 | sc << 9 ), 0x1000 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_DATA_TEST_CASE(xxxl_by_reg,
					 bdata::xrange(8),
					 sc
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.D[sc] = 3;
	test_cpu({ (uint16_t)(0000671 | sc << 9 ), 0, 0x1000 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_DATA_TEST_CASE(mem_by_imm,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000; 
	test_cpu({ (uint16_t)(0004220 | dst), 3 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_DATA_TEST_CASE(inc_by_imm,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000; 
	test_cpu({ (uint16_t)(0004230 | dst), 3 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001 ) );
}

BOOST_DATA_TEST_CASE(dec_by_imm,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001; 
	test_cpu({ (uint16_t)(0004240 | dst), 3 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16_by_imm,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1010, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000; 
	test_cpu({ (uint16_t)(0004250 | dst), 3, 0x10 });
	BOOST_TEST( test_read8(0x1010) == 0x80 );
}

BOOST_DATA_TEST_CASE(d8anxn_by_imm,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1040, 0x80 | 1 << 3);	
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu({ (uint16_t)(0004260 | dst), 3, 0x0830 });
	BOOST_TEST( test_read8(0x1040) == 0x80 );
}

BOOST_AUTO_TEST_CASE(xxxw_by_imm)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	test_cpu({ 0004270 , 3, 0x1000 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_AUTO_TEST_CASE(xxxl_by_imm)
{
	test_write8(0x1000, 0x80 | 1 << 3);	
	test_cpu({ 0004271, 3, 0, 0x1000 });
	BOOST_TEST( test_read8(0x1000) == 0x80 );
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(bclr_long, fixture);
BOOST_DATA_TEST_CASE(result,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.X = xcpu.N = xcpu.V = xcpu.C = true;
	xcpu.D[0] = (1 | ((!z) << 22));
	test_cpu({ 0004200, 22});
	BOOST_TEST( xcpu.D[0] == 1 );
	BOOST_TEST(xcpu.X == true );
	BOOST_TEST(xcpu.N == true );
	BOOST_TEST(xcpu.V == true );
	BOOST_TEST(xcpu.C == true );
	BOOST_TEST(xcpu.Z == z  );
}

BOOST_DATA_TEST_CASE(test_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 sc, dst
	)
{
	xcpu.D[dst] = 1 | 1 << 22;
	xcpu.D[sc] = 22;
	test_cpu({ (uint16_t)( 0000600 | sc << 9 | dst) });
	if( dst == sc ) {
		BOOST_TEST( xcpu.D[sc] == 22 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 1 );
	}
}

BOOST_DATA_TEST_CASE(test_by_imm,
					 bdata::xrange(8),
					 dst
	)
{
	xcpu.D[dst] = 1 | 1 << 22;
	test_cpu({ (uint16_t)(0004200 | dst), 22});
	xcpu.D[dst] = 1;

}
BOOST_AUTO_TEST_SUITE_END();
