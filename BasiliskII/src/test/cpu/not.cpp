#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"
extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(not_byte, fixture);
BOOST_AUTO_TEST_CASE(result) {
	cpu.R[0] = 0x23;
	cpu.V = true;
	cpu.C = true;
	test_cpu( { 0043000 } );
	BOOST_TEST( cpu.R[0] == 0xdc );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x10 : 0x80;
	test_cpu( { 0043000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0xff : 1;
	test_cpu( { 0043000 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_b(cpu, { 0043000 }, 0x23, ea, 0xdc );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(not_word, fixture);
BOOST_AUTO_TEST_CASE(result) {
	cpu.R[0] = 0x2323;
	cpu.V = true;
	cpu.C = true;
	test_cpu( { 0043100 } );
	BOOST_TEST( cpu.R[0] == 0xdcdc );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x2000 : 0x8000;
	test_cpu( { 0043100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0xffff : 1;
	test_cpu( { 0043100 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_w(cpu, { 0043100 }, 0x2323, ea, 0xdcdc );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(not_long, fixture);
BOOST_AUTO_TEST_CASE(result) {
	cpu.R[0] = 0x23232323;
	cpu.V = true;
	cpu.C = true;
	test_cpu( { 0043200 } );
	BOOST_TEST( cpu.R[0] == 0xdcdcdcdc );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x20000000 : 0x80000000;
	test_cpu( { 0043200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0xffffffff : 1;
	test_cpu( { 0043200 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_l(cpu, { 0042200 }, 222222, ea, -222222 );
}

BOOST_AUTO_TEST_SUITE_END();
