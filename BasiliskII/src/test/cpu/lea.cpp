#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(lea, fixture);

BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041722 } );
	BOOST_TEST( cpu.R[8+1] == 0x1000 );
}
BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041752, 0x100 } );
	BOOST_TEST( cpu.R[8+1] == 0x1100 );
}
BOOST_AUTO_TEST_CASE(d8anxn)
{
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 0x10;
	test_cpu( { 0041762, 0x0830 } );
	BOOST_TEST( cpu.R[8+1] == 0x1040 );
}

BOOST_AUTO_TEST_CASE(is_1_od_0)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041762, 0x0950 } );
	BOOST_TEST( cpu.R[8+1] == 0x100 );
}

BOOST_AUTO_TEST_CASE(is_1_od_1)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x100, 0x2000);
	test_cpu( { 0041762, 0x0951 } );
	BOOST_TEST( cpu.R[8+1] == 0x2000 );
}

BOOST_AUTO_TEST_CASE(is_1_od_2)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x100, 0x2000);
	test_cpu( { 0041762, 0x0952, 0x50 } );
	BOOST_TEST( cpu.R[8+1] == 0x2050 );
}

BOOST_AUTO_TEST_CASE(is_1_od_3)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x100, 0x2000);
	test_cpu( { 0041762, 0x0953, 0, 0x50 } );
	BOOST_TEST( cpu.R[8+1] == 0x2050 );
}

BOOST_AUTO_TEST_CASE(is_0_od_0)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041762, 0x0910 }  );
	BOOST_TEST( cpu.R[8+1] == 0x1100 );
}

BOOST_AUTO_TEST_CASE(is_0_od_1)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1100, 0x2000);
	test_cpu( { 0041762, 0x0911 }  );
	BOOST_TEST( cpu.R[8+1] == 0x2000 );
}

BOOST_AUTO_TEST_CASE(is_0_od_2)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1100, 0x2000);
	test_cpu( { 0041762, 0x0912, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5000 );
}

BOOST_AUTO_TEST_CASE(is_0_od_3)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1100, 0x2000);
	test_cpu( { 0041762, 0x0913, 0, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5000 );
}

BOOST_AUTO_TEST_CASE(is_0_od_5)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x2000);
	test_cpu( { 0041762, 0x0915 }  );
	BOOST_TEST( cpu.R[8+1] == 0x2100 );
}

BOOST_AUTO_TEST_CASE(is_0_od_6)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x2000);
	test_cpu( { 0041762, 0x0916, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5100 );
}

BOOST_AUTO_TEST_CASE(is_0_od_7)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x2000);
	test_cpu( { 0041762, 0x0917, 0, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5100 );
}

BOOST_AUTO_TEST_CASE(bd_2)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x6000, 0x2000);
	test_cpu( { 0041762, 0x0926, 0x5000, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5100 );
}

BOOST_AUTO_TEST_CASE(bd_3)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x6000, 0x2000);
	test_cpu( { 0041762, 0x0936, 0, 0x5000, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5100 );
}

BOOST_AUTO_TEST_CASE(bs_0)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_write32(0x6000, 0x2000);
	test_cpu( { 0041762, 0x09A6, 0x5000, 0x3000 }  );
	BOOST_TEST( cpu.R[8+1] == 0x5000 );
}

BOOST_DATA_TEST_CASE(sw, bdata::xrange(4), sc)
{
	cpu.R[0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041762, (uint16_t)(0x0950 | sc << 9)} );
	BOOST_TEST( cpu.R[8+1] == (0x100 << sc ));
}

BOOST_AUTO_TEST_CASE(wl_s)
{
	cpu.R[0] = -100 & 0xffff;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041762, 0x0150 } );
	BOOST_TEST( cpu.R[8+1] == -100);
}

BOOST_AUTO_TEST_CASE(index_an)
{
	cpu.R[8+0] = 0x100;
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0041762, 0x8950 } );
	BOOST_TEST( cpu.R[8+1] == 0x100 );
}



BOOST_AUTO_TEST_CASE(xxx_w)
{
	test_cpu( { 0041770, 0x1000 } );
	BOOST_TEST( cpu.R[8+1] == 0x1000 );	
}
BOOST_AUTO_TEST_CASE(xxx_l)
{
	test_cpu( { 0041771, 0, 0x1000 } );
	BOOST_TEST( cpu.R[8+1] == 0x1000 );	
}
BOOST_AUTO_TEST_CASE(d16pc)
{
	test_cpu( { 0041772, 0x1000 } );
	BOOST_TEST( cpu.R[8+1] == 0x1002 );	
}

BOOST_AUTO_TEST_CASE(d8pcxn)
{
	cpu.R[0] = 0x1000;
	test_cpu( { 0041773, 0x0830 } );
	BOOST_TEST( cpu.R[8+1] == 0x1032 );	
}

BOOST_AUTO_TEST_SUITE_END();
