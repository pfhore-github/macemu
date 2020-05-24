#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
namespace ROM {
std::optional<uint16_t> get_model_id() { return MOCK::invoke<std::optional<uint16_t>>("get_model_id"); }
uint32_t read_id_from_via() { return MOCK::invoke<uint32_t>("read_id_from_via"); }
}
using namespace ROM;
// $4B6C
void prepare() {
	fixture f;
	rom_base = 0x40800000 ;
	set_sr( 0x2700 );
	MOCK::reset_all();
	MOCK::make<std::optional<uint16_t>()>("get_model_id");
	MOCK::make<uint32_t()>("read_id_from_via");
	DEFINE_ROM( 04BE4 );
	DEFINE_ROM( 047CC );
}
BOOST_AUTO_TEST_CASE( not_found )  {
	prepare();
	MOCK::get<std::optional<uint16_t>()>("get_model_id")->always( std::optional<uint16_t>() );
	TEST_ROM2( 04B6C, 02F6A );
}

BOOST_DATA_TEST_CASE( found, bdata::xrange(26), id )  {
	prepare();
	MOCK::get<std::optional<uint16_t>()>("get_model_id")->always( new_machines[id].machine_id );
	MOCK::get<uint32_t()>("read_id_from_via")->always( new_machines[id].model_id );
	TEST_ROM2( 04B6C, 02F82 );
	BOOST_TEST( AR(1) == (rom_base | new_machines[id].BASE) );
}

