#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "mem.hpp"
#include "glu.hpp"
#include "v8.hpp"
namespace bdata = boost::unit_test::data;
extern uint32_t RAMSize;
using namespace ROM;
namespace ROM {
void getHWInfo(uint8_t v) { MOCK::invoke<void>("getHWInfo", v); }
check_simm_size_t check_simm_size(uint32_t base, uint32_t end, uint32_t data, uint32_t unit) {
	return MOCK::invoke<check_simm_size_t>("check_simm_size", base, end, data, unit);
}
std::bitset<4> check_simm(uint32_t base, uint32_t check) { return MOCK::invoke<std::bitset<4>>("check_simm", base ,check); }
}
void prepare(std::unique_ptr<Machine>&& m) {
	fixture f(std::move(m));
	delete [] RAMBaseHost;
	RAMSize = 0x14000000; // 320MB
	RAMBaseHost = new uint8_t[RAMSize];
	DEFINE_ROM( 02F64 );		// getHWinfo
	DEFINE_ROM( A0242 );		// check_simm_size
	DEFINE_ROM( A0286 );		// check_simm
	MOCK::reset_all();
	MOCK::make<void(uint8_t)>("getHWInfo")
		->always_do( [](uint8_t) { } );
	MAKE_MOCK(getHWInfo);
	MAKE_MOCK(check_simm_size);
	MAKE_MOCK(check_simm);
	clear_global();
}
// 4B0C
constexpr uint32_t CHECK_DATA = 0x54696E61;
void declare_mock_ok(const RAM_SLOT_T& rams, unsigned slot, uint32_t var_size, uint32_t fix_size) {
	for(unsigned int i = 0; i < rams.ranges.size(); ++i ) {
		GET_MOCK(check_simm_size)
			.nth_is(i+1, { slot == i ? var_size : fix_size, 0u },
					rams.ranges[i].first, rams.ranges[i].second, CHECK_DATA, rams.unit );
		GET_MOCK(check_simm)
			.nth_is(i+1, std::bitset<4>("0000"), rams.ranges[i].first, CHECK_DATA );
	}
}

void declare_mock_ng(const RAM_SLOT_T& rams, unsigned slot, uint32_t fix_size, unsigned int n) {
	for(unsigned int i = 0, j = 0; i < rams.ranges.size(); ++i ) {
		if( slot != i ) {
			GET_MOCK(check_simm_size)
				.nth_is(++j, { fix_size, 0u },
						rams.ranges[i].first, rams.ranges[i].second, CHECK_DATA, rams.unit );
		}
		GET_MOCK(check_simm)
			.nth_is(i+1, std::bitset<4>( slot == i ? 1u << n : 0u ), rams.ranges[i].first, CHECK_DATA );
	}
}
std::ostream& operator<<(std::ostream& os, const SIMM& simm) {
	os << "SIMM " << (simm.sz/1_mb) << " MB";
	return os;
}

BOOST_AUTO_TEST_SUITE( glue )
BOOST_DATA_TEST_CASE( success, bdata::xrange(2) * simm30,
					  slot, simm ) {
	prepare(std::make_unique<GLU>());
	uint32_t sz1 = simm.sz;
	constexpr uint32_t sz2 = 0x100000;
	declare_mock_ok(ram_tables[0], slot, sz1, sz2);
	AR(1) = 0x40803688;
	TEST_ROM2( A01C0, A02BA );
	BOOST_CHECK_EQUAL( DR(0), sz1+sz2 );
	BOOST_CHECK_EQUAL( (DR(5)>>(slot*4)) & 0xf, simm.bit );
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( fail, bdata::xrange(2) * bdata::xrange(4),
					  slot, i ) {
	prepare(std::make_unique<GLU>());
	constexpr uint32_t sz = 0x100000;
	declare_mock_ng(ram_tables[0], slot, sz, i );
	AR(1) = 0x40803688;
	TEST_ROM( A01C0 );
	BOOST_CHECK_EQUAL( DR(0), sz );
	BOOST_CHECK_EQUAL( (DR(5)>>(slot*4)) & 0xf, 0 );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( v8 )
BOOST_DATA_TEST_CASE( success, bdata::xrange(2) * simm30,
					  slot, simm ) {
	prepare(std::make_unique<V8>());
	uint32_t sz1 = simm.sz;
	constexpr uint32_t sz2 = 0x100000;
	declare_mock_ok(ram_tables[1], slot, sz1, sz2);
	AR(1) = 0x40803880;
	TEST_ROM2( A01C0, A02BA );
	BOOST_CHECK_EQUAL( DR(0), sz1+sz2 );
	BOOST_CHECK_EQUAL( (DR(5)>>(slot*4)) & 0xf, simm.bit );
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( fail, bdata::xrange(2) * bdata::xrange(4),
					  slot, i ) {
	prepare(std::make_unique<V8>());
	constexpr uint32_t sz = 0x100000;
	declare_mock_ng(ram_tables[1], slot, sz, i );

	AR(1) = 0x40803880;
	TEST_ROM( A01C0 );
	BOOST_CHECK_EQUAL( DR(0), sz );
	BOOST_CHECK_EQUAL( (DR(5)>>(slot*4)) & 0xf, 0 );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()
