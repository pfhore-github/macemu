#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <ios>
namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(fp_store, fixture);
BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), sign)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(sign ? -INFINITY : INFINITY, ret, 3);
	BOOST_TEST(ret[0] == (0x7f | sign << 7));
	BOOST_TEST(ret[1] == 0xff);	
	for(int i = 4; i < 12; ++i ) {
		BOOST_TEST(ret[i] == 0);
	}
}

BOOST_DATA_TEST_CASE(nan, bdata::xrange(2), sign)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(sign ? -NAN : NAN, ret, 3);
	BOOST_TEST(ret[0] == (0x7f | sign << 7));
	BOOST_TEST(ret[1] == 0xff);
	uint64_t payload = 0;
	for(int i = 4; i < 12; ++i ) {
		payload |= (uint64_t)ret[i] << (64-(i-3)*8);
	}
	BOOST_TEST_MESSAGE("payload: " << std::hex << payload);
	BOOST_TEST( payload != 0 ); 
}

BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sign)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(sign ? -0.0 : 0.0, ret, 3);
	BOOST_TEST((ret[0] & 0x80) == sign << 7);
	for(int i = 4; i < 12; ++i ) {
		BOOST_TEST(ret[i] == 0);
	}
}

BOOST_DATA_TEST_CASE(SE, bdata::xrange(2), sign)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(sign ? 1e-4 : 1e4, ret, 3);
	BOOST_TEST((ret[0] & 0x40) == sign << 6);
}

BOOST_AUTO_TEST_CASE(exp2)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(1e11, ret, 3);
	BOOST_TEST((ret[0] & 0xf) == 0);
	BOOST_TEST(ret[1] == 0x11);
}

BOOST_AUTO_TEST_CASE(exp3)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(1e123, ret, 3);
	BOOST_TEST((ret[0] & 0xf) == 1);
	BOOST_TEST(ret[1] == 0x23);
}

BOOST_AUTO_TEST_CASE(exp4)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(1e1234L, ret, 3);
	BOOST_TEST((ret[0] & 0xf) == 2);
	BOOST_TEST(ret[1] == 0x34);
	BOOST_TEST((ret[2] & 0xf0)== 0x10);
}

BOOST_AUTO_TEST_CASE(k_5)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, 5);
	BOOST_TEST((ret[3] & 0xf) == 1);
	BOOST_TEST(ret[4] == 0x23);
	BOOST_TEST(ret[5] == 0x46);
	BOOST_TEST(ret[6] == 0);
}

BOOST_AUTO_TEST_CASE(k_3)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, 3);
	BOOST_TEST(ret[4] == 0x23);
	BOOST_TEST(ret[5] == 0);
}

BOOST_AUTO_TEST_CASE(k_1)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, 1);
	BOOST_TEST(ret[4] == 0);
}

BOOST_AUTO_TEST_CASE(k_0)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, 0);
	BOOST_TEST(ret[4] == 0x23);
	BOOST_TEST(ret[5] == 0x46);
	BOOST_TEST(ret[6] == 0);
}

BOOST_AUTO_TEST_CASE(k__1)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, -1);
	BOOST_TEST(ret[4] == 0x23);
	BOOST_TEST(ret[5] == 0x45);
	BOOST_TEST(ret[6] == 0x70);
}

BOOST_AUTO_TEST_CASE(k__3)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, -3);
	BOOST_TEST(ret[4] == 0x23);
	BOOST_TEST(ret[5] == 0x45);
	BOOST_TEST(ret[6] == 0x67);
	BOOST_TEST(ret[7] == 0x90);
}

BOOST_AUTO_TEST_CASE(k__5)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, -5);
	BOOST_TEST(ret[4] == 0x23);
	BOOST_TEST(ret[5] == 0x45);
	BOOST_TEST(ret[6] == 0x67);
	BOOST_TEST(ret[7] == 0x87);
	BOOST_TEST(ret[8] == 0x70);
}

BOOST_AUTO_TEST_CASE(k_err)
{
	uint8_t ret[13] = {};
	FPU_R.store_packed_decimal(12345.678765, ret, 20);
	BOOST_TEST( FPU_R.ex.OPERR );
}

BOOST_AUTO_TEST_SUITE_END();
