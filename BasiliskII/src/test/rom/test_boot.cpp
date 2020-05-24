 
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "registers.hpp"
#include "exceptions.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"
#include "prototype.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "machine.hpp"
namespace bdata = boost::unit_test::data;
extern uint8_t* ROMBaseHost;
extern uint32_t ROMSize;
using namespace ROM;
BOOST_FIXTURE_TEST_SUITE(rom_0309C, fixture);
BOOST_AUTO_TEST_CASE( has )  {
	set_sr( 0x2700 );
	model = 0x40803910;
	INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
	if( rom ) {
		test_rom( 0x4080309C );
	} else {
		update_rom_flag();
	}
	BOOST_TEST( DR(1) == 0x00043924 );
	BOOST_TEST( ! INIT_FLAGS.test( INIT_FLAG_T::FAULT_SKIP ) );
}

BOOST_AUTO_TEST_CASE( no )  {
	set_sr( 0x2700 );
	model = 0x40803AC0;
	motherboard = 0x408034C0;
	if( rom ) {
		test_rom( 0x4080309C );
	} else {
		update_rom_flag();
	}
	BOOST_TEST( DR(1) == 0x000000A6 );
}
BOOST_AUTO_TEST_SUITE_END();
// 4B0C
BOOST_FIXTURE_TEST_SUITE(rom_04B0C, fixture);
BOOST_AUTO_TEST_CASE( none )  {
	set_sr( 0x2700 );
	HWCFG_FLAGS.reset( HWCFG_T::HAS_FPU );
	INIT_HW_FLG.reset( INIT_HW_FLG_T::ETHER );
	INIT_HW_FLG.reset( INIT_HW_FLG_T(29) );
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x309C ] == 1 );
	}
}
BOOST_AUTO_TEST_CASE( no_fpu )  {
	set_sr( 0x2700 );
	HWCFG_FLAGS.set( HWCFG_T::HAS_FPU );
	rom_routines[ 0x4640] =	[]() { cpu.Z = false; } ;
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x309C ] == 1 );
	}
	BOOST_TEST( ! HWCFG_FLAGS.test( HWCFG_T::HAS_FPU) );
}

BOOST_AUTO_TEST_CASE( fpu )  {
	set_sr( 0x2700 );
	HWCFG_FLAGS.set( HWCFG_T::HAS_FPU );
	rom_routines[ 0x4640] =	[]() { cpu.Z = true; } ;
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x309C ] == 1 );
	}
	BOOST_TEST( HWCFG_FLAGS.test( HWCFG_T::HAS_FPU) );
}

BOOST_AUTO_TEST_CASE( no_ether)  {
	set_sr( 0x2700 );
	INIT_HW_FLG.set( INIT_HW_FLG_T::ETHER );
	rom_routines[ 0x47B0] =	[]() { cpu.Z = false; } ;
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x47B0 ] == 1 );
	}
	BOOST_TEST( ! INIT_HW_FLG.test( INIT_HW_FLG_T::ETHER ));
}

BOOST_AUTO_TEST_CASE( ether)  {
	set_sr( 0x2700 );
	INIT_HW_FLG.set( INIT_HW_FLG_T::ETHER );
	rom_routines[ 0x47B0] =	[]() { cpu.Z = true; } ;
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x47B0 ] == 1 );
	}
	BOOST_TEST( INIT_HW_FLG.test( INIT_HW_FLG_T::ETHER ));
}

BOOST_AUTO_TEST_CASE( no_lowmem_global)  {
	set_sr( 0x2700 );
	INIT_HW_FLG.set( INIT_HW_FLG_T::SPECIAL_LOWMEM );
	AR(0) = 0x40803564;
	rom_routines[ 0x47BA] =	[]() { cpu.Z = false; } ;
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x47BA ] == 1 );
	}
	BOOST_TEST( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SPECIAL_LOWMEM ));
}

BOOST_AUTO_TEST_CASE( lowmem_global)  {
	set_sr( 0x2700 );
	INIT_HW_FLG.set( INIT_HW_FLG_T(29) );
	AR(0) = 0x40803564;
	rom_routines[ 0x47BA] =	[]() { cpu.Z = true; } ;
	rom_routines[ 0x309C] =	[]() { } ;
	if( rom ) {
		test_rom( 0x40804B0C, 0x40804B66 );
	} else {
		check_flag();
		BOOST_TEST( called[ 0x47BA ] == 1 );
	}
	BOOST_TEST( INIT_HW_FLG.test( INIT_HW_FLG_T(29) ));
}

BOOST_AUTO_TEST_SUITE_END();
