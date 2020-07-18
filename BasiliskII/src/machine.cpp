#include "machine.hpp"
std::shared_ptr<IO_BASE> IO_NONE::SELF = std::make_shared<IO_NONE>();
std::shared_ptr<IO_BASE> Machine::get_via2() {
	if( via2 != IO_NONE::SELF )
		return via2;
	if( rbv != IO_NONE::SELF )
		return rbv;
	return {};
}
Machine::Machine() {
	via2 = IO_NONE::SELF;
	iop = IO_NONE::SELF;
	rbv = IO_NONE::SELF;
	vdac = IO_NONE::SELF;
	ether = IO_NONE::SELF;
	// machine specific
	oss  = IO_NONE::SELF;
	exp0  = IO_NONE::SELF;
	exp1   = IO_NONE::SELF;
	pb  = IO_NONE::SELF;
	pb2  = IO_NONE::SELF;
	mcu  = IO_NONE::SELF;
	reg1  = IO_NONE::SELF;
	reg2  = IO_NONE::SELF;
	memset(RAMBaseHost, 0, 4098);
}
// TODO
#include "rom/data.hpp"
using ROM::operator"" _mb;
std::unique_ptr<Machine> machine;
// 4-way SIMM
const SIMM simm30[] = {
	{ 1_mb, 1, 0 },
	{ 4_mb, 3, 1 },
	{ 16_mb, 5, 2 },
	{ 64_mb, 7, 3 }
};
// 2-way SIMM
const SIMM simm30_l[] = {
	{ 2_mb, 2, 0 },
	{ 4_mb, 3, 1 },
	{ 8_mb, 4, 2 },
};

const SIMM simm64[] = {
	{ 4_mb, 3, 0 },
	{ 16_mb, 5, 1 },
	{ 64_mb, 7, 2 }
};

const SIMM simm72[] = {
	{ 1_mb, 1, 0 },
	{ 2_mb, 2, 1 },
	{ 4_mb, 3, 2 },
	{ 8_mb, 4, 3 },
	{ 16_mb, 5, 4 },
	{ 32_mb, 6, 5 },
	{ 64_mb, 7, 6 },
	{ 128_mb, 8, 7 },
};

void* Machine::get_ram_addr(uint32_t addr, int attr) {
	return &RAMBaseHost[addr];
}
