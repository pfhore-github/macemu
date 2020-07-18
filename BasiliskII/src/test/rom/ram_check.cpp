#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;
extern uint8_t *RAMBaseHost;
extern uint32_t RAMSize;
#include "via2.hpp"
#include "glu.hpp"
#include "oss.hpp"
#include "rbv.hpp"
#include "mcu.hpp"
#include "mdu.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "v8.hpp"
#include "memcjr.hpp"
namespace ROM {
ram_result check_simm_slots(const RAM_SLOT_T* ram_slots) { return MOCK::invoke<ram_result>("check_simm_slots", ram_slots); }
void getHWInfo(uint8_t v) { MOCK::invoke<void>("getHWInfo", v); }
check_simm_size_t check_simm_size(uint32_t base, uint32_t end, uint32_t data, uint32_t unit) {
	return MOCK::invoke<check_simm_size_t>("check_simm_size", base, end, data, unit);
}
}
static std::function<void()> get_hw_info_mock;
using namespace ROM;
template<class T>
void prepare(const MACHINE_TABLE_T* mc, const MOTHERBOARD_TABLE_T* mb) {
	fixture f(std::make_unique<T>());
	delete [] RAMBaseHost;
	RAMSize = 0x14000000; // 320MB
	RAMBaseHost = new uint8_t[RAMSize];
	DEFINE_ROM( 02F64 );		// getHWinfo
	DEFINE_ROM( A01C6 );		// check_simm_slots
	DEFINE_ROM( A0242 );		// check_simm_size
	MOCK::reset_all();
	MOCK::make<void(uint8_t)>("getHWInfo")
		->always_do( [](uint8_t) { get_hw_info_mock(); } );

	get_hw_info_mock =
		[mc, mb]() {
			model = mc;
			motherboard = mb;
			INIT_HW_FLG = mc->init_flg;
			if( INIT_HW_FLG.none() ) {
				INIT_HW_FLG = mb->init_flg;
			}
			GEN = mc->generation;
		};
    MAKE_MOCK(check_simm_slots);
	MAKE_MOCK(check_simm_size); // for V8

	clear_global();
}

std::ostream& operator<<(std::ostream& os, const SIMM& simm) {
	os << "SIMM " << (simm.sz/1_mb) << " MB";
	return os;
}
constexpr uint32_t CHECK_DATA = 0x54696E61;
BOOST_AUTO_TEST_SUITE( glue )

BOOST_DATA_TEST_CASE( success_bankA, simm30, simm1 ) {
	
	prepare<GLU>( &old_machines[0], &motherboards[0]);
	uint32_t sz = simm1.sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm1]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, simm1.bit, true };
					});
	auto via2 = std::make_shared<IO_TEST<VIA2>>(0);
	via2->set_read_data( VIA_REG::RA, { 0x3f, 0xff } );
	machine->via2 = via2;
	TEST_ROM( A0000 );
	via2->verify( VIA_REG::RA, { 0xff, uint8_t(0x3f | (simm1.id << 6))  } );
	BOOST_CHECK_EQUAL( read_l( sz-28), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( sz-24), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( sz-20), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( success_bankB, simm30, simm2 ) {
	prepare<GLU>(&old_machines[0], &motherboards[0]);
	uint32_t sz = simm2.sz + 1_mb;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm2]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, uint32_t(simm2.bit << 4 | 0x1), true };
					});
	auto via2 = std::make_shared<IO_TEST<VIA2>>(0);
	via2->set_read_data( VIA_REG::RA, { 0x3f, 0xff } );
	machine->via2 = via2;
	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( read_l( sz-20), 1_mb ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( sz-16), simm2.sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}
BOOST_AUTO_TEST_CASE( fail ) {
	prepare<GLU>(&old_machines[0], &motherboards[0]);
	uint32_t sz = 0x100000 << 6;
	GET_MOCK(check_simm_slots)
		.always_do( [sz]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { 0, 0 , false };
					});
	auto via2 = std::make_shared<IO_TEST<VIA2>>(0);
	via2->set_read_data( VIA_REG::RA, { 0x3f, 0xff } );
	machine->via2 = via2;
	TEST_ROM( A0000 );
	via2->verify( VIA_REG::RA, { 0xff } );
	BOOST_CHECK( ! cpu.Z );
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( mdu )

