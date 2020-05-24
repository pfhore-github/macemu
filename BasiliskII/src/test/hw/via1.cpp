#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "test_common.hpp"
#include <atomic>
#include "via.hpp"
#include "via1.hpp"
#include "rtc.hpp"
#include "adb.h"
#include "machine.hpp"
namespace bdata = boost::unit_test::data;
VIA1 tester(1);
BOOST_AUTO_TEST_SUITE(via1);
BOOST_DATA_TEST_CASE( regA, bdata::xrange(8), x )  {
	machine->model_map[0] = (x) & 1;
	machine->model_map[1] = (x >> 1) & 1;
	machine->model_map[2] = (x >> 2) & 1;
	machine->model_map[3] = (x >> 3) & 1;
	uint8_t v = tester.read( 15  );
	BOOST_TEST( (v >> 1 & 1) == (x & 1) ); 
	BOOST_TEST( (v >> 2 & 1) == (x >> 1& 1) ); 
	BOOST_TEST( (v >> 4 & 1) == (x >> 2& 1) ); 
	BOOST_TEST( (v >> 6 & 1) == (x >> 3& 1) ); 
}
BOOST_AUTO_TEST_SUITE_END();
