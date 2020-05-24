#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(move_byte, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[1] = 0x24 | 0xffffff00;
	cpu.R[2] = 0x35 | 0xeeeeee00;
	cpu.V = cpu.C = true;
	test_cpu( { 0012001 } );
	BOOST_TEST( cpu.R[2] == ( 0x24 | 0xeeeeee00 ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = (n << 7) | 0x33;
	test_cpu( { 0012001 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = !z;
	test_cpu( { 0012001 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE( to_mem, ea_all, ea ) {
	if( ea == AR )
		return;
	cpu.R[8+5] = 0x1000;
	test_ea_b(cpu, { 0015200 }, 11, ea, 11 );
	BOOST_TEST( test_read8(0x1000) == 11 );
}

BOOST_DATA_TEST_CASE( to_inc, ea_all, ea ) {
	if( ea == AR )
		return;
	cpu.R[8+5] = 0x1000;
	test_ea_b(cpu, { 0015300 }, 11, ea, 11 );
	BOOST_TEST( test_read8(0x1000) == 11 );
	BOOST_TEST( cpu.R[8+5] == 0x1001 );
}

BOOST_DATA_TEST_CASE( push, ea_all, ea ) {
	if( ea == AR )
		return;
	cpu.R[8+7] = 0x1000;
	test_ea_b(cpu, { 0017300 }, 11, ea, 11 );
	BOOST_TEST( test_read8(0x1000) == 11 );
	BOOST_TEST( cpu.R[8+7] == 0x1002 );
}

BOOST_DATA_TEST_CASE( to_dec, ea_all, ea ) {
	if( ea == AR )
		return;
	cpu.R[8+5] = 0x1001;
	test_ea_b(cpu, { 0015400 }, 11, ea, 11 );
	BOOST_TEST( test_read8(0x1000) == 11 );
	BOOST_TEST( cpu.R[8+5] == 0x1000 );
}

BOOST_DATA_TEST_CASE( to_stack_dec, ea_all, ea ) {
	if( ea == AR )
		return;
	cpu.R[8+7] = 0x1002;
	test_ea_b(cpu, { 0017400 }, 11, ea, 11 );
	BOOST_TEST( test_read8(0x1000) == 11 );
	BOOST_TEST( cpu.R[8+7] == 0x1000 );
}

BOOST_AUTO_TEST_CASE( to_d16an ) {
	cpu.R[8+5] = 0x1000;
	cpu.R[6] = 11;
	test_cpu({ 0015506, 0x100 });
	BOOST_TEST( test_read8(0x1100) == 11 );
}

BOOST_AUTO_TEST_CASE( to_d8anxn ) {
	cpu.R[8+5] = 0x1000;
	cpu.R[0] = 0x10;
	cpu.R[6] = 11;
	test_cpu( { 0015606, 0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 11 );
}

BOOST_AUTO_TEST_CASE( to_xxx_w ) {
	cpu.R[7] = 11;
	test_cpu( { 0010707, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 11 );
}

BOOST_AUTO_TEST_CASE( to_xxx_l ) {
	cpu.R[7] = 11;
	test_cpu( { 0011707, 0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 11 );
}

BOOST_AUTO_TEST_CASE( pop ) {
	cpu.R[8+7] = 0x1002;
	test_write8(0x1000, 11);
	test_cpu({ 0010047, 0x100 });
	BOOST_TEST( cpu.R[0] == 11 );
	BOOST_TEST( cpu.R[8+7] == 0x1000 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_word, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[1] = 0x2424 | 0xffff0000;
	cpu.R[2] = 0x3535 | 0xeeee0000;
	cpu.V = cpu.C = true;
	test_cpu( { 0032001 } );
	BOOST_TEST( cpu.R[2] == ( 0x2424 | 0xeeee0000 ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = (n << 15) | 0x333;
	test_cpu( { 0032001 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = !z;
	test_cpu( { 0032001 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE( to_mem, ea_all, ea ) {
	cpu.R[8+5] = 0x1000;
	test_ea_w(cpu, { 0035200 }, 1111, ea, 1111 );
	BOOST_TEST( test_read16(0x1000) == 1111 );
}

BOOST_DATA_TEST_CASE( to_inc, ea_all, ea ) {
	cpu.R[8+5] = 0x1000;
	test_ea_w(cpu, { 0035300 }, 1111, ea, 1111 );
	BOOST_TEST( test_read16(0x1000) == 1111 );
	BOOST_TEST( cpu.R[8+5] == 0x1002 );
}

BOOST_DATA_TEST_CASE( to_dec, ea_all, ea ) {
	cpu.R[8+5] = 0x1002;
	test_ea_w(cpu, { 0035400 }, 1111, ea, 1111 );
	BOOST_TEST( test_read16(0x1000) == 1111 );
	BOOST_TEST( cpu.R[8+5] == 0x1000 );
}


BOOST_AUTO_TEST_CASE( to_d16an ) {
	cpu.R[8+5] = 0x1000;
	cpu.R[6] = 1111;
	test_cpu({ 0035506, 0x100 });
	BOOST_TEST( test_read16(0x1100) == 1111 );
}

BOOST_AUTO_TEST_CASE( to_d8anxn ) {
	cpu.R[8+5] = 0x1000;
	cpu.R[0] = 0x10;
	cpu.R[6] = 1111;
	test_cpu( { 0035606, 0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 1111 );
}

BOOST_AUTO_TEST_CASE( to_xxx_w ) {
	cpu.R[7] = 1111;
	test_cpu( { 0030707, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 1111 );
}

BOOST_AUTO_TEST_CASE( to_xxx_l ) {
	cpu.R[7] = 1111;
	test_cpu( { 0031707, 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 1111 );
}



BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_long, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[1] = 0x24242424;
	cpu.R[2] = 0x35353535;
	cpu.V = cpu.C = true;
	test_cpu( { 0022001 } );
	BOOST_TEST( cpu.R[2] == ( 0x24242424 ));
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = (n << 31) | 0x3333333;
	test_cpu( { 0022001 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[1] = !z;
	test_cpu( { 0022001 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE( to_mem, ea_all, ea ) {
	cpu.R[8+5] = 0x1000;
	test_ea_l(cpu, { 0025200 }, 111111, ea, 111111 );
	BOOST_TEST( test_read32(0x1000) == 111111 );
}

BOOST_DATA_TEST_CASE( to_inc, ea_all, ea ) {
	cpu.R[8+5] = 0x1000;
	test_ea_l(cpu, { 0025300 }, 111111, ea, 111111 );
	BOOST_TEST( test_read32(0x1000) == 111111 );
	BOOST_TEST( cpu.R[8+5] == 0x1004 );
}

BOOST_DATA_TEST_CASE( to_dec, ea_all, ea ) {
	cpu.R[8+5] = 0x1004;
	test_ea_l(cpu, { 0025400 }, 111111, ea, 111111 );
	BOOST_TEST( test_read32(0x1000) == 111111 );
	BOOST_TEST( cpu.R[8+5] == 0x1000 );
}


BOOST_AUTO_TEST_CASE( to_d16an ) {
	cpu.R[8+5] = 0x1000;
	cpu.R[6] = 111111;
	test_cpu({ 0025506, 0x100 });
	BOOST_TEST( test_read32(0x1100) == 111111 );
}

BOOST_AUTO_TEST_CASE( to_d8anxn ) {
	cpu.R[8+5] = 0x1000;
	cpu.R[0] = 0x10;
	cpu.R[6] = 111111;
	test_cpu( { 0025606, 0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 111111 );
}

BOOST_AUTO_TEST_CASE( to_xxx_w ) {
	cpu.R[7] = 111111;
	test_cpu( { 0020707, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 111111 );
}

BOOST_AUTO_TEST_CASE( to_xxx_l ) {
	cpu.R[7] = 111111;
	test_cpu( { 0021707, 0, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 111111 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_from_ccr, fixture);
BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(2) * bdata::xrange(2) * bdata::xrange(2) *
					 bdata::xrange(2) * bdata::xrange(2),
					 x,n,z,v,c
	)
{
	cpu.X = x;
	cpu.N = n;
	cpu.Z = z;
	cpu.V = v;
	cpu.C = c;
	test_cpu( { 0041302 } );
	uint16_t f = x << 4 | n << 3 | z << 2 | v << 1 | c;
	BOOST_TEST( cpu.R[2] == f );
}

BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea) {
	cpu.C = true;
	test_ea_w(cpu, { 0041300 }, 0, ea, 1 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_to_ccr, fixture);
BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(2) * bdata::xrange(2) * bdata::xrange(2) *
					 bdata::xrange(2) * bdata::xrange(2),
					 x,n,z,v,c
	)
{
	uint16_t f = x << 4 | n << 3 | z << 2 | v << 1 | c;
	cpu.R[2] = f;
	test_cpu( { 0042302 } );
	BOOST_TEST( cpu.X == x );
	BOOST_TEST( cpu.N == n );
	BOOST_TEST( cpu.Z == z );
	BOOST_TEST( cpu.V == v );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_data, ea ) {
	test_ea_w(cpu, { 0042300 }, 1, ea, 1 );
	BOOST_TEST( cpu.X == false );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == true );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_from_sr, fixture);
BOOST_AUTO_TEST_CASE(user) {
	cpu.S = false;
	cpu.R[2] = 0x1111;
	test_cpu( { 0040302 } );
	test_in_exception(8) ;
	BOOST_TEST( cpu.R[2] == 0x1111 );
}

BOOST_AUTO_TEST_CASE(system) 
{
	cpu.T = 0;
	cpu.S = true;
	cpu.M = true;
	cpu.intmask = 3;
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
	test_cpu( { 0040302 } );
	BOOST_TEST( cpu.R[2] == 0x3300 );
}

BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea) {
	cpu.S = true;
	cpu.M = true;
	cpu.intmask = 3;
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
	test_ea_w(cpu, { 0040300 }, 0, ea, 0x3300 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_to_sr, fixture);
BOOST_AUTO_TEST_CASE(user) {
	cpu.S = false;
	cpu.intmask = 0;
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
	cpu.R[2] = 0x071f;
	test_cpu( { 0043302 } );
	test_in_exception(8) ;
	BOOST_TEST( cpu.intmask == 0 );
	BOOST_TEST( cpu.X == false );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_AUTO_TEST_CASE(system) 
{
	cpu.S = true;
	cpu.intmask = 0;
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
	cpu.R[2] = 0x071f;
	test_cpu( { 0043302 } );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.intmask == 7 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.N == true );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.V == true );
	BOOST_TEST( cpu.C == true );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_to_usp, fixture);
BOOST_AUTO_TEST_CASE(user) {
	cpu.S = false;
	cpu.USP = 0x12345678;
	cpu.R[8+2] = 0xdeadbeaf;
	test_cpu( { 0047142 } );
	test_in_exception(8) ;
	BOOST_TEST( cpu.USP == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(system) 
{
	cpu.S = true;
	cpu.R[8+2] = 0xa000;
	cpu.USP = 0x12345678;
	test_cpu( { 0047142 } );
	BOOST_TEST( cpu.USP == 0xa000 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(move_from_usp, fixture);
BOOST_AUTO_TEST_CASE(user) {
	cpu.S = false;
	cpu.USP = 0x12345678;
	cpu.R[8+2] = 0xdeadbeaf;
	test_cpu( { 0047152 } );
	test_in_exception(8) ;
	BOOST_TEST( cpu.R[8+2] == 0xdeadbeaf );
}

BOOST_AUTO_TEST_CASE(system) 
{
	cpu.S = true;
	cpu.USP = 0xa000;;
	cpu.R[8+2] = 0x12345678;
	test_cpu( { 0047152 } );
	BOOST_TEST( cpu.R[8+2] == 0xa000 );
}

BOOST_AUTO_TEST_SUITE_END();
