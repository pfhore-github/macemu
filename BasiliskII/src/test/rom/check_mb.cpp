#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "glu.hpp"
#include "rbv.hpp"
#include "oss.hpp"
#include "mcu.hpp"
#include "mdu.hpp"
#include "v8.hpp"
#include "jaws.hpp"
#include "mem.hpp"
using namespace ROM;
template<class T>
void init() {
	fixture f(std::make_unique<T>());
	set_sr( 0x2700 );
	rom_base = 0x40800000;
	cpu.VBR = 0x40803DD8;
	MOCK::reset_all();
	MOCK::make<bool(uint32_t, uint32_t)>("has_via");
	DEFINE_ROM( 046AC );

}
template<class T>
class BROKEN_CHIP : public T {
public:
	template<typename ... Args>	
	BROKEN_CHIP(Args... args) :T(args...) {}
	uint8_t read(int addr) override { bus_error(addr, 1); }
	void write(int addr, uint8_t v) override { bus_error(addr, 5); }
};
namespace ROM {
bool has_via(uint32_t v1, uint32_t v2) { return MOCK::get<bool(uint32_t, uint32_t)>("has_via")->invoke(v1, v2); }
}
#define TEST_SUCCESS 02FB0
#define TEST_FAILURE 02FA4
namespace bdata = boost::unit_test::data;
struct glu_fixture {
	glu_fixture() {
		init<GLU>();
		AR(0) = rom_base | ROM::motherboards[0].BASE;
	}
};
BOOST_FIXTURE_TEST_SUITE(test_GLU, glu_fixture);

BOOST_AUTO_TEST_CASE( success )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x20000 );
	TEST_ROM2( 030AC, 02FB0 );
}
BOOST_AUTO_TEST_CASE( failure )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0x20000 );
	TEST_ROM2( 030AC, 02FA4 );
}
BOOST_AUTO_TEST_SUITE_END();
struct mdu_fixture {
	mdu_fixture() {
		init<MDU>();
		AR(0) = rom_base | ROM::motherboards[1].BASE;
	}
};
BOOST_FIXTURE_TEST_SUITE(test_MDU, mdu_fixture);

BOOST_AUTO_TEST_CASE( success )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, false, 0x50F01C00, 0x20000 );
	TEST_ROM2( 030CA, 02FB0 );
}
BOOST_AUTO_TEST_CASE( failure1  )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0x40000 );
	TEST_ROM2( 030CA, 02FA4 );	
}

BOOST_AUTO_TEST_CASE( failure2 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, true, 0x50F01C00, 0x20000 );
	TEST_ROM2( 030CA, 02FA4 );		
}

BOOST_AUTO_TEST_CASE( failure3 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, false, 0x50F01C00, 0x20000 );
	machine->rbv = std::make_shared<BROKEN_CHIP<RBV>>();
	TEST_ROM2( 030CA, 02FA4 );		
}

BOOST_AUTO_TEST_CASE( failure4 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, false, 0x50F01C00, 0x20000 );
	machine->vdac = std::make_shared<BROKEN_CHIP<VDAC>>();
	TEST_ROM2( 030CA, 02FA4 );		
}



BOOST_AUTO_TEST_SUITE_END();
struct oss_fixture {
	oss_fixture() {
		init<OSS>();
		AR(0) = rom_base | ROM::motherboards[2].BASE;
	}
};
BOOST_FIXTURE_TEST_SUITE(test_oss, oss_fixture);

BOOST_AUTO_TEST_CASE( success )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, false, 0x50F01C00, 0x20000 );
	TEST_ROM2( 0310A, 02FB0 );	
}
BOOST_AUTO_TEST_CASE( failure1 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0x40000 );
	motherboard = &ROM::motherboards[2];
	AR(0) = rom_base | motherboard->BASE;
	TEST_ROM2( 0310A, 02FA4 );
}
BOOST_AUTO_TEST_CASE( failure2 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, true, 0x50F01C00, 0x20000 );
	TEST_ROM2( 0310A, 02FA4 );
}

BOOST_AUTO_TEST_CASE( failure3 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x40000 )
		.nth_is( 2, false, 0x50F01C00, 0x20000 );
	machine->oss = std::make_shared<BROKEN_CHIP<OSS_REG>>();
	
	TEST_ROM2( 0310A, 02FA4 );
}

BOOST_AUTO_TEST_SUITE_END();
struct v8_fixture {
	v8_fixture() {
		init<V8>();
		AR(0) = rom_base | ROM::motherboards[3].BASE;
	}
};

BOOST_FIXTURE_TEST_SUITE(check_v8, v8_fixture);

BOOST_AUTO_TEST_CASE( success )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0 )
		.nth_is( 2, false, 0x50F01C00, 0x40000 );
	TEST_ROM2( 03166, 02FB0 );	
}

BOOST_AUTO_TEST_CASE( failure1 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0 );
	TEST_ROM2( 03166, 02FA4 );	
}

