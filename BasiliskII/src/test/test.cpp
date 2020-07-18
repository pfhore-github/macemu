#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "fpu/68881.hpp"
#include "registers.hpp"
#include "test_common.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include <fenv.h>
#include "main.h"
#include "glu.hpp"
#include "via_test.hpp"
#include "fmt/core.h"
#include <iostream>
bool is_quadra = true;
int PrefsFindInt32(const std::string& s) {
	return 0;
}
uint32_t rom_base;
namespace ROM {
extern std::unordered_map<uint32_t, std::function<void()>> rom_routines;
}
extern "C" void Execute68k(uint32_t addr, struct M68kRegisters *r)
{
	auto stub = ROM::rom_routines.find(addr & 0xfffff);
	if(stub != ROM::rom_routines.end() ) {
		memcpy(&cpu.R[0], r, sizeof(uint32_t)*16);
		set_sr(r->sr);
		(stub->second)( );
		memcpy(r, &cpu.R[0], sizeof(uint32_t)*16);
		r->sr = get_sr();
	}
}
void QuitEmulator() {}
void emul_reset(unsigned int*,unsigned int*) {}
uint8_t XPRAM[256];
const std::vector<EA_V> ea_all = {
	DR, AR, MEM, INC, DEC, D16, D8XN, ADDR_W, ADDR_L, D16PC, D8PCXN, IMM
}, ea_data = {
	DR, MEM, INC, DEC, D16, D8XN, ADDR_W, ADDR_L, D16PC, D8PCXN, IMM
}, ea_memory = {
	MEM, INC, DEC, D16, D8XN, ADDR_W, ADDR_L, D16PC, D8PCXN, IMM
}, ea_control = {
	MEM, D16, D8XN, ADDR_W, ADDR_L, D16PC, D8PCXN
}, ea_write = {
	DR, AR, MEM, D16, D8XN, ADDR_W, ADDR_L,
}, ea_write2 = {
	MEM, D16, D8XN, ADDR_W, ADDR_L,
}, ea_writedn = {
	DR, MEM, D16, D8XN, ADDR_W, ADDR_L,
};
const char* op[] = {
	"T", "F", "HI", "LS", "CC", "CS", "NE", "EQ", "VC", "VS",
	"PL", "MI", "GE", "LT", "GT", "LE" };
std::ostream& operator<<(std::ostream& os, const conds &cd) {
	return os << op[cd.cc] << ":" <<
		cd.C << "," << cd.Z << "," << cd.V << "," << cd.N;
}
#include "machine.hpp"
using op_type = void (*)(CPU*, uint16_t op, int dn, int mode, int reg);
extern op_type op_list[65536];
std::vector<conds> t_data = {
	{  0, false, false, false, false }, // T
	{  2, false, false, false, false }, // HI
	{  3, true, false, false, false },  // LS(C)
	{  3, false, true, false, false },  // LS(Z)
	{  4, false, true, false, false },  // CC
	{  5, true, true, false, false },   // CS
	{  6, false, false, false, false }, // NE
	{  7, false, true, false, false },  // EQ
	{  8, false, false, false, false }, // VC
	{  9, false, false, true, false },  // VS
	{ 10, false, false, false, false }, // PL
	{ 11, false, false, false, true },  // MI
	{ 12, false, false, true, true },	// GE(NV)
	{ 12, false, false, false, false }, // GE(~N~V)
	{ 13, false, false, true, false },	// LT(N~V)
	{ 13, false, false, false, true },  // LT(~NV)
	{ 14, false, false, true, true },	// GT(NV~Z)
	{ 14, false, false, false, false }, // GT(~N~V~Z)
	{ 15, false, true, false, false },	// LE(Z)
	{ 15, false, false, false, true }, // LE(N~V)
	{ 15, false, false, true, false }, // LE(~NV)
};
std::vector<conds> f_data = {
	{  1, false, false, false, false }, // F
	{  2, true, false, false, false },  // !HI(C)
	{  2, false, true, false, false },  // !HI(Z)
	{  3, false, false, false, false }, // !LS(C)
	{  4, true, true, false, false },   // !CC
	{  5, false, true, false, false },  // !CS
	{  6, false, true, false, false },  // !NE
	{  7, false, false, false, false }, // !EQ
	{  8, false, false, true, false },  // !VC
	{  9, false, false, false, false }, // !VS
	{ 10, false, false, false, true },  // !PL
	{ 11, false, false, false, false }, // !MI
	{ 12, false, false, true, false },	// !GE(N~V)
	{ 12, false, false, false, true },  // !GE(~NV)
	{ 13, false, false, true, true },	// !LT(NV)
	{ 13, false, false, false, false }, // !LT(~N~V)
	{ 14, false, true, false, false },	// !GT(Z)
	{ 14, false, false, false, true },  // !GT(N~V)
	{ 14, false, false, true, false },  // !GT(~NV)
	{ 15, false, false, true, true },	// !LE(NV~Z)
	{ 15, false, false, false, false }, // !LE(~N~V~Z)
};
CPU cpu;
fixture::fixture()
	:fixture(std::make_unique<GLU>()) {}

