#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <atomic>
#include "rbv.hpp"
#include "via_test.hpp"


namespace bdata = boost::unit_test::data;
static RBV v;

BOOST_AUTO_TEST_SUITE(rbv);

BOOST_DATA_TEST_CASE( b_read, bdata::xrange(8), x )  {
	RBV_TEST v;
	v.reset();
	for(int i = 0; i < 8; ++i ) {
		v.b[i].set_read_data( { i == x } );
	}
	BOOST_TEST( v.read(VIA_REG::RB) == 1 << x );
}

BOOST_DATA_TEST_CASE( b_write, bdata::xrange(8), x )  {
	RBV_TEST v;
	v.reset();
	v.write( VIA_REG::RB, 1 << x);
	for(int i = 0; i < 8; ++i ) {
		v.b[i].verify( { i == x } );
	}
}


BOOST_DATA_TEST_CASE( a_read, bdata::xrange(8), x )  {
	RBV_TEST v;
	v.reset();
	for(int i = 0; i < 8; ++i ) {
		v.a[i].set_read_data( { i == x } );
	}
	BOOST_TEST( v.read(RBV_REG::RA) == 1 << x );
}

BOOST_DATA_TEST_CASE( a_write, bdata::xrange(8), x )  {
	RBV_TEST v;
	v.reset();
	v.write( RBV_REG::RA, 1 << x);
	for(int i = 0; i < 8; ++i ) {
		v.a[i].verify( { i == x } );
	}
}

// taken from ROM$46D8
BOOST_AUTO_TEST_CASE( works )  {
	v.reset();	
	uint8_t x = v.read(RBV_REG::IER);
	uint8_t l = 0xff;
	v.write(RBV_REG::IER, l);
	do {
		l = -l;
		v.write(RBV_REG::IER, l);
		l *= 2;
		if( l == 0 ) break;
		l = -l;
	} while( v.read(RBV_REG::IER) == l );
	v.write(RBV_REG::IER, 0x7f);
	v.write(RBV_REG::IER, x);
	BOOST_TEST( l == 0 );
}
BOOST_AUTO_TEST_SUITE_END();