BOOST_AUTO_TEST_CASE( failure2 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0 )
		.nth_is( 2, true, 0x50F01C00, 0x40000 );
	TEST_ROM2( 03166, 02FA4 );	
	
}

BOOST_AUTO_TEST_CASE( failure3 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0 )
		.nth_is( 2, false, 0x50F01C00, 0x40000 );
	machine->rbv = std::make_shared<BROKEN_CHIP<RBV>>();
	TEST_ROM2( 03166, 02FA4 );		
}

BOOST_AUTO_TEST_CASE( failure4 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0 )
		.nth_is( 2, false, 0x50F01C00, 0x40000 );
	machine->vdac = std::make_shared<BROKEN_CHIP<VDAC>>();
	TEST_ROM2( 03166, 02FA4 );		
}

BOOST_AUTO_TEST_SUITE_END();
struct mcu_fixture {
	mcu_fixture() {
		init<MCU>();
		AR(0) = rom_base | ROM::motherboards[4].BASE;
	}
};

BOOST_FIXTURE_TEST_SUITE(check_mcu, mcu_fixture);
BOOST_AUTO_TEST_CASE( sucess )  {
	TEST_ROM2( 031A0, 02FB0 );		
}
BOOST_AUTO_TEST_CASE( failure )  {
	machine->mcu = std::make_shared<BROKEN_CHIP<MCU_REG>>();
	TEST_ROM2( 031A0, 02FA4 );		
}

BOOST_AUTO_TEST_SUITE_END();
struct jaws_fixture {
	jaws_fixture() {
		init<JAWS>();
		AR(0) = rom_base | ROM::motherboards[5].BASE;
	}
};
BOOST_FIXTURE_TEST_SUITE(test_jaws, jaws_fixture);
BOOST_AUTO_TEST_CASE( success )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x100000 )
		.nth_is( 2, false, 0x50F01C00, 0x80000 )
		.nth_is( 3, false, 0x50F01C00, 0x40000 );
	TEST_ROM2( 031AE, 02FB0 );			
}
BOOST_AUTO_TEST_CASE( failure1 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0x100000 );
	TEST_ROM2( 031AE, 02FA4 );		
}

BOOST_AUTO_TEST_CASE( failure2 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x100000 )
		.nth_is( 2, true, 0x50F01C00, 0x80000 );
	TEST_ROM2( 031AE, 02FA4 );		
}

BOOST_AUTO_TEST_CASE( failure3 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x100000 )
		.nth_is( 2, false, 0x50F01C00, 0x80000 )
		.nth_is( 3, true, 0x50F01C00, 0x40000 );
	TEST_ROM2( 031AE, 02FA4 );		
}

BOOST_AUTO_TEST_CASE( failure4 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0x100000 )
		.nth_is( 2, false, 0x50F01C00, 0x80000 )
		.nth_is( 3, false, 0x50F01C00, 0x40000 );
	machine->pb = std::make_shared<BROKEN_CHIP<JAWS_REG>>();
	TEST_ROM2( 031AE, 02FA4 );		
}
BOOST_AUTO_TEST_SUITE_END();
struct niagara_fixture {
	niagara_fixture() {
		init<Niagara>();
		AR(0) = rom_base | ROM::motherboards[8].BASE;
	}
};
BOOST_FIXTURE_TEST_SUITE(test_niagara, niagara_fixture);

BOOST_AUTO_TEST_CASE( success )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0xFF200000 )
		.nth_is( 2, false, 0x50F01C00, 0x80000 )
		.nth_is( 3, false, 0x50F01C00, 0x40000 );
	TEST_ROM2( 031F2, 02FB0 );			
}

BOOST_AUTO_TEST_CASE( failure1 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, true, 0x50F01C00, 0xFF200000 );
	TEST_ROM2( 031F2, 02FA4 );			
}

BOOST_AUTO_TEST_CASE( failure2 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0xFF200000 )
		.nth_is( 2, true, 0x50F01C00, 0x80000 );
	TEST_ROM2( 031F2, 02FA4 );			
}

BOOST_AUTO_TEST_CASE( failure3 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0xFF200000 )
		.nth_is( 2, false, 0x50F01C00, 0x80000 )
		.nth_is( 3, true, 0x50F01C00, 0x40000 );
	TEST_ROM2( 031F2, 02FA4 );			
}

BOOST_AUTO_TEST_CASE( failure4 )  {
	MOCK::get<bool(uint32_t, uint32_t)>("has_via")
		->nth_is( 1, false, 0x50F01C00, 0xFF200000 )
		.nth_is( 2, false, 0x50F01C00, 0x80000 )
		.nth_is( 3, false, 0x50F01C00, 0x40000 );
	machine->pb = std::make_shared<BROKEN_CHIP<PB_REG>>();
	TEST_ROM2( 031F2, 02FA4 );			
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_CASE( check_none )  {
	TEST_ROM2( 0323A, 02FB0 );		
}