fixture::fixture(std::unique_ptr<Machine>&& m) {
	machine = std::move(m);
	memset(&cpu.R, 0, sizeof(uint32_t)*15);
	cpu.X = cpu.C = cpu.V = cpu.Z = cpu.N = false;
	cpu.S = false;
	cpu.R[8+7] = 3*1024;
	fpu->reset();
	machine->rom_mirror = false;
	rom_base = 0x40800000;
	((MC68040_MMU*)mmu)->MMU_SR = 0;
	// set exception vector
	cpu.VBR = 0x10000;
	for(int i = 1; i < 256; ++i ) {
		test_write32(0x10000+(i<<2), 0x20000+i*0x100);
	}
	ROM::rom_routines.clear();
}
void EmulOp(unsigned short, M68kRegisters*) {}
extern bool dump_;
void test_cpu(const std::vector<uint16_t>& c) {
	dump_ = false;
	machine->rom_mirror = false;
	for(unsigned int i = 0; i < c.size(); ++i) {
		RAMBaseHost[2*i] = c[i] >> 8;
		RAMBaseHost[2*i+1] = c[i] & 0xff;
	}
	cpu.PC = 0;
	while( cpu.PC < 2*c.size() ) {
		cpu.do_op();
	}
}
uint32_t test_ea_b(CPU& cpu, const std::vector<uint16_t>& v,
			   uint8_t before,
			   EA_V mode,
			   uint8_t after) {
	std::vector<uint16_t> nv = v;
	int pc = nv.size()*2;
	switch(mode) {
	case DR :
		cpu.R[7] = ( before | 0xffffff00);
		nv[0] |= 7;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( cpu.R[7] , after | 0xffffff00);
		return -1;
	case AR :
		return -1;
	case MEM :
		cpu.R[14] = 0x1000;
		test_write8(0x1000, before);
		nv[0] |= 026;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1000) ,  after);
		return 0x1000;
	case INC :
		cpu.R[14] = 0x1000;
		test_write8(0x1000, before);
		nv[0] |= 036;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1000) ,  after);
		BOOST_CHECK_EQUAL( cpu.R[14] ,  0x1001 );
		return 0x1000;
	case DEC :
		cpu.R[14] = 0x1001;
		test_write8(0x1000, before);
		nv[0] |= 046;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1000) ,  after);
		BOOST_CHECK_EQUAL( cpu.R[14] ,  0x1000 );
		return 0x1000;
	case D16:
		cpu.R[14] = 0x1000;
		test_write8(0x1030, before);
		nv[0] |= 056;
		nv.push_back(0x30);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1030) ,  after);
		return 0x1030;
	case D8XN:
		cpu.R[14] = 0x1000;
		cpu.R[7] = 0x10;
		test_write8(0x1040, before);
		nv[0] |= 066;
		nv.push_back(0x7830);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1040) ,  after);
		return 0x1040;
	case ADDR_W:
		test_write8(0x1000, before);
		nv[0] |= 070;
		nv.push_back(0x1000);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1000) ,  after);
		return 0x1000;
	case ADDR_L:
		test_write8(0x1000, before);
		nv[0] |= 071;
		nv.push_back(0);
		nv.push_back(0x1000);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read8(0x1000) ,  after);
		return 0x1000;
	case D16PC: 
		test_write8(0x1000 + pc, before);
		nv[0] |= 072;
		nv.push_back(0x1000);
		test_cpu(nv);
		return 0x1000+pc;
	case D8PCXN:
		cpu.R[7] = 0x1000;
		test_write8(0x1030+pc, before);
		nv[0] |= 073;
		nv.push_back(0x7830);
		test_cpu(nv);
		return 0x1030+pc;
	case IMM:
		nv.push_back(before);
		nv[0] |= 074;
		test_cpu(nv);
		return -1;
	}
	return -1;
}

