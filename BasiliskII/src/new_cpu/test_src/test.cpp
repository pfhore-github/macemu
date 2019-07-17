#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <unordered_map>
#include "../registers.hpp"
#include "../exceptions.hpp"
uint8_t *RAMBaseHost = 0;
uint32_t ROMBaseMac = 0;
CPU xcpu;
using op_type = void (*)(CPU*, uint16_t op, int dn, int mode, int reg);
extern op_type op_list[65536];
#include "init.h"

init::init() {
	if( ! RAMBaseHost ) {
		RAMBaseHost = new uint8_t[1024*1024*4];
		xcpu.mmu = new NonMMU(&xcpu);
	}
	xcpu.init();
	xcpu.reset();
}
init::~init() {
}
fixture::fixture() {
	memset(&xcpu.R, 0, sizeof(uint32_t)*15);
	memset(RAMBaseHost, 0, 4098);
	xcpu.X = xcpu.C = xcpu.V = xcpu.Z = xcpu.N = false;
	xcpu.S = false;
	xcpu.A[7] = 3*1024*1024;
}
struct M68kRegisters {};
void EmulOp(unsigned short, M68kRegisters*) {}
void test_cpu(const std::vector<uint16_t>& c) {
	for(int i = 0; i < c.size(); ++i) {
		RAMBaseHost[2*i] = c[i] >> 8;
		RAMBaseHost[2*i+1] = c[i] & 0xff;
	}
	xcpu.PC = 0;
	while( xcpu.PC < 2*c.size() ) {
		xcpu.NPC = xcpu.PC;
		uint16_t op = xcpu.fetch_w();		
		op_list[op](&xcpu, op, (op>>9)&7, (op>>3)&7, op&7);
	}
}
