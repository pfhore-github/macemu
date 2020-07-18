#pragma once
#include "machine.hpp"
#include <functional>
#include "mem.hpp"
#include <bitset>
#ifndef DATA_PATH
#define DATA_PATH "/home/michiaki/Document/macemu/macemu/BasiliskII/src/rom/data/"
#endif
constexpr uint32_t OSTYPE(const char s[]) {
	return uint8_t(s[0]) << 24 | uint8_t(s[1]) << 16 | uint8_t(s[2]) << 8 | uint8_t(s[3]);
}
extern uint8_t* ROMBaseHost;
extern uint32_t rom_base;
namespace ROM {
struct VIA_DATA_T {
	uint8_t regA;
	uint8_t dirA;
	uint8_t regB;
	uint8_t dirB;
	uint8_t pcr;
	uint8_t acr;
};
struct RAM_SLOT_T;
struct MODEL_UNKOWN52 {
	uint32_t BASE;
	uint32_t value[3];
	uint16_t (*handler4)(uint16_t d1);
	uint32_t value2[5];
	uint16_t value3;
};
struct MACHINE_TABLE_T {
	uint32_t BASE;
	const struct MOTHERBOARD_TABLE_T* motherboard;
	uint32_t ram_id;
	uint32_t unknown_8;
	uint32_t unknown_12;
	std::bitset<16> hwcfg;
	uint8_t gestalt;
	uint8_t generation;
	uint16_t unkown_20;
	uint8_t gen2;
	uint8_t xid;
	std::bitset<32> init_flg; 
	std::bitset<32> rom_flg;
	uint32_t model_mask;
	uint32_t model_id;
	const VIA_DATA_T* via_data1;
	const VIA_DATA_T* via_data2;
	uint32_t unknown_48;
	const MODEL_UNKOWN52* unknown_52;
	uint32_t unknown_56;
	std::optional<uint8_t> unknown_60;
	uint32_t unknown_64;
	uint16_t machine_id;
	uint16_t gestalt_machine_id() const { return gestalt << 8 | generation; }
};
struct MOTHERBOARD_TABLE_T {
	uint32_t BASE;
	std::bitset<32> init_flg; 
	std::bitset<32> rom_flg;
	uint8_t via1A_mask;
	uint8_t via1B_mask;
	uint8_t via2A_mask;
	uint8_t via2B_mask;
	bool (*check)();
	uint8_t generation;
	uint32_t rom_base;
	uint32_t secret;
	uint32_t via1_addr;
	uint32_t scc_read_addr;
	uint32_t scc_write_addr;
	uint32_t swim_addr;
	uint32_t unused_24;
	uint32_t unused_28;
	uint32_t scsi_addr;
	uint32_t scsi_pseudu_dma_addr;
	uint32_t scsi_handshake_addr;
	uint32_t via2_addr;
	uint32_t asc_addr;
	uint32_t rbv_addr;
	uint32_t vdac_addr;
	uint32_t scsi_dma_addr;
	uint32_t swim_iop_addr;
	uint32_t scc_iop_addr;
	uint32_t oss_addr;
	uint32_t iifx_exp0_addr;
	uint32_t iifx_exp1_addr;
	uint32_t mcu_addr;
	uint32_t pb_addr;
	uint32_t ether_addr;
	uint32_t quadra_scsi_int_addr;
	uint32_t quadra_scsi_ext_addr;
	uint32_t quadra_addr;
	uint32_t f108_addr;
	uint32_t v8addr;
	uint32_t unknown116;		
	uint32_t unknown120;		
	uint32_t unknown124;		
	uint32_t unknown128;		
	uint32_t unknown132;		
	uint32_t unknown136;		
	uint32_t unknown140;		
};
extern const MACHINE_TABLE_T old_machines[19];
extern const std::vector<MACHINE_TABLE_T> new_machines;
extern std::unordered_map<uint32_t, std::function<void()>> rom_routines;
namespace ROM_FLG_T {
enum ROM_FLG_T {
	IICI_PGC = 0,
	MSC = 17,
	MONITOR = 20,
	SONOR_F108 = 22,
	MEMCRjr_F108,
	F108_Ardbeg = 27,
};
}
inline int8_t read_rom8(uint32_t base) { return ROMBaseHost[base&0xfffff]; }
inline int16_t read_rom16(uint32_t base) {
	return ROMBaseHost[base&0xfffff] << 8 | ROMBaseHost[(base&0xfffff)+1];
}
inline int32_t read_rom32(uint32_t base) {
	return ROMBaseHost[base&0xfffff] << 24 |
		ROMBaseHost[(base&0xfffff)+1] << 16 |
		ROMBaseHost[(base&0xfffff)+2] << 8 |
		ROMBaseHost[(base&0xfffff)+3];
}

namespace INIT_HW_FLG_T {
enum {
	MASTER = 0,					// always true
	MASTER2,					// always true
	VIA1,						// always true
	NON_IIFX_1,						// only IIfx false
	NON_IIFX_2,						// only IIfx false
	NO_ISM,						// only IIfx false
	PSUEDO_DMA_1 = 8,			// not true DMA SCSI(1)
	PSUEDO_DMA_2,
	PSUEDO_DMA_3,
	VIA2,					//
	KNOWN_HW, 						// always true?
	RBV,
	VDAC,
	SCSI_DMA,
	IOP_SWIM,
	IOP_SCC,
	OSS,
	IIFX_EXP0,						// only IIfx true
	IIFX_EXP1,					// IIfx exp1 card
	MCU,						// MCU(early quadra) only
	POWERBOOK,						// Powerbook only
	ETHER,
	SCSI_040_INT,					// 680(LC)40 SCSI (internal/main)
	SCSI_040_EXT,					// 680(LC)40 SCSI (external)
	SPECIAL_LOWMEM = 29			// LOWMEM HAS "romp"
};
}
namespace HWCFG_T {
enum HWCFG_T {
	POWERBOOK = 24-16,
	HAS_FPU = 28-16		// (always true on ROM; disable in init routine)
};
}
namespace INIT_FLAG_T {
enum {
	MODEM_PORT_UNSEND = 16-16,
	MODEM_PORT_VALID = 17-16,
	MODEM_PORT_UNKNOWN = 18-16,
	MODEM_PORT_IN_CMD = 19-16,
	MODEM_PORT_ASCII = 20-16,
	MODEM_PORT_ECHO = 21-16,
	MODEM_PORT_WAIT = 22-16,
	MODEM_PORT_CRLF = 23-16,
	MODEM_PORT_ENABLE = 26-16,
	FAULT_SKIP = 27-16,
};
}
inline uint32_t calc_relative(uint32_t base, int32_t offset) {
	return offset ? base + offset : 0;
}

#define DR(x) cpu.R[x]
#define AR(x) cpu.R[8+(x)]







extern std::bitset<32> INIT_HW_FLG; // D0
extern std::bitset<32> ROM_FLG; // D1
extern std::bitset<16> HWCFG_FLAGS; // D2/H
extern uint8_t GID, GEN;			// D2/L
inline uint32_t GET_D2() { return HWCFG_FLAGS.to_ulong() << 16 | GID << 8 | GEN; }
inline void SET_D2(uint32_t v) {
	HWCFG_FLAGS = std::bitset<16>(v >> 16);
	GID = (v>>8)&0xff;
	GEN = v & 0xff;
}
extern std::bitset<16> INIT_FLAGS; // D7/H
extern uint16_t EXCEPTION_STATUS;	   // D7/L

extern const MOTHERBOARD_TABLE_T* hw_test_table[8];
// extern MOTHERBOARD motherboard; // A0
extern const MOTHERBOARD_TABLE_T* motherboard;
// extern MODEL model; // A1
extern const MACHINE_TABLE_T* model;
extern const std::vector<RAM_SLOT_T> ram_tables;
void get_motherboard(uint32_t a0);
void get_model(uint32_t a1);
// GLOBAL
enum OSS_T {
	OSS_IRQ_LEVEL = 0,
	OSS_IRQ_PENDING = 0x202,
	OSS_ROM_CTRL = 0x204,
	OSS_ACK_60HZ = 0x207
};
inline void SWAP(uint32_t& v) {
	v = v >> 16 | v << 16;
}

inline uint8_t BSET(uint32_t addr, uint8_t v) {
	uint8_t old_v = read_b(addr);
	write_b(addr, old_v | v);
	return old_v & v;
}

inline uint8_t BCLR(uint32_t addr, uint8_t v) {
	uint8_t old_v = read_b(addr);
	write_b(addr, old_v &~ v);
	return old_v & v;
}

inline void BMASK(uint32_t addr, uint8_t v) {
	uint8_t old_v = read_b(addr);
	write_b(addr, old_v & v);
}


void run_rom(uint32_t addr);
constexpr int TEST_MODEL_ID_SUCCESS = 0x2F82;
constexpr int TEST_MODEL_ID_FAILURE = 0x2F6A;
template<class T>
bool test_machine_register(const std::shared_ptr<T>& io, uint32_t addr) {
	if( ! addr ) {
		return false;
	}
	return io == machine->get_io( addr );
}
struct ASC_param {
	uint16_t volume;
	uint32_t wait;
	uint32_t delay;
	uint32_t repeat;
	uint16_t chs;
	uint32_t freq[4];
};
		
struct EASC_param {
	bool long_sound;
	uint16_t repeat1;
	uint16_t repeat2;
	uint16_t repeat3;
	uint16_t data1;
	uint16_t data2;
	uint16_t data3;
	uint16_t data4;
};
extern const std::vector<MOTHERBOARD_TABLE_T> motherboards;
extern const MACHINE_TABLE_T old_machines[19];
// $03DC8
extern const uint32_t vbr_table[4];

inline constexpr uint32_t operator"" _mb(unsigned long long v) { return v * 1024 * 1024; }  
inline constexpr uint32_t operator"" _kb(unsigned long long v) { return v * 1024 ; }  

}

