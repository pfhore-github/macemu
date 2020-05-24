#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;



BOOST_FIXTURE_TEST_SUITE(movem_to_word, fixture);
BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+2] = 0x1000;
	cpu.R[3] = 0x1234;
	cpu.R[6] = 0xaaaa;
	cpu.R[8+4] = 0xbdbd;
	cpu.R[8+6] = 0xffff;
	test_cpu( { 0044222, 0b0101000001001000 } );
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
	BOOST_TEST( test_read16(0x1002) == 0xaaaa );
	BOOST_TEST( test_read16(0x1004) == 0xbdbd );
	BOOST_TEST( test_read16(0x1006) == 0xffff );
}

BOOST_AUTO_TEST_CASE(decr) {
	cpu.R[8+2] = 0x1008;
	cpu.R[3] = 0x1234;
	cpu.R[6] = 0xaaaa;
	cpu.R[8+4] = 0xbdbd;
	cpu.R[8+6] = 0xffff;
	test_cpu( { 0044242, 0b0001001000001010 } );
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
	BOOST_TEST( test_read16(0x1002) == 0xaaaa );
	BOOST_TEST( test_read16(0x1004) == 0xbdbd );
	BOOST_TEST( test_read16(0x1006) == 0xffff );
	BOOST_TEST( cpu.R[8+2] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+2] = 0x1000;
	cpu.R[3] = 0x1234;
	cpu.R[6] = 0xaaaa;
	cpu.R[8+4] = 0xbdbd;
	cpu.R[8+6] = 0xffff;
	test_cpu( { 0044252, 0b0101000001001000, 0x50 } );
	BOOST_TEST( test_read16(0x1050) == 0x1234 );
	BOOST_TEST( test_read16(0x1052) == 0xaaaa );
	BOOST_TEST( test_read16(0x1054) == 0xbdbd );
	BOOST_TEST( test_read16(0x1056) == 0xffff );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	cpu.R[0] = 0x10;
	cpu.R[8+2] = 0x1000;
	cpu.R[3] = 0x1234;
	cpu.R[6] = 0xaaaa;
	cpu.R[8+4] = 0xbdbd;
	cpu.R[8+6] = 0xffff;
	test_cpu( { 0044262, 0b0101000001001000, 0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 0x1234 );
	BOOST_TEST( test_read16(0x1042) == 0xaaaa );
	BOOST_TEST( test_read16(0x1044) == 0xbdbd );
	BOOST_TEST( test_read16(0x1046) == 0xffff );
}

BOOST_AUTO_TEST_CASE(xxx_w) {
	cpu.R[3] = 0x1234;
	cpu.R[6] = 0xaaaa;
	cpu.R[8+4] = 0xbdbd;
	cpu.R[8+6] = 0xffff;
	test_cpu( { 0044270, 0b0101000001001000, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
	BOOST_TEST( test_read16(0x1002) == 0xaaaa );
	BOOST_TEST( test_read16(0x1004) == 0xbdbd );
	BOOST_TEST( test_read16(0x1006) == 0xffff );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	cpu.R[3] = 0x1234;
	cpu.R[6] = 0xaaaa;
	cpu.R[8+4] = 0xbdbd;
	cpu.R[8+6] = 0xffff;
	test_cpu( { 0044271, 0b0101000001001000, 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
	BOOST_TEST( test_read16(0x1002) == 0xaaaa );
	BOOST_TEST( test_read16(0x1004) == 0xbdbd );
	BOOST_TEST( test_read16(0x1006) == 0xffff );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(movem_to_long, fixture);
BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+2] = 0x1000;
	cpu.R[3] = 0x12345678;
	cpu.R[6] = 0xaaaaaaaa;
	cpu.R[8+4] = 0xbdbdcaca;
	cpu.R[8+6] = 0xffffffff;
	test_cpu( { 0044322, 0b0101000001001000 } );
	BOOST_TEST( test_read32(0x1000) == 0x12345678 );
	BOOST_TEST( test_read32(0x1004) == 0xaaaaaaaa );
	BOOST_TEST( test_read32(0x1008) == 0xbdbdcaca );
	BOOST_TEST( test_read32(0x100C) == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(decr) {
	cpu.R[8+2] = 0x1010;
	cpu.R[3] = 0x12345678;
	cpu.R[6] = 0xaaaaaaaa;
	cpu.R[8+4] = 0xbdbdcaca;
	cpu.R[8+6] = 0xffffffff;
	test_cpu( { 0044342, 0b0001001000001010 } );
	BOOST_TEST( test_read32(0x1000) == 0x12345678 );
	BOOST_TEST( test_read32(0x1004) == 0xaaaaaaaa );
	BOOST_TEST( test_read32(0x1008) == 0xbdbdcaca );
	BOOST_TEST( test_read32(0x100C) == 0xffffffff );
	BOOST_TEST( cpu.R[8+2] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+2] = 0x1000;
	cpu.R[3] = 0x12345678;
	cpu.R[6] = 0xaaaaaaaa;
	cpu.R[8+4] = 0xbdbdcaca;
	cpu.R[8+6] = 0xffffffff;
	test_cpu( { 0044352, 0b0101000001001000, 0x50 } );
	BOOST_TEST( test_read32(0x1050) == 0x12345678 );
	BOOST_TEST( test_read32(0x1054) == 0xaaaaaaaa );
	BOOST_TEST( test_read32(0x1058) == 0xbdbdcaca );
	BOOST_TEST( test_read32(0x105C) == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	cpu.R[0] = 0x10;
	cpu.R[8+2] = 0x1000;
	cpu.R[3] = 0x12345678;
	cpu.R[6] = 0xaaaaaaaa;
	cpu.R[8+4] = 0xbdbdcaca;
	cpu.R[8+6] = 0xffffffff;
	test_cpu( { 0044362, 0b0101000001001000, 0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 0x12345678 );
	BOOST_TEST( test_read32(0x1044) == 0xaaaaaaaa );
	BOOST_TEST( test_read32(0x1048) == 0xbdbdcaca );
	BOOST_TEST( test_read32(0x104C) == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(xxx_w) {
	cpu.R[3] = 0x12345678;
	cpu.R[6] = 0xaaaaaaaa;
	cpu.R[8+4] = 0xbdbdcaca;
	cpu.R[8+6] = 0xffffffff;
	test_cpu( { 0044370, 0b0101000001001000, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 0x12345678 );
	BOOST_TEST( test_read32(0x1004) == 0xaaaaaaaa );
	BOOST_TEST( test_read32(0x1008) == 0xbdbdcaca );
	BOOST_TEST( test_read32(0x100C) == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	cpu.R[3] = 0x12345678;
	cpu.R[6] = 0xaaaaaaaa;
	cpu.R[8+4] = 0xbdbdcaca;
	cpu.R[8+6] = 0xffffffff;
	test_cpu( { 0044371, 0b0101000001001000, 0, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 0x12345678 );
	BOOST_TEST( test_read32(0x1004) == 0xaaaaaaaa );
	BOOST_TEST( test_read32(0x1008) == 0xbdbdcaca );
	BOOST_TEST( test_read32(0x100C) == 0xffffffff );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(movem_from_word, fixture);
BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+2] = 0x1000;

	test_write16(0x1000, 0x1234);
	test_write16(0x1002, 0xaaaa);
	test_write16(0x1004, 0xbdbd);
	test_write16(0x1006, 0xffff);

	test_cpu( { 0046222, 0b0101000001001000 } );
	BOOST_TEST( cpu.R[3] == 0x1234 );
	BOOST_TEST( cpu.R[6] == 0xaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbd );
	BOOST_TEST( cpu.R[8+6] == 0xffff );
}
BOOST_AUTO_TEST_CASE(incr) {
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 0x1234);
	test_write16(0x1002, 0xaaaa);
	test_write16(0x1004, 0xbdbd);
	test_write16(0x1006, 0xffff);
	test_cpu( { 0046232, 0b0101000001001000 } );
	BOOST_TEST( cpu.R[3] == 0x1234 );
	BOOST_TEST( cpu.R[6] == 0xaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbd );
	BOOST_TEST( cpu.R[8+6] == 0xffff );
	BOOST_TEST( cpu.R[8+2] == 0x1008 );
}

BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+2] = 0x1000;
	test_write16(0x1050, 0x1234);
	test_write16(0x1052, 0xaaaa);
	test_write16(0x1054, 0xbdbd);
	test_write16(0x1056, 0xffff);
	test_cpu( { 0046252, 0b0101000001001000, 0x50 } );
	BOOST_TEST( cpu.R[3] == 0x1234 );
	BOOST_TEST( cpu.R[6] == 0xaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbd );
	BOOST_TEST( cpu.R[8+6] == 0xffff );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	cpu.R[0] = 0x10;
	cpu.R[8+2] = 0x1000;
	test_write16(0x1040, 0x1234);
	test_write16(0x1042, 0xaaaa);
	test_write16(0x1044, 0xbdbd);
	test_write16(0x1046, 0xffff);
	test_cpu( { 0046262, 0b0101000001001000, 0x0830 } );
	BOOST_TEST( cpu.R[3] == 0x1234 );
	BOOST_TEST( cpu.R[6] == 0xaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbd );
	BOOST_TEST( cpu.R[8+6] == 0xffff );
}

BOOST_AUTO_TEST_CASE(xxx_w) {
	test_write16(0x1000, 0x1234);
	test_write16(0x1002, 0xaaaa);
	test_write16(0x1004, 0xbdbd);
	test_write16(0x1006, 0xffff);
	test_cpu( { 0046270, 0b0101000001001000, 0x1000 } );
	BOOST_TEST( cpu.R[3] == 0x1234 );
	BOOST_TEST( cpu.R[6] == 0xaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbd );
	BOOST_TEST( cpu.R[8+6] == 0xffff );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	test_write16(0x1000, 0x1234);
	test_write16(0x1002, 0xaaaa);
	test_write16(0x1004, 0xbdbd);
	test_write16(0x1006, 0xffff);
	test_cpu( { 0046271, 0b0101000001001000, 0, 0x1000 } );
	BOOST_TEST( cpu.R[3] == 0x1234 );
	BOOST_TEST( cpu.R[6] == 0xaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbd );
	BOOST_TEST( cpu.R[8+6] == 0xffff );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(movem_from_long, fixture);
BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+2] = 0x1000;

	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0xaaaaaaaa);
	test_write32(0x1008, 0xbdbdeaea);
	test_write32(0x100C, 0xffffffff);

	test_cpu( { 0046322, 0b0101000001001000 } );
	BOOST_TEST( cpu.R[3] == 0x12345678 );
	BOOST_TEST( cpu.R[6] == 0xaaaaaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbdeaea );
	BOOST_TEST( cpu.R[8+6] == 0xffffffff );
}
BOOST_AUTO_TEST_CASE(incr) {
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0xaaaaaaaa);
	test_write32(0x1008, 0xbdbdeaea);
	test_write32(0x100C, 0xffffffff);
	test_cpu( { 0046332, 0b0101000001001000 } );
	BOOST_TEST( cpu.R[3] == 0x12345678 );
	BOOST_TEST( cpu.R[6] == 0xaaaaaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbdeaea );
	BOOST_TEST( cpu.R[8+6] == 0xffffffff );
	BOOST_TEST( cpu.R[8+2] == 0x1010 );
}

BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+2] = 0x1000;
	test_write32(0x1050, 0x12345678);
	test_write32(0x1054, 0xaaaaaaaa);
	test_write32(0x1058, 0xbdbdeaea);
	test_write32(0x105C, 0xffffffff);
	test_cpu( { 0046352, 0b0101000001001000, 0x50 } );
	BOOST_TEST( cpu.R[3] == 0x12345678 );
	BOOST_TEST( cpu.R[6] == 0xaaaaaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbdeaea );
	BOOST_TEST( cpu.R[8+6] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(d8anxn) {
	cpu.R[0] = 0x10;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1040, 0x12345678);
	test_write32(0x1044, 0xaaaaaaaa);
	test_write32(0x1048, 0xbdbdeaea);
	test_write32(0x104C, 0xffffffff);
	test_cpu( { 0046362, 0b0101000001001000, 0x0830 } );
	BOOST_TEST( cpu.R[3] == 0x12345678 );
	BOOST_TEST( cpu.R[6] == 0xaaaaaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbdeaea );
	BOOST_TEST( cpu.R[8+6] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(xxx_w) {
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0xaaaaaaaa);
	test_write32(0x1008, 0xbdbdeaea);
	test_write32(0x100C, 0xffffffff);
	test_cpu( { 0046370, 0b0101000001001000, 0x1000 } );
	BOOST_TEST( cpu.R[3] == 0x12345678 );
	BOOST_TEST( cpu.R[6] == 0xaaaaaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbdeaea );
	BOOST_TEST( cpu.R[8+6] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(xxx_l) {
	test_write32(0x1000, 0x12345678);
	test_write32(0x1004, 0xaaaaaaaa);
	test_write32(0x1008, 0xbdbdeaea);
	test_write32(0x100C, 0xffffffff);
	test_cpu( { 0046371, 0b0101000001001000, 0, 0x1000 } );
	BOOST_TEST( cpu.R[3] == 0x12345678 );
	BOOST_TEST( cpu.R[6] == 0xaaaaaaaa );
	BOOST_TEST( cpu.R[8+4] == 0xbdbdeaea );
	BOOST_TEST( cpu.R[8+6] == 0xffffffff );
}
BOOST_AUTO_TEST_SUITE_END();
