#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "mmu/mmu.hpp"
class MMU_Tester {
public:
	static MC68040_MMU& get_mmu() {
		return *static_cast<MC68040_MMU*>(mmu);
	}
	static uint16_t get_TCR() { return get_mmu().TCR; }
	static void set_TCR(uint16_t v) { get_mmu().TCR = v; }
	static uint32_t get_ITTR0() { return get_mmu().ITTR[0]; }
	static void set_ITTR0(uint32_t v) { get_mmu().ITTR[0] = v; }
	static uint32_t get_ITTR1() { return get_mmu().ITTR[1]; }
	static void set_ITTR1(uint32_t v) { get_mmu().ITTR[1] = v; }
	static uint32_t get_DTTR0() { return get_mmu().DTTR[0]; }
	static void set_DTTR0(uint32_t v) { get_mmu().DTTR[0] = v; }
	static uint32_t get_DTTR1() { return get_mmu().DTTR[1]; }
	static void set_DTTR1(uint32_t v) { get_mmu().DTTR[1] = v; }
	static uint32_t get_SR() { return get_mmu().MMU_SR; }
	static void set_SR(uint32_t v) { get_mmu().MMU_SR = v; }
	static uint32_t get_URP() { return get_mmu().URP; }
	static void set_URP(uint32_t v) { get_mmu().URP = v; }
	static uint32_t get_SRP() { return get_mmu().SRP; }
	static void set_SRP(uint32_t v) { get_mmu().SRP = v; }
	
};
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(movec_from, fixture);
BOOST_AUTO_TEST_CASE( user)  {
	cpu.S = false;
	test_cpu( { 0047172, 0 } );
	test_in_exception(8) ;
}

BOOST_AUTO_TEST_CASE( sfc)  {
	cpu.S = true;
	cpu.SFC = 1;
	test_cpu( { 0047172, (uint16_t)(2 << 12) } );
	BOOST_TEST( cpu.R[2] == 1 );
}

BOOST_AUTO_TEST_CASE( dfc)  {
	cpu.S = true;
	cpu.DFC = 3;
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 1)} );
	BOOST_TEST( cpu.R[2] == 3 );
}

BOOST_AUTO_TEST_CASE( usp)  {
	cpu.S = true;
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x800)} );
	BOOST_TEST( cpu.R[2] == cpu.USP );
}

BOOST_AUTO_TEST_CASE( vbr)  {
	cpu.S = true;
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x801)} );
	BOOST_TEST( cpu.R[2] == cpu.VBR );
}

BOOST_AUTO_TEST_CASE( cacr)  {
	cpu.S = true;
	cpu.CACR = 0x80008000;
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 2)} );
	BOOST_TEST( cpu.R[2] == 0x80008000 );
}

BOOST_AUTO_TEST_CASE( msp)  {
	cpu.S = true;
	cpu.MSP = 0x3000;
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x803)} );
	BOOST_TEST( cpu.R[2] == 0x3000 );
}

BOOST_AUTO_TEST_CASE( isp)  {
	cpu.S = true;
	cpu.ISP = 0x4000;
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x804)} );
	BOOST_TEST( cpu.R[2] == 0x4000 );
}


BOOST_AUTO_TEST_CASE( tcr)  {
	cpu.S = true;
	MMU_Tester::set_TCR( 1 << 14 );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 3)} );
	BOOST_TEST( cpu.R[2] == 1 << 14  );
	MMU_Tester::set_TCR( 0 );
}

BOOST_AUTO_TEST_CASE( itt0)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 3 << 24 | 1 << 15 | 1 << 8 | 1 << 2 ;
	MMU_Tester::set_ITTR0( i );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 4)} );
	BOOST_TEST( cpu.R[2] == i );
	MMU_Tester::set_ITTR0( 0 );
}

BOOST_AUTO_TEST_CASE( itt1)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 7 << 24 | 1 << 15 | 1 << 2 ;
	MMU_Tester::set_ITTR1( i );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 5)} );
	BOOST_TEST( cpu.R[2] == i );
	MMU_Tester::set_ITTR1( 0 );
}

BOOST_AUTO_TEST_CASE( dtt0)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 3 << 24 | 1 << 15 | 1 << 8 | 1 << 2 ;
	MMU_Tester::set_DTTR0( i );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 6)} );
	BOOST_TEST( cpu.R[2] == i );
	MMU_Tester::set_DTTR0( 0 );
}

BOOST_AUTO_TEST_CASE( dtt1)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 7 << 24 | 1 << 15 | 1 << 2 ;
	MMU_Tester::set_DTTR1( i );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 7)} );
	BOOST_TEST( cpu.R[2] == i );
	MMU_Tester::set_DTTR1( 0 );
}

BOOST_AUTO_TEST_CASE( MMUSR)  {
	cpu.S = true;
	uint32_t i = 0x223 << 12 | 1 << 10 | 1 << 7 | 1 << 2 | 1;
	MMU_Tester::set_SR( i );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x805)} );
	BOOST_TEST( cpu.R[2] == i );
	MMU_Tester::set_SR( 0 );
}

