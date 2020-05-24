#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "wrapper.hpp"
#include <ios>
#include <boost/test/tools/output_test_stream.hpp>
#include <sstream>
using namespace ROM;
static std::unique_ptr<std::ostringstream> modem_out;
struct EX_DONE {};
namespace ROM {
void run_rom(uint32_t v) { cpu.PC = v; }
void boot() { throw EX_DONE(); }
bool send_to_pb_key() { return MOCK::invoke<bool>("send_to_pb_key"); }
void play_chime_4() { MOCK::invoke<void>("play_chime_4"); }
void getHWInfo2_common() {
	motherboard = &motherboards[0];
	model = &old_machines[0];
	INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_IN_CMD );
	INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ENABLE );
	INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ASCII );
}
void getHWInfo2(uint8_t d0) { MOCK::invoke<void>("getHWInfo2", d0); }
RW_offset get_scc_addr() {	return { 0, 0 }; }
void init_modem_port() { MOCK::invoke<void>("init_modem_port"); }
void reset_VIA1_timer2() { MOCK::invoke<void>("reset_VIA1_timer2"); }
void write_modem_port_byte(uint8_t v) {
	modem_out->put(v);
}
uint32_t modem_port_send1(uint32_t d1, uint32_t v) { return MOCK::invoke<uint32_t>("modem_port_send1", d1, v); }
std::pair<int8_t, uint8_t> read_modem_port() {
	return MOCK::invoke<std::pair<int8_t, uint8_t>>("read_modem_port");
}
}
void prepare(std::string_view sv, int k = 1) {
	clear_global();
	modem_out = std::make_unique<std::ostringstream>();
	fixture f;
	DEFINE_ROM( 45C52 );
	DEFINE_ROM( 47752 );
	DEFINE_ROM( 47826 );
	DEFINE_ROM( B9A4E );
	DEFINE_ROM( B9F46 );
	DEFINE_ROM( B9F78 );
	DEFINE_ROM( B9FDA );
	DEFINE_ROM( BA090 );
	DEFINE_ROM( BA0EA );
	DEFINE_ROM( BA138 );
	DEFINE_ROM( BA182 );
	DEFINE_ROM( BA964 );
	MOCK::reset_all();
	MOCK::make<void()>("boot")->always_do( []() {} );
	MOCK::make<bool()>("send_to_pb_key")->always( true );
	MOCK::make<void()>("play_chime_4")->always_do( []() {} );
	MOCK::make<void()>("init_modem_port")->always_do( []() {} );
	MOCK::make<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
					 } );
	auto send = MOCK::make<uint32_t(uint32_t, uint32_t)>("modem_port_send1");

	auto read = MOCK::make<std::pair<int8_t, uint8_t>()>("read_modem_port");
	for(unsigned int i = 0; i < sv.size(); ++i ) {
		read->nth_is( i+1, std::make_pair( 0, sv[i] ) );
	}
	for(int i = 0; i < k-1; ++i ) {
		send->nth_do(i+1, [](uint32_t v, uint32_t) { return v; } );
	}
	send->nth_do( k, [](uint32_t, uint32_t) -> uint32_t { throw EX_DONE(); } );
	read->nth_is( sv.size()+1, std::make_pair( 0x80, 0 ) );
	MOCK::make<void()>("reset_VIA1_timer2")->always_do( []() {} );
}
void run_test() {
	if( rom ) {
		BOOST_CHECK_THROW( test_rom( 0x408B98BC ), EX_DONE );
	} else {
		BOOST_CHECK_THROW( ROMWrapper::run_B98BC(), EX_DONE );
		DR(7) = ROM::INIT_FLAGS.to_ulong() << 16 | ROM::EXCEPTION_STATUS;
	}
}	
BOOST_AUTO_TEST_CASE( pb ) {
	prepare("?");
	DR(4) = 0;
	DR(6) = 0;
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 HWCFG_FLAGS.set( HWCFG_T::POWERBOOK );
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ENABLE );
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_IN_CMD );
					 } );
	run_test();
	MOCK::verify( "send_to_pb_key" );
}
BOOST_AUTO_TEST_CASE( unknown_hw ) {
	prepare("?");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_ENABLE );
					 } );
	run_test();
	MOCK::verify( "play_chime_4", 0 );
}

BOOST_AUTO_TEST_CASE( known_hw_err ) {
	prepare("?");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_ENABLE );
						 INIT_HW_FLG.set( INIT_HW_FLG_T::KNOWN_HW );
					 } );
	run_test();
	MOCK::verify( "play_chime_4", 1 );
}