BOOST_DATA_TEST_CASE( success_bankA, simm30, simm1 ) {
	prepare<MDU>( &old_machines[3], &motherboards[1]);
	uint32_t sz = simm1.sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm1]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, simm1.bit, true };
					});

	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( read_l( sz-20), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( sz-16), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( success_bankB, simm30, simm2 ) {
	prepare<MDU>( &old_machines[3], &motherboards[1]);
	uint32_t sz = simm2.sz ;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm2]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz+ 1_mb, uint32_t(simm2.bit << 4 | 0x1), true };
					});

	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( read_l( sz+0x04000000-20), 0x04000000 ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( sz+0x04000000-16), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( sz+0x04000000-12), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail ) {
	prepare<MDU>( &old_machines[3], &motherboards[1]);
	uint32_t sz = 0x100000 << 6;
	GET_MOCK(check_simm_slots)
		.always_do( [sz]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { 0, 0 , false };
					});
	TEST_ROM( A0000 );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( oss )
static const uint8_t OSS_RAMSIZE[] = { 0, 0b10, 0b11 }; // $A039E
BOOST_DATA_TEST_CASE( success_1_bankA, simm64, simm ) {
	prepare<OSS>( &old_machines[5], &motherboards[2]);
	uint32_t sz = simm.sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, simm.bit, true };
					});
	auto oss = std::dynamic_pointer_cast<IIFX_exp0>(machine->exp0 );
	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( oss->UT_IIFX_exp0_GET( 0 ), 0xF3EF );
	BOOST_CHECK_EQUAL( oss->UT_IIFX_exp0_GET( 0xff ), 0xF33F | (OSS_RAMSIZE[simm.id]<<6) );
	BOOST_CHECK_EQUAL( read_l(sz-28), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l(sz-24), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l(sz-20), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( success_1_bankB, simm64, simm ) {
	prepare<OSS>( &old_machines[5], &motherboards[2]);
	uint32_t sz = simm.sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, uint8_t(simm.bit << 4), true };
					});
	auto oss = std::dynamic_pointer_cast<IIFX_exp0>(machine->exp0 );
	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( oss->UT_IIFX_exp0_GET( 0 ), 0xF3EF );
	BOOST_CHECK_EQUAL( oss->UT_IIFX_exp0_GET( 0xff ), 0xF3FF );
	BOOST_CHECK_EQUAL( read_l(0x04000000+ sz-28), 0x04000000 ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l(0x04000000+ sz-24), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l(0x04000000+ sz-20), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( success_2, simm64, simm ) {
	prepare<OSS>( &old_machines[5], &motherboards[2]);
	uint32_t sz = simm.sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz, simm]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, simm.bit, true };
					});
	get_hw_info_mock =
		[]() {
			model = &old_machines[5];
			motherboard = &motherboards[2];
			INIT_HW_FLG = motherboards[2].init_flg;
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IIFX_EXP1 );
			GEN = model->generation;
		};
	auto oss = std::dynamic_pointer_cast<IIFX_exp0>(machine->exp0 );
	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( oss->UT_IIFX_exp0_GET( 0xff ), 0xF32F | (OSS_RAMSIZE[simm.id]<<6) );
	BOOST_CHECK( cpu.Z );
}


BOOST_AUTO_TEST_CASE( fail ) {
	prepare<OSS>( &old_machines[5], &motherboards[2]);
	GET_MOCK(check_simm_slots)
		.always_do( []( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { 0, 0 , false };
					});
	auto oss = std::dynamic_pointer_cast<IIFX_exp0>(machine->exp0 );
	TEST_ROM( A0000 );
	BOOST_CHECK_EQUAL( oss->UT_IIFX_exp0_GET( 0 ), 0xF3EF );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( v8 )
// V8 RBV::EXP HIGH 3BIT? 
static uint8_t V8_RAMSIZE[] = { 0x40, 0x80, 0xc0 };
BOOST_AUTO_TEST_CASE( embbed_1 ) {
	prepare<V8>( &old_machines[7], &motherboards[3]);
	uint32_t sz = 1_mb;
	RAMSize = sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz]( const RAM_SLOT_T* t) -> ram_result {
 						BOOST_CHECK_EQUAL( t->ranges[0].second, ram_tables[1].ranges[0].second );
						get_hw_info_mock();
						return { sz, 0x2, true };
					});
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->set_read_data( RBV_REG::EXP, { 0xff, 0x3f, 0xff, 0, 0 } );
	machine->rbv = rbv;
	TEST_ROM( A0000 );
	rbv->verify( RBV_REG::EXP, { 0x1f, 0xff, 0x3f, 0xe0 } );
	BOOST_CHECK_EQUAL( read_l( sz-28), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( sz-24), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( sz-20), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}