BOOST_AUTO_TEST_CASE( URP)  {
	cpu.S = true;
	MMU_Tester::set_URP( 0x20000 );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x806)} );
	BOOST_TEST( cpu.R[2] == 0x20000 );
	MMU_Tester::set_URP( 0 );
}

BOOST_AUTO_TEST_CASE( SRP)  {
	cpu.S = true;
	MMU_Tester::set_SRP( 0x30000 );
	test_cpu( { 0047172, (uint16_t)(2 << 12 | 0x807)} );
	BOOST_TEST( cpu.R[2] == 0x30000 );
	MMU_Tester::set_SRP( 0 );
}



BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(movec_to, fixture);
BOOST_AUTO_TEST_CASE( user)  {
	cpu.S = false;
	test_cpu( { 0047173, 0 } );
	test_in_exception(8) ;
}

BOOST_AUTO_TEST_CASE( sfc)  {
	cpu.S = true;
	cpu.R[2] = 1;
	test_cpu( { 0047173, (uint16_t)(2 << 12) } );
	BOOST_TEST( cpu.SFC == 1 );
}

BOOST_AUTO_TEST_CASE( dfc)  {
	cpu.S = true;
	cpu.R[2] = 3;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 1)} );
	BOOST_TEST( cpu.DFC == 3 );
}
BOOST_AUTO_TEST_CASE( usp)  {
	cpu.S = true;
	cpu.R[2] = 0x3000;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x800)} );
	BOOST_TEST( cpu.USP  == 0x3000 );
}

BOOST_AUTO_TEST_CASE( vbr)  {
	cpu.S = true;
	cpu.R[2] = cpu.VBR;
	cpu.VBR = 0;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x801)} );
	BOOST_TEST( cpu.VBR == cpu.R[2] );
}


BOOST_AUTO_TEST_CASE( cacr)  {
	cpu.S = true;
	cpu.R[2] = 0x80008000;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 2)} );
	BOOST_TEST( cpu.CACR == 0x80008000 );
}

BOOST_AUTO_TEST_CASE( msp)  {
	cpu.S = true;
	cpu.R[2] = 0x3000;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x803)} );
	BOOST_TEST( cpu.MSP == 0x3000 );
}


BOOST_AUTO_TEST_CASE( isp)  {
	cpu.S = true;
	cpu.R[2] = 0x4000;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x804)} );
	BOOST_TEST( cpu.ISP == 0x4000 );
}

BOOST_AUTO_TEST_CASE( tcr)  {
	cpu.S = true;
	cpu.R[2] = 1 << 14;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 3)} );
	BOOST_TEST( MMU_Tester::get_TCR() == 1 << 14  );
	MMU_Tester::set_TCR( 0 );
}

BOOST_AUTO_TEST_CASE( itt0)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 3 << 24 | 1 << 15 | 1 << 8 | 1 << 2 ;
	cpu.R[2] = i;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 4)} );;
	BOOST_TEST( 	MMU_Tester::get_ITTR0( ) == i );
	MMU_Tester::set_ITTR0( 0 );
}

BOOST_AUTO_TEST_CASE( itt1)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 7 << 24 | 1 << 15 | 1 << 2 ;
	cpu.R[2] = i;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 5)} );
	BOOST_TEST( 	MMU_Tester::get_ITTR1( ) == i );
	MMU_Tester::set_ITTR1( 0 );
}

BOOST_AUTO_TEST_CASE( dtt0)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 3 << 24 | 1 << 15 | 1 << 8 | 1 << 2 ;
	cpu.R[2] = i;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 6)} );
	BOOST_TEST( MMU_Tester::get_DTTR0() == i );
	MMU_Tester::set_DTTR0( 0 );
}

BOOST_AUTO_TEST_CASE( dtt1)  {
	cpu.S = true;
	uint32_t i = 3 << 24 | 7 << 24 | 1 << 15 | 1 << 2 ;
	cpu.R[2] = i;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 7)} );
	BOOST_TEST( MMU_Tester::get_DTTR1() == i );
	MMU_Tester::set_DTTR1( 0 );
}

BOOST_AUTO_TEST_CASE( MMUSR)  {
	cpu.S = true;
	uint32_t i = 0x223 << 12 | 1 << 10 | 1 << 7 | 1 << 2 | 1;
	cpu.R[2] = i;
	MMU_Tester::set_SR( i );
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x805)} );
	BOOST_TEST( MMU_Tester::get_SR() == i );
	MMU_Tester::set_SR( 0 );
}

BOOST_AUTO_TEST_CASE( URP)  {
	cpu.S = true;
	cpu.R[2] = 0x20000;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x806)} );
	BOOST_TEST( MMU_Tester::get_URP() == 0x20000 );
	MMU_Tester::set_URP( 0 );
}

BOOST_AUTO_TEST_CASE( SRP)  {
	cpu.S = true;
	cpu.R[2] = 0x30000;
	test_cpu( { 0047173, (uint16_t)(2 << 12 | 0x807)} );
	BOOST_TEST( MMU_Tester::get_SRP() == 0x30000 );
	MMU_Tester::set_SRP( 0 );
}


BOOST_AUTO_TEST_SUITE_END();