BOOST_AUTO_TEST_CASE( read_err ) {
	prepare("");
	DR(6) = 0x12345678;
	run_test();
	BOOST_CHECK_EQUAL( modem_out->str(), "" );
}

BOOST_AUTO_TEST_CASE( aster ) {
	prepare("*");
	DR(6) = 0x12345678;
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_IN_CMD );
					 } );
	run_test();
	BOOST_CHECK( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_IN_CMD + 16 ) );
}


BOOST_AUTO_TEST_CASE( cmt ) {
	prepare("ab", 2);
	DR(6) = 0x12345678;
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_IN_CMD );
					 } );
	run_test();
	BOOST_CHECK_EQUAL( modem_out->str(), "ab" );
}
#if 0
BOOST_AUTO_TEST_CASE( stackx ) {
	prepare("!");
	fill =
		[]() {
			INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ENABLE );
			INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_IN_CMD );
		};
	loop = 2;
	AR(7) = 0x1f00;
	DR(6) = 0x12345678;
	run_test();
}
#endif
BOOST_AUTO_TEST_CASE( cmd_s ) {
	prepare("S");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_UNSEND );
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_WAIT );
					 } );
	DR(6) = 0x12345678;	
	run_test();
	BOOST_CHECK_EQUAL( modem_out->str(), "*S\r\n" );
	BOOST_CHECK( ! ( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_UNSEND) ) );
	BOOST_CHECK( ! ( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_WAIT) ) );
}

BOOST_AUTO_TEST_CASE( cmd_l_echo ) {
	prepare("L12345678");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ECHO );
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_CRLF );
					 } );
	DR(4) = 0x11223344;
	run_test();
	BOOST_CHECK_EQUAL( DR(4), 0x3344 );
	BOOST_CHECK_EQUAL( AR(4), 0x12345678 );
	BOOST_CHECK_EQUAL( modem_out->str(), "12345678*L\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_l ) {
	prepare("L12345678");
	DR(4) = 0x11223344;
	run_test();
	BOOST_CHECK_EQUAL( DR(4), 0x3344 );
	BOOST_CHECK_EQUAL( AR(4), 0x12345678 );
	BOOST_CHECK_EQUAL( modem_out->str(), "*L\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_b_echo ) {
	prepare("B1234");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ECHO );
					 } );
	DR(4) = 0x11223344;
	run_test();
	BOOST_CHECK_EQUAL( DR(4), 0x12343344 );
	BOOST_CHECK_EQUAL( modem_out->str(), "1234*B\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_b ) {
	prepare("B1234");
	DR(4) = 0x11223344;
	run_test();
	BOOST_CHECK_EQUAL( DR(4), 0x12343344 );
	BOOST_CHECK_EQUAL( modem_out->str(), "*B\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_d ) {
	prepare("D313233");
	DR(4) = 0x30000;
	AR(4) = 0x1000;
	run_test();
	BOOST_CHECK_EQUAL( AR(4), 0x1003 );
	BOOST_CHECK_EQUAL( read_b( 0x1000), 0x31 );
	BOOST_CHECK_EQUAL( read_b( 0x1001), 0x32 );
	BOOST_CHECK_EQUAL( read_b( 0x1002), 0x33 );
}


BOOST_AUTO_TEST_CASE( cmd_c ) {
	prepare("C");
	write_b( 0x1000, 0x12 );
	write_b( 0x1001, 0x34 );
	write_b( 0x1002, 0x56 );
	DR(4) = 0x30000;
	AR(4) = 0x1000;
	run_test();
	BOOST_CHECK_EQUAL( AR(4), 0x1003 );
	BOOST_CHECK_EQUAL( modem_out->str(), "0000009C*C\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_g ) {
	prepare("G00001000");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ECHO );
					 } );
	if( rom ) {
		test_rom( 0x408B98BC, 0x1000 );
	} else {
		BOOST_CHECK_THROW( ROMWrapper::run_B98BC(), EX_DONE );
	}
	BOOST_TEST( cpu.PC = 0x1000 );
}