uint32_t test_ea_w(CPU& cpu, const std::vector<uint16_t>& v,
				   uint16_t before,
				   EA_V mode,
				   uint16_t after) {
	std::vector<uint16_t> nv = v;
	int pc = nv.size()*2;
	switch(mode) {
	case DR :
		cpu.R[7] = ( before | 0xffff0000);
		nv[0] |= 7;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( cpu.R[7] ,  (after | 0xffff0000));
		return -1;
	case AR :
		cpu.R[8+6] = (int16_t)before;
		nv[0] |= 016;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( cpu.R[8+6] ,  (int16_t)after );
		return -1;
	case MEM :
		cpu.R[8+6] = 0x1000;
		test_write16(0x1000, before);
		nv[0] |= 026;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1000) ,  after);
		return 0x1000;
	case INC :
		cpu.R[8+6] = 0x1000;
		test_write16(0x1000, before);
		nv[0] |= 036;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1000) ,  after);
		BOOST_CHECK_EQUAL( cpu.R[8+6] ,  0x1002 );
		return 0x1000;
	case DEC :
		cpu.R[8+6] = 0x1002;
		test_write16(0x1000, before);
		nv[0] |= 046;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1000) ,  after);
		BOOST_CHECK_EQUAL( cpu.R[8+6] ,  0x1000 );
		return 0x1000;
	case D16:
		cpu.R[8+6] = 0x1000;
		test_write16(0x1030, before);
		nv[0] |= 056;
		nv.push_back(0x30);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1030) ,  after);
		return 0x1030;
	case D8XN:
		cpu.R[8+6] = 0x1000;
		cpu.R[7] = 0x10;
		test_write16(0x1040, before);
		nv[0] |= 066;
		nv.push_back(0x7830);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1040) ,  after);
		return 0x1040;
	case ADDR_W:
		test_write16(0x1000, before);
		nv[0] |= 070;
		nv.push_back(0x1000);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1000) ,  after);
		return 0x1000;
	case ADDR_L:
		test_write16(0x1000, before);
		nv[0] |= 071;
					nv.push_back(0);
		nv.push_back(0x1000);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read16(0x1000) ,  after);
		return 0x1000;
	case D16PC: 
		test_write16(0x1000 + pc, before);
		nv[0] |= 072;
		nv.push_back(0x1000);
		test_cpu(nv);
		return 0x1000+pc;
	case D8PCXN:
		cpu.R[7] = 0x1000;
		test_write16(0x1030+pc, before);
		nv[0] |= 073;
		nv.push_back(0x7830);
		test_cpu(nv);
		return 0x1030+pc;
	case IMM:
		nv.push_back(before);
		nv[0] |= 074;
		test_cpu(nv);
		return -1;
	}
	return -1;
}