#if 0
BOOST_DATA_TEST_CASE( success_1, bdata::xrange(1, 5), n ) {
	prepare(MB_TYPE::V8, &old_machines[7], &motherboards[3]);
	uint32_t sz = 0x100000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0x800000u }, 0, 0x00800000, CHECK_DATA, 0x100000 );
	GET_MOCK(check_simm_slots)
		.always_do( [sz, n]( const RAM_SLOT_T* t) -> ram_result {
 						BOOST_CHECK_EQUAL( t->ranges[0].second, ram_tables[1].ranges[0].second );
						get_hw_info_mock();
						return { 2 * sz, uint32_t(n) << 4 | n, true };
					});
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->set_read_data( RBV_REG::EXP, { 0xff, 0x3f, 0xff, 0, 0 } );
	machine->rbv = rbv;
	TEST_ROM( A0000 );
	rbv->verify( RBV_REG::EXP, { 0x1f, 0xff, 0x3f, V8_RAMSIZE[n] } );
	BOOST_CHECK_EQUAL( read_l( 2*sz-28), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( 2*sz-24), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( 2*sz-20), sz ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( 2*sz-16), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( 2*sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK_EQUAL( read_l( 2*sz-8), 0 ); // padding
	BOOST_CHECK_EQUAL( read_l( 2*sz-4), 0 ); // padding
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( success_2 ) {
	prepare(MB_TYPE::V8, &old_machines[7], &motherboards[3]);
	constexpr uint32_t sz = 0x100000;
	RAMSize = sz;
	GET_MOCK(check_simm_slots)
		.always_do( [sz]( const RAM_SLOT_T*) -> ram_result {
						get_hw_info_mock();
						return { sz, 1, true };
					});
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->set_read_data( RBV_REG::EXP, { 0xff, 0, 0xff, 0, 0 } );
	machine->rbv = rbv;
	TEST_ROM( A0000 );
	rbv->verify( RBV_REG::EXP, { 0x1f, 0xE0, 0x3f, 0xff } );
	BOOST_CHECK_EQUAL( read_l( sz-28), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( sz-24), 0x100000 ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( sz-20), 0xffffffff ); // sentinel
	BOOST_CHECK_EQUAL( read_l( sz-16), 0 ); // padding
	BOOST_CHECK_EQUAL( read_l( sz-12), 0 ); // padding
	BOOST_CHECK_EQUAL( read_l( sz-8), 0 ); // padding
	BOOST_CHECK_EQUAL( read_l( sz-4), 0 ); // padding
	BOOST_CHECK( cpu.Z );
}

BOOST_DATA_TEST_CASE( success_3, bdata::xrange(1, 5), n ) {
	prepare(MB_TYPE::V8, &old_machines[7], &motherboards[3]);
	uint32_t sz = 0x100000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0x400000u }, 0, 0x00800000, CHECK_DATA, 0x100000 );
	GET_MOCK(check_simm_slots)
		.always_do( [sz, n]( const RAM_SLOT_T* t) -> ram_result {
 						BOOST_CHECK_EQUAL( t->ranges[0].second, ram_tables[2].ranges[0].second );
						get_hw_info_mock();
						return { 2 * sz, uint32_t(n) << 4 | n, true };
					});

	
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->set_read_data( RBV_REG::EXP, { 0xff, 0x3f, 0, 0xff, 0xff, 0, 0 } );
	machine->rbv = rbv;
	TEST_ROM( A0000 );
	rbv->verify( RBV_REG::EXP, { 0x1f, 0xff, 0x80, 0x9f, 0x3f, V8_RAMSIZE[n] } );
	BOOST_CHECK_EQUAL( read_l( 2*sz-28), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( 2*sz-24), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( 2*sz-20), sz ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( 2*sz-16), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( 2*sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK_EQUAL( read_l( 2*sz-8), 0 ); // padding
	BOOST_CHECK_EQUAL( read_l( 2*sz-4), 0 ); // padding
	BOOST_CHECK( cpu.Z );
}


BOOST_AUTO_TEST_CASE( fail ) {
	prepare(MB_TYPE::V8, &old_machines[7], &motherboards[3]);
	uint32_t sz = 0x100000 << 6;
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0x800000u }, 0, 0x00800000, CHECK_DATA, 0x100000 )
		.nth_is(2, { sz, 0u }, 0x00800000, 0x00A00000, CHECK_DATA, 0x100000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0100"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x0800000, CHECK_DATA );
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	rbv->set_read_data( RBV_REG::EXP, { 0xff, 0x3f }  );
	machine->rbv = rbv;
	TEST_ROM( A0000 );
	rbv->verify( RBV_REG::EXP, { 0x1f, 0xff } );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( mcu )
BOOST_DATA_TEST_CASE( success, bdata::xrange(1, 5), n ) {
	prepare(MB_TYPE::MCU, &old_machines[10], &motherboards[4]);
	uint32_t sz = 0x400000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0u }, 0, 0x04000000, CHECK_DATA, 0x400000 )
		.nth_is(2, { sz, 0u }, 0x04000000, 0x08000000, CHECK_DATA, 0x400000 )
		.nth_is(3, { sz, 0u }, 0x08000000, 0x0C000000, CHECK_DATA, 0x400000 )
		.nth_is(4, { sz, 0u }, 0x0C000000, 0x10000000, CHECK_DATA, 0x400000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x04000000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x08000000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x0C000000, CHECK_DATA );
	auto mcu = std::dynamic_pointer_cast<MCU_REG>(machine->mcu );
	TEST_ROM( A0000 );
	for(int i = 0; i < 5; ++i ) {
		BOOST_TEST_CHECKPOINT( "i=" << i );
		BOOST_CHECK_EQUAL( mcu->UT_GET_REG( i ), (1<<(n-1))>>i );
		BOOST_CHECK_EQUAL( mcu->UT_GET_REG( i+6 ), 2*(1<<(n-1))>>i );
		BOOST_CHECK_EQUAL( mcu->UT_GET_REG( i+6*2 ), 3*(1<<(n-1))>>i );
	}
	BOOST_CHECK_EQUAL( read_l( 4*sz-44), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( 4*sz-40), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( 4*sz-36), sz ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( 4*sz-32), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( 4*sz-28), 2*sz ); // BASE[2]
	BOOST_CHECK_EQUAL( read_l( 4*sz-24), sz ); // SIZE[2]
	BOOST_CHECK_EQUAL( read_l( 4*sz-20), 3*sz ); // BASE[3]
	BOOST_CHECK_EQUAL( read_l( 4*sz-16), sz ); // SIZE[3]
	BOOST_CHECK_EQUAL( read_l( 4*sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK_EQUAL( read_l( 4*sz-8), 0 ); // padding
	BOOST_CHECK_EQUAL( read_l( 4*sz-4), 0 ); // padding
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail ) {
	prepare(MB_TYPE::MCU, &old_machines[10], &motherboards[4]);
	uint32_t sz = 0x400000 ;
	GET_MOCK(check_simm_size)
//		.nth_is(1, { sz, 0u }, 0, 0x04000000, CHECK_DATA, 0x400000 )
		.nth_is(1, { sz, 0u }, 0x04000000, 0x08000000, CHECK_DATA, 0x400000 )
		.nth_is(2, { sz, 0u }, 0x08000000, 0x0C000000, CHECK_DATA, 0x400000 )
		.nth_is(3, { sz, 0u }, 0x0C000000, 0x10000000, CHECK_DATA, 0x400000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0100"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x04000000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x08000000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x0C000000, CHECK_DATA );
	auto mcu = std::dynamic_pointer_cast<MCU_REG>(machine->mcu );
	TEST_ROM( A0000 );
	for(int i = 0; i < 18; ++i ) {
		BOOST_TEST_INFO( "i=" << i );
		BOOST_CHECK_EQUAL( mcu->UT_GET_REG( i ), (0x30810 >> i) );
	}
	BOOST_CHECK( ! cpu.Z );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( jaws )
BOOST_DATA_TEST_CASE( success_1, bdata::xrange(1, 4), n ) {
	prepare(MB_TYPE::JAWS, &old_machines[11], &motherboards[5]);
	auto pb = std::make_shared<IO_TEST<JAWS_REG>>();
	machine->pb = pb;
	uint32_t sz = 0x80000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0u }, 0, 0x00200000, CHECK_DATA, 0x80000 )
		.nth_is(2, { sz, 0u }, 0x00200000, 0x00400000, CHECK_DATA, 0x80000 )
		.nth_is(3, { sz, 0u }, 0x00400000, 0x00600000, CHECK_DATA, 0x80000 )
		.nth_is(4, { sz, 0u }, 0x00600000, 0x00800000, CHECK_DATA, 0x80000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x00200000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x00400000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x00600000, CHECK_DATA );
	TEST_ROM( A0000 );
	pb->verify( 0x10000, { 0, uint8_t(n == 1 ? 1 : 0) } );
	pb->verify( 0x12000, { 0, 0 } );
	pb->verify( 0x14000, { 0, 0 } );
	BOOST_CHECK_EQUAL( read_l( 4*sz-76), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( 4*sz-72), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( 4*sz-68), sz ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( 4*sz-64), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( 4*sz-60), 2*sz ); // BASE[2]
	BOOST_CHECK_EQUAL( read_l( 4*sz-56), sz ); // SIZE[2]
	BOOST_CHECK_EQUAL( read_l( 4*sz-52), 3*sz ); // BASE[3]
	BOOST_CHECK_EQUAL( read_l( 4*sz-48), sz ); // SIZE[3]
	BOOST_CHECK_EQUAL( read_l( 4*sz-44), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( success_2 ) {
	prepare(MB_TYPE::JAWS, &old_machines[11], &motherboards[5]);
	auto pb = std::make_shared<IO_TEST<JAWS_REG>>();
	machine->pb = pb;
	uint32_t sz = 0x200000;
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0u }, 0, 0x00200000, CHECK_DATA, 0x80000 )
		.nth_is(2, { 0u, 0u }, 0x00200000, 0x00400000, CHECK_DATA, 0x80000 )
		.nth_is(3, { 0u, 0u }, 0x00400000, 0x00600000, CHECK_DATA, 0x80000 )
		.nth_is(4, { 0u, 0u }, 0x00600000, 0x00800000, CHECK_DATA, 0x80000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x00200000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x00400000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x00600000, CHECK_DATA );
	TEST_ROM( A0000 );
	pb->verify( 0x10000, { 0, 0 } );
	pb->verify( 0x12000, { 0, 0 } );
	pb->verify( 0x14000, { 0, 0 } );
	BOOST_CHECK_EQUAL( read_l( sz-76), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( sz-72), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( sz-68), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail ) {
	prepare(MB_TYPE::JAWS, &old_machines[11], &motherboards[5]);
	auto pb = std::make_shared<IO_TEST<JAWS_REG>>();
	machine->pb = pb;
	uint32_t sz = 0x200000;
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0u }, 0, 0x00200000, CHECK_DATA, 0x80000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("1000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0100"), 0x00200000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0010"), 0x00400000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0001"), 0x00600000, CHECK_DATA );
	TEST_ROM( A0000 );
	pb->verify( 0x10000, { 0 } );
	pb->verify( 0x12000, { 0 } );
	pb->verify( 0x14000, { 0 } );
	BOOST_CHECK( ! cpu.Z );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( msc )
BOOST_DATA_TEST_CASE( success, bdata::xrange(1, 4), n ) {
	prepare(MB_TYPE::MSC, &new_machines[0], &motherboards[6]);
	uint32_t sz1 = 0x200000;
	uint32_t sz2 = 0x200000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz1, 0u }, 0, 0x00200000, CHECK_DATA, sz1 )
		.nth_is(2, { sz1, 0u }, 0x00200000, 0x00400000, CHECK_DATA, sz1 )
		.nth_is(3, { sz1, 0u }, 0x00400000, 0x00600000, CHECK_DATA, sz1 )
		.nth_is(4, { sz1, 0u }, 0x00600000, 0x00800000, CHECK_DATA, sz1 )
		.nth_is(5, { sz2, 0u }, 0x00800000, 0x01000000, CHECK_DATA, sz1 )
		.nth_is(6, { sz2, 0u }, 0x01000000, 0x01800000, CHECK_DATA, sz1 )
		.nth_is(7, { sz2, 0u }, 0x01800000, 0x02000000, CHECK_DATA, sz1 )
		.nth_is(8, { sz2, 0u }, 0x02000000, 0x02800000, CHECK_DATA, sz1 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x00200000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x00400000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x00600000, CHECK_DATA )
		.nth_is(5, std::bitset<4>("0000"), 0x00800000, CHECK_DATA )
		.nth_is(6, std::bitset<4>("0000"), 0x01000000, CHECK_DATA )
		.nth_is(7, std::bitset<4>("0000"), 0x01800000, CHECK_DATA )
		.nth_is(8, std::bitset<4>("0000"), 0x02000000, CHECK_DATA );
	auto rbv = std::make_shared<IO_TEST<MSCRbv>>(dynamic_cast<MSC*>(machine.get()));
	machine->rbv = rbv;
	rbv->set_read_data( RBV_REG::MSC_RBV_X16, { 0x0f, 0xff } );
	uint32_t sz = sz1+sz2;
	TEST_ROM( A0000 );
	if( n == 3 ) {
		rbv->verify( RBV_REG::MSC_RBV_X16, { 0xff, 0xff } );
	} else {
		rbv->verify( RBV_REG::MSC_RBV_X16, { 0xff, 0xef } );
	}
	for(int i = 0; i < 4; ++i ) {
		BOOST_CHECK_EQUAL( read_l( 4*sz-76+8*i), sz1*i ); // BASE[0..3]
		BOOST_CHECK_EQUAL( read_l( 4*sz-72+8*i), sz1 ); // SIZE[0..3]
		BOOST_CHECK_EQUAL( read_l( 4*sz-44+8*i), sz1*4+sz2*i ); // BASE[4..7]
		BOOST_CHECK_EQUAL( read_l( 4*sz-40+8*i), sz2 ); // SIZE[4..7]
	}
	BOOST_CHECK_EQUAL( read_l( 4*sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK_EQUAL( read_l( 0x7FFC ), 4*sz - 76 - 0x2000 );
	for(int i = 0; i < 76; ++i ) {
		BOOST_CHECK_EQUAL( read_b( 4*sz - 0x2000-i ), read_b( 4*sz-i) );
	}
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail ) {
	prepare(MB_TYPE::MSC, &new_machines[0], &motherboards[6]);
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("1000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("1000"), 0x00200000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("1000"), 0x00400000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("1000"), 0x00600000, CHECK_DATA )
		.nth_is(5, std::bitset<4>("1000"), 0x00800000, CHECK_DATA )
		.nth_is(6, std::bitset<4>("1000"), 0x01000000, CHECK_DATA )
		.nth_is(7, std::bitset<4>("1000"), 0x01800000, CHECK_DATA )
		.nth_is(8, std::bitset<4>("1000"), 0x02000000, CHECK_DATA );
	auto rbv = std::make_shared<IO_TEST<MSCRbv>>(dynamic_cast<MSC*>(machine.get()));
	machine->rbv = rbv;
	rbv->set_read_data( RBV_REG::MSC_RBV_X16, { 0x0f } );
	TEST_ROM( A0000 );
	rbv->verify( RBV_REG::MSC_RBV_X16, { 0xff } );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( sonora )
BOOST_DATA_TEST_CASE( success_1, bdata::xrange(1, 4), n ) {
	prepare(MB_TYPE::SONORA, &new_machines[5], &motherboards[7]);
	uint32_t sz = 0x100000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0u }, 0, 0x01000000, CHECK_DATA, 0x100000 )
		.nth_is(2, { sz, 0u }, 0x01000000, 0x02000000, CHECK_DATA, 0x100000 )
		.nth_is(3, { sz, 0u }, 0x02000000, 0x03000000, CHECK_DATA, 0x100000 )
		.nth_is(4, { sz, 0u }, 0x03000000, 0x04000000, CHECK_DATA, 0x100000 )
		.nth_is(5, { sz, 0u }, 0x04000000, 0x05000000, CHECK_DATA, 0x100000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x01000000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x02000000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x03000000, CHECK_DATA )
		.nth_is(5, std::bitset<4>("0000"), 0x04000000, CHECK_DATA );
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	machine->rbv = rbv;
	rbv->set_read_data( RBV_REG::EXP, { 0x80, 0x80 } );
	TEST_ROM( A0000 );
	if( n == 4 ) {
		rbv->verify( RBV_REG::EXP, { 0xff, 0xff } );
	} else {
		rbv->verify( RBV_REG::EXP, { 0xff, uint8_t(0xc0 | (n+1) << 3 | (n+1)) } );
	}
	BOOST_CHECK_EQUAL( read_l( 5*sz-52), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( 5*sz-48), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( 5*sz-44), sz ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( 5*sz-40), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( 5*sz-36), 2*sz ); // BASE[2]
	BOOST_CHECK_EQUAL( read_l( 5*sz-32), sz ); // SIZE[2]
	BOOST_CHECK_EQUAL( read_l( 5*sz-28), 3*sz ); // BASE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-24), sz ); // SIZE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-20), 4*sz ); // BASE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-16), sz ); // SIZE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-12), 0xffffffff ); // sentinel
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail ) {
	prepare(MB_TYPE::SONORA, &new_machines[5], &motherboards[7]);
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("1000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("1000"), 0x01000000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("1000"), 0x02000000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("1000"), 0x03000000, CHECK_DATA )
		.nth_is(5, std::bitset<4>("1000"), 0x04000000, CHECK_DATA );
	auto rbv = std::make_shared<IO_TEST<RBV>>();
	machine->rbv = rbv;
	rbv->set_read_data( RBV_REG::EXP, { 0x80 } );
	TEST_ROM( A0000 );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( memc_jr )
BOOST_DATA_TEST_CASE( success_1, bdata::xrange(1, 8), n ) {
	prepare(MB_TYPE::MEMCjr, &new_machines[10], &motherboards[9]);
	uint32_t sz = 0x100000 << (n-1);
	GET_MOCK(check_simm_size)
		.nth_is(1, { sz, 0u }, 0, 0x04000000, CHECK_DATA, 0x100000 )
		.nth_is(2, { sz, 0u }, 0x04000000, 0x08000000, CHECK_DATA, 0x100000 )
		.nth_is(3, { sz, 0u }, 0x08000000, 0x0C000000, CHECK_DATA, 0x100000 )
		.nth_is(4, { sz, 0u }, 0x0C000000, 0x10000000, CHECK_DATA, 0x100000 )
		.nth_is(5, { sz, 0u }, 0x10000000, 0x14000000, CHECK_DATA, 0x100000 );
	GET_MOCK(check_simm)
		.nth_is(1, std::bitset<4>("0000"), 0, CHECK_DATA )
		.nth_is(2, std::bitset<4>("0000"), 0x04000000, CHECK_DATA )
		.nth_is(3, std::bitset<4>("0000"), 0x08000000, CHECK_DATA )
		.nth_is(4, std::bitset<4>("0000"), 0x0C000000, CHECK_DATA )
		.nth_is(5, std::bitset<4>("0000"), 0x10000000, CHECK_DATA );
	auto reg1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	machine->reg1 = reg1;
	reg1->set_read_data( 4, { 0xff } );
	reg1->set_read_data( 5, { 0xff } );
	reg1->set_read_data( 6, { 0xff } );
	reg1->set_read_data( 7, { 0xff } );
	reg1->set_read_data( 126, { 0xff } );
	reg1->set_read_data( 127, { 0xff } );
	reg1->set_read_data( 6, { 0xff } );
	reg1->set_read_data( 7, { 0xff } );
	TEST_ROM( A0000 );
#if 0
	if( n == 4 ) {
		rbv->verify( RBV_REG::EXP, { 0xff, 0xff } );
	} else {
		rbv->verify( RBV_REG::EXP, { 0xff, uint8_t(0xc0 | (n+1) << 3 | (n+1)) } );
	}
	
	BOOST_CHECK_EQUAL( read_l( 5*sz-52), 0 ); // BASE[0]
	BOOST_CHECK_EQUAL( read_l( 5*sz-48), sz ); // SIZE[0]
	BOOST_CHECK_EQUAL( read_l( 5*sz-44), sz ); // BASE[1]
	BOOST_CHECK_EQUAL( read_l( 5*sz-40), sz ); // SIZE[1]
	BOOST_CHECK_EQUAL( read_l( 5*sz-36), 2*sz ); // BASE[2]
	BOOST_CHECK_EQUAL( read_l( 5*sz-32), sz ); // SIZE[2]
	BOOST_CHECK_EQUAL( read_l( 5*sz-28), 3*sz ); // BASE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-24), sz ); // SIZE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-20), 4*sz ); // BASE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-16), sz ); // SIZE[3]
	BOOST_CHECK_EQUAL( read_l( 5*sz-12), 0xffffffff ); // sentinel
#endif
	BOOST_CHECK( cpu.Z );
}
#endif
BOOST_AUTO_TEST_SUITE_END()