BOOST_AUTO_TEST_CASE( cmd_0_echo ) {
	prepare("000002A00");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ECHO );
					 } );
	run_test();
	BOOST_CHECK_EQUAL( AR(0), 0x2A00 );
	BOOST_CHECK_EQUAL( modem_out->str(), "00002A00*0\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_0_no_echo ) {
	prepare("000002A00");
	run_test();
	BOOST_CHECK_EQUAL( AR(0), 0x2A00 );
	BOOST_CHECK_EQUAL( modem_out->str(), "*0\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_1_echo ) {
	prepare("100002B00");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ECHO );
					 } );
	run_test();
	BOOST_CHECK_EQUAL( AR(1), 0x2B00 );
	BOOST_CHECK_EQUAL( modem_out->str(), "00002B00*1\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_1_no_echo ) {
	prepare("100002B00");
	run_test();
	BOOST_CHECK_EQUAL( AR(1), 0x2B00 );
	BOOST_CHECK_EQUAL( modem_out->str(), "*1\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_2 ) {
	prepare("212345678");
	cpu.S = true;
	run_test();
	BOOST_CHECK_EQUAL( mmu->movec_from(2), 0x12345678 & 0x80008000 );
	BOOST_CHECK_EQUAL( modem_out->str(), "*2\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_3_68040 ) {
	prepare("3");
	cpu.S = true;
	run_test();
	BOOST_CHECK( ! (mmu->movec_from(3) & 1 << 15) );
	BOOST_CHECK_EQUAL( modem_out->str(), "*3\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_4 ) {
	prepare("4");
	DR(7) = 0x12345678;
	DR(6) = 0xabcd0123;
	run_test();
	BOOST_CHECK_EQUAL( DR(7) & 0xffff, 0 );
	BOOST_CHECK_EQUAL( DR(6), 0 );
	BOOST_CHECK_EQUAL( modem_out->str(), "*4\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_5 ) {
	prepare("5");
	run_test();
	MOCK::verify("reset_VIA1_timer2");
	BOOST_CHECK( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_WAIT + 16) );
	BOOST_CHECK( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_UNSEND + 16) );
	BOOST_CHECK_EQUAL( modem_out->str(), "*5\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_6 ) {
	prepare("6");
	BOOST_CHECK_NO_THROW( run_test() );
	BOOST_CHECK_EQUAL( modem_out->str(), "*6\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_7 ) {
	prepare("7");
	run_test();
	BOOST_CHECK( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_UNKNOWN + 16) );
	BOOST_CHECK_EQUAL( modem_out->str(), "*7\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_A ) {
	prepare("A");
	run_test();
	BOOST_CHECK( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_ASCII + 16) );
	BOOST_CHECK_EQUAL( modem_out->str(), "*A\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_H ) {
	prepare("H");
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ASCII );
					 } );
	run_test();
	BOOST_CHECK( ! ( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_UNKNOWN + 16) ));
	BOOST_CHECK_EQUAL( modem_out->str(), "*H\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_R ) {
	prepare("R");
	DR(6) = 0x12345678;
	MOCK::get<void(uint8_t)>("getHWInfo2")
		->always_do( [](uint8_t v) {
						 getHWInfo2_common();
						 EXCEPTION_STATUS = 0xABCD;
					 } );
	run_test();
	BOOST_CHECK_EQUAL( modem_out->str(), "12345678ABCD*R\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_M ) {
	prepare("M");
	write_l( 0x1000, 0x12345678 );
	write_l( 0x1004, 0xabcd1234 );
	write_l( 0x1008, 0xdeadbeaf );
	DR(4) = 0x30000;
	AR(4) = 0x1000;
	run_test();
	BOOST_CHECK_EQUAL( DR(4), 0xffff0000 );
	BOOST_CHECK_EQUAL( AR(4), 0x100C );
	BOOST_CHECK_EQUAL( modem_out->str(), "12345678\r\nABCD1234\r\nDEADBEAF\r\n*M\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_E ) {
	prepare("E");
	run_test();
	BOOST_CHECK( DR(7) & 1 << (INIT_FLAG_T::MODEM_PORT_ECHO + 16) );
	BOOST_CHECK_EQUAL( modem_out->str(), "*E\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_I ) {
	prepare("I");
	if( rom ) {
		test_rom( 0x408B98BC, 0x4080008C );
	} else {
		BOOST_CHECK_THROW( ROMWrapper::run_B98BC(), EX_DONE );
	}
}

BOOST_AUTO_TEST_CASE( cmd_P ) {
	prepare("P0000");
	run_test();
	BOOST_CHECK_EQUAL( modem_out->str(), "*ERROR**P\r\n" );
}

BOOST_AUTO_TEST_CASE( cmd_T_0){
	prepare("T000000001234");
	test_rom( 0x408B98BC, 0x408B9D82 );
}