uint32_t test_ea_l(CPU& cpu, const std::vector<uint16_t>& v,
				   uint32_t before,
				   EA_V mode,
				   uint32_t after) {
	std::vector<uint16_t> nv = v;
	int pc = nv.size()*2;
	switch(mode) {
	case DR :
		cpu.R[7] = before;
		nv[0] |= 7;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( cpu.R[7] ,  after );
		return -1;
	case AR :
		cpu.R[8+6] = before;
		nv[0] |= 016;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( cpu.R[8+6] ,  after );
		return -1;
	case MEM :
		cpu.R[8+6] = 0x1000;
		test_write32(0x1000, before);
		nv[0] |= 026;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1000) ,  after);
		return 0x1000;
	case INC :
		cpu.R[8+6] = 0x1000;
		test_write32(0x1000, before);
		nv[0] |= 036;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1000) ,  after);
		BOOST_CHECK_EQUAL( cpu.R[8+6] ,  0x1004 );
		return 0x1000;
	case DEC :
		cpu.R[8+6] = 0x1004;
		test_write32(0x1000, before);
		nv[0] |= 046;
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1000) ,  after);
		BOOST_CHECK_EQUAL( cpu.R[8+6] ,  0x1000 );
		return 0x1000;
	case D16:
		cpu.R[8+6] = 0x1000;
		test_write32(0x1030, before);
		nv[0] |= 056;
		nv.push_back(0x30);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1030) ,  after);
		return 0x1030;
	case D8XN:
		cpu.R[8+6] = 0x1000;
		cpu.R[7] = 0x10;
		test_write32(0x1040, before);
		nv[0] |= 066;
		nv.push_back(0x7830);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1040) ,  after);
		return 0x1040;
	case ADDR_W:
		test_write32(0x1000, before);
		nv[0] |= 070;
		nv.push_back(0x1000);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1000) ,  after);
		return 0x1000;
	case ADDR_L:
		test_write32(0x1000, before);
		nv[0] |= 071;
		nv.push_back(0);
		nv.push_back(0x1000);
		test_cpu(nv);
		BOOST_CHECK_EQUAL( test_read32(0x1000) ,  after);
		return 0x1000;
	case D16PC: 
		test_write32(0x1000 + pc, before);
		nv[0] |= 072;
		nv.push_back(0x1000);
		test_cpu(nv);
		return 0x1000+pc;
	case D8PCXN:
		cpu.R[7] = 0x1000;
		test_write32(0x1030+pc, before);
		nv[0] |= 073;
		nv.push_back(0x7830);
		test_cpu(nv);
		return 0x1030+pc;
	case IMM:
		nv.push_back(before >> 16);
		nv.push_back(before);
		nv[0] |= 074;
		test_cpu(nv);
		return -1;		
	}
	return -1;
}

void IO_TEST_IMPL::set_read_data(uint32_t addr, const std::vector<uint8_t>& data) {
 	to_read[ addr ] = std::make_unique<std::istringstream>( to_string( data ) );
}
void IO_TEST_IMPL::write(uint32_t addr, uint8_t v) {
	if( ! written.count( addr ) ) {
		written[addr] = std::make_unique<std::ostringstream>();
	}
	*written[ addr ] << char(v);
}

uint8_t STREAM_TEST::in() {
	uint8_t c = in_.front();
	if( in_.size() != 1 ) { in_.pop_front(); }
	return c;
}
uint8_t IO_TEST_IMPL::read(uint32_t addr) {
	auto iter = to_read.find(addr);
	if( iter == to_read.end() ) {
		BOOST_FAIL( "expected data is empty at " << addr );
		return 0;
	}
	auto& stream = *(iter->second);
	int c = stream.get();
	if( c == -1 ) {
		std::string s = iter->second->str();
		iter->second = std::make_unique<std::istringstream>( s );
		c = iter->second->get();
	}
	return c;
}
static std::vector<uint8_t> from_string(const std::string& s) {
	std::vector<uint8_t> u(s.size());
	for(size_t n = 0; n < s.size(); ++n ) {
		u[n] = s[n];
	}
	return u;
}
void IO_TEST_IMPL::verify(uint32_t addr, const std::vector<uint8_t>& expected) {
	auto v = written.find(addr);
	if( v == written.end() ) {
		BOOST_ERROR( "unexpected write to " << addr );
	} else {
		BOOST_TEST_INFO("addr = " << addr);
		std::vector<uint8_t> result = from_string( v->second->str() );
		BOOST_CHECK_EQUAL_COLLECTIONS( result.begin(), result.end(),
									   expected.begin(), expected.end() );
	}
}
namespace MOCK {
void reset_all() {
	objects.clear();
}
std::unordered_map<std::string, std::shared_ptr<MOCK::MockBase>> objects;
void verify(const std::string& name, int cnt) {
	BOOST_REQUIRE( objects.count(name) );
	objects[name]->verify(cnt);
}
}
void adb_set_state(int) __attribute__((weak));
void adb_set_state(int) {}
#include "adb.h"
uint8_t ADB_Bus::get_from() { return 0;}

extern uint32_t ROMSize;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "machine.hpp"
#include "asc.hpp"
#include <unistd.h>
#include <stdlib.h>

