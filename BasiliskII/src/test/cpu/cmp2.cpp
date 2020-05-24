#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(cmp2_byte, fixture);
BOOST_AUTO_TEST_CASE(in_bound) {
	test_write8(0x1000, 3);
	test_write8(0x1001, 10);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 7;
	test_cpu( { 0000320, 0000000 } );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(on_bound) {
	test_write8(0x1000, 3);
	test_write8(0x1001, 10);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 3;
	test_cpu( { 0000320, 0000000 } );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.C == false );
}
BOOST_AUTO_TEST_CASE(out_bound) {
	test_write8(0x1000, 3);
	test_write8(0x1001, 10);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 2;
	test_cpu( { 0000320, 0000000 } );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.C == true );
}

BOOST_AUTO_TEST_CASE(mem) {
	test_write8(0x1000, 3);
	test_write8(0x1001, 10);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 7;
	test_cpu( { 0000322, 0000000 } );
	BOOST_TEST( cpu.C == false );
}


BOOST_AUTO_TEST_CASE(d16an) {
	test_write8(0x1010, 3);
	test_write8(0x1011, 10);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 4;
	test_cpu( { 0000352, 0000000, 0x10 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	test_write8(0x1040, 3);
	test_write8(0x1041, 10);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 0x10;
	cpu.R[1] = 4;
	test_cpu( { 0000362, 0010000, 0x0830 } );
	BOOST_TEST( cpu.C == false );

}

BOOST_AUTO_TEST_CASE(xxx_w) {
	test_write8( 0x1000, 3 );
	test_write8( 0x1001, 10 );
	cpu.R[0] = 4;
	test_cpu( { 0000370, 0000000, 0x1000 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	test_write8( 0x1000, 3 );
	test_write8( 0x1001, 10 );
	cpu.R[0] = 4;
	test_cpu( { 0000371, 0000000, 0, 0x1000 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d16pc) {
	test_write8( 0x14, 3 );
	test_write8( 0x15, 10 );
	cpu.R[0] = 4;
	test_cpu( { 0000372, 0000000, 0x10 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d8pcxn) {
	test_write8( 0x44, 3 );
	test_write8( 0x45, 10 );
	cpu.R[0] = 0x10;
	cpu.R[1] = 4;
	test_cpu( { 0000373, 0010000,0x0830 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmp2_word, fixture);
BOOST_AUTO_TEST_CASE(in_bound) {
	test_write16(0x1000, 300);
	test_write16(0x1002, 1000);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 700;
	test_cpu( { 0001320, 0000000 } );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(on_bound) {
	test_write16(0x1000, 300);
	test_write16(0x1002, 1000);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 300;
	test_cpu( { 0001320, 0000000 } );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.C == false );
}
BOOST_AUTO_TEST_CASE(out_bound) {
	test_write16(0x1000, 300);
	test_write16(0x1002, 1000);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 200;
	test_cpu( { 0001320, 0000000 } );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.C == true );
}

BOOST_AUTO_TEST_CASE(mem) {
	test_write16(0x1000, 300);
	test_write16(0x1002, 1000);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 700;
	test_cpu( { 0001322, 0000000 } );
	BOOST_TEST( cpu.C == false );
}


BOOST_AUTO_TEST_CASE(d16an) {
	test_write16(0x1010, 300);
	test_write16(0x1012, 1000);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 400;
	test_cpu( { 0001352, 0000000, 0x10 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	test_write16(0x1040, 300);
	test_write16(0x1042, 1000);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 0x10;
	cpu.R[1] = 400;
	test_cpu( { 0001362, 0010000, 0x0830 } );
	BOOST_TEST( cpu.C == false );

}

BOOST_AUTO_TEST_CASE(xxx_w) {
	test_write16( 0x1000, 300 );
	test_write16( 0x1002, 1000 );
	cpu.R[0] = 400;
	test_cpu( { 0001370, 0000000, 0x1000 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	test_write16( 0x1000, 300 );
	test_write16( 0x1002, 1000 );
	cpu.R[0] = 400;
	test_cpu( { 0001371, 0000000, 0, 0x1000 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d16pc) {
	test_write16( 0x14, 300 );
	test_write16( 0x16, 1000 );
	cpu.R[0] = 400;
	test_cpu( { 0001372, 0000000, 0x10 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d8pcxn) {
	test_write16( 0x44, 300 );
	test_write16( 0x46, 1000 );
	cpu.R[0] = 0x10;
	cpu.R[1] = 400;
	test_cpu( { 0001373, 0010000,0x0830 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(cmp2_long, fixture);

BOOST_AUTO_TEST_CASE(in_bound) {
	test_write32(0x1000, 30000);
	test_write32(0x1004, 100000);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 70000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0002320, 0000000 } ) );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(on_bound) {
	test_write32(0x1000, 30000);
	test_write32(0x1004, 100000);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 30000;
	test_cpu( { 0002320, 0000000 } );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.C == false );
}
BOOST_AUTO_TEST_CASE(out_bound) {
	test_write32(0x1000, 30000);
	test_write32(0x1004, 100000);
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 20000;
	test_cpu( { 0002320, 0000000 } );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.C == true );
}

BOOST_AUTO_TEST_CASE(mem) {
	test_write32(0x1000, 30000);
	test_write32(0x1004, 100000);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 70000;
	test_cpu( { 0002322, 0000000 } );
	BOOST_TEST( cpu.C == false );
}


BOOST_AUTO_TEST_CASE(d16an) {
	test_write32(0x1010, 30000);
	test_write32(0x1014, 100000);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 40000;
	test_cpu( { 0002352, 0000000, 0x10 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	test_write32(0x1040, 30000);
	test_write32(0x1044, 100000);
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 0x10;
	cpu.R[1] = 40000;
	test_cpu( { 0002362, 0010000, 0x0830 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(xxx_w) {
	test_write32( 0x1000, 30000 );
	test_write32( 0x1004, 100000 );
	cpu.R[0] = 40000;
	test_cpu( { 0002370, 0000000, 0x1000 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	test_write32( 0x1000, 30000 );
	test_write32( 0x1004, 100000 );
	cpu.R[0] = 40000;
	test_cpu( { 0002371, 0000000, 0, 0x1000 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d16pc) {
	test_write32( 0x14, 30000 );
	test_write32( 0x18, 100000 );
	cpu.R[0] = 40000;
	test_cpu( { 0002372, 0000000, 0x10 } );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(d8pcxn) {
	test_write32( 0x44, 30000 );
	test_write32( 0x48, 100000 );
	cpu.R[0] = 0x10;
	cpu.R[1] = 40000;
	test_cpu( { 0002373, 0010000,0x0830 } );
	BOOST_TEST( cpu.C == false );
}
BOOST_AUTO_TEST_SUITE_END();
