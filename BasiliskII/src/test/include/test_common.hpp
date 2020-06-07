#pragma once
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "registers.hpp"
#include "prototype.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "machine.hpp"
#include <iostream>
#include <ostream>
#include "fpu/68881.hpp"
#include "io.hpp"
#include "wrapper.hpp"
#include "mock.hpp"
#include <deque>
#define FPU_R (*(M68040_FPU*)fpu)
extern uint8_t *RAMBaseHost;
struct conds {
	int cc;
	bool C, Z, V, N;
};
void test_rom(uint32_t begin, uint32_t end = 0);
std::ostream& operator<<(std::ostream& os, const conds &cd);
extern std::vector<conds> t_data;
extern std::vector<conds> f_data;
inline void test_write8(uint32_t addr, uint8_t v) {
	RAMBaseHost[addr] = v;
}
inline void test_write16(uint32_t addr, uint16_t v) {
	RAMBaseHost[addr] = v >> 8;
	RAMBaseHost[addr+1] = v;
}
inline void test_write32(uint32_t addr, uint32_t v) {
	RAMBaseHost[addr] = v >> 24;
	RAMBaseHost[addr+1] = v >> 16;
	RAMBaseHost[addr+2] = v >> 8;
	RAMBaseHost[addr+3] = v;
}

inline uint8_t test_read8(uint32_t addr) {
	return RAMBaseHost[addr];
}

inline uint16_t test_read16(uint32_t addr) {
	return RAMBaseHost[addr] << 8 | RAMBaseHost[addr+1];
}
inline uint32_t test_read32(uint32_t addr) {
	return RAMBaseHost[addr] << 24 |
		RAMBaseHost[addr+1] << 16 |
		RAMBaseHost[addr+2] << 8 |
		RAMBaseHost[addr+3];
}
inline void test_in_exception(int v) {
	if( v == 0 ) {
		BOOST_TEST( cpu.PC < 0x20000 );
	} else {
		BOOST_TEST( cpu.S == true );
		BOOST_TEST( cpu.PC == 0x20000 + (v*0x100));
	}
}
enum EA_V { DR, AR, MEM, INC, DEC, D16, D8XN, ADDR_W, ADDR_L, D16PC, D8PCXN, IMM };
extern const std::vector<EA_V> ea_all, ea_data, ea_memory,
							   ea_control, ea_write, ea_write2, ea_writedn;
uint32_t test_ea_b(CPU& cpu, const std::vector<uint16_t>& v,
			   uint8_t before,
			   EA_V mode,
			   uint8_t after) ;
uint32_t test_ea_w(CPU& cpu, const std::vector<uint16_t>& v,
			   uint16_t before,
			   EA_V mode,
			   uint16_t after) ;
uint32_t test_ea_l(CPU& cpu, const std::vector<uint16_t>& v,
			   uint32_t before,
			   EA_V mode,
			   uint32_t after);
extern bool dump_;
extern bool rom;
struct fixture {
	fixture(MB_TYPE m = MB_TYPE::GLU);
};

inline std::string to_string(const std::vector<uint8_t>& v) {
	return std::string(reinterpret_cast<const char*>(&*v.begin()), reinterpret_cast<const char*>(&*v.end()));
}

class STREAM_TEST {
	std::string name;
	std::deque<bool> out_;
	std::deque<bool> in_;
public:
	STREAM_TEST(std::string_view s)	:name(s) {}
	void set_read_data(const std::vector<bool>& s) { in_.assign(s.begin(), s.end() ); }
	void verify(const std::vector<bool>& s) {
		BOOST_CHECK_EQUAL_COLLECTIONS(out_.begin(), out_.end(), s.begin(), s.end() );
	}
	void out(bool c) { out_.push_back( c ); }
	bool in();
	void reset() { out_.clear(); }
};
class IO_TEST_IMPL {
	std::unordered_map<uint32_t, std::unique_ptr<std::ostringstream>> written;
	std::unordered_map<uint32_t, std::unique_ptr<std::istringstream>> to_read;
public:
	void set_read_data(uint32_t addr, const std::vector<uint8_t>& data);
	void write(uint32_t addr, uint8_t v);
	uint8_t read(uint32_t addr);
	void verify(uint32_t addr, const std::vector<uint8_t>& expected);
};
template<typename Base>
struct IO_TEST : public Base, private IO_TEST_IMPL {
	template<typename... Args>
	IO_TEST(Args... args) :Base(args...) {}
public:
	void set_read_data(uint32_t addr, const std::vector<uint8_t>& data) {
		IO_TEST_IMPL::set_read_data(addr, data);
	}
	void write(int addr, uint8_t v) override {
		IO_TEST_IMPL::write(addr, v);
	}
	uint8_t read(int addr) override {
		return IO_TEST_IMPL::read(addr);
	}
	void verify(uint32_t addr, const std::vector<uint8_t>& expected) {
		IO_TEST_IMPL::verify(addr, expected);
	}
};
void test_cpu(const std::vector<uint16_t>& c);
void update_global();
void back_global();
void clear_global();
#define DATA_PATH "/home/michiaki/Document/macemu/macemu/BasiliskII/src/rom/data/"
#define TEST_ROM( begin_ ) if( rom ) { \
		test_rom( 0x408##begin_ ); \
	} else { \
		ROMWrapper::run_##begin_();				\
		BOOST_REQUIRE_EQUAL( cpu.PC, 0 );		\
	}
#define TEST_ROM2( begin_, end_ ) if( rom ) { \
		test_rom( 0x408##begin_, 0x408##end_ ); \
	} else { \
		ROMWrapper::run_##begin_();				\
		BOOST_REQUIRE_EQUAL( cpu.PC, 0x408##end_ );	\
	}

