#include <stdint.h>
#include "registers.hpp"
#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
// GLOBAL REGISTER
std::bitset<32> INIT_HW_FLG; // D0
std::bitset<32> ROM_FLG; // D1
std::bitset<16> HWCFG_FLAGS; // D2/H
uint8_t GID, GEN;			// D2/L
std::bitset<16> INIT_FLAGS; // D7.H
uint16_t EXCEPTION_STATUS;		// D7.L

const MOTHERBOARD_TABLE_T* motherboard = nullptr; // A0
const MACHINE_TABLE_T* model = nullptr;			  // A1
const std::vector<RAM_SLOT_T> ram_tables = {
	// GLU/MDU/OSS; $03BA4
	{ 1_mb, { { 0, 64_mb }, { 64_mb, 128_mb } }	},
	// V8; $03BBC
	{ 1_mb, { { 0, 8_mb }, { 8_mb, 10_mb } } },
	// V8-2; $03BD4
	{ 1_mb, { {0, 4_mb }, {4_mb, 8_mb } } },
	// Quadra 9x0; $03BEC
	{ 4_mb, { {0, 64_mb}, {64_mb, 128_mb }, {128_mb, 192_mb }, {192_mb, 256_mb } } },
	// Quadra 700; $03C14
	{ 4_mb, { {0, 64_mb}, {64_mb, 128_mb } } },
	// Jaws; $03C2C
	{ 512_kb, { { 0, 2_mb }, { 2_mb, 4_mb}, { 4_mb, 6_mb }, { 6_mb, 8_mb } } },
	// MSC; $A85E8
	{ 2_mb, { { 0, 2_mb}, { 2_mb, 4_mb}, { 4_mb, 6_mb}, { 6_mb, 8_mb },
			 { 8_mb, 16_mb}, { 16_mb, 24_mb}, { 24_mb, 32_mb }, { 32_mb, 40_mb } } },
	// Sonora/Ardbeg; $A8630
	{ 1_mb, { { 0, 16_mb }, { 16_mb, 32_mb}, { 32_mb, 48_mb }, { 48_mb, 64_mb },
			 { 64_mb, 80_mb } } },
	// Niagara; $A8660
	{ 512_kb,  { { 0, 2_mb}, { 2_mb, 4_mb}, { 4_mb, 6_mb}, { 6_mb, 8_mb },
			 { 8_mb, 10_mb}, { 10_mb, 12_mb}, { 12_mb, 14_mb }, { 14_mb, 16_mb } } },
	// MEMCjr/F108; $A8708
	{ 1_mb, { { 0, 64_mb}, { 64_mb, 128_mb }, { 128_mb, 192_mb }, { 192_mb, 256_mb },
			 { 256_mb, 320_mb } } },
	// from Q650 ROM
	// djMEMC; $D3034
	{ 4_mb, { {0, 64_mb}, {64_mb, 128_mb }, {128_mb, 192_mb }, {192_mb, 256_mb },
			  { 256_mb, 320_mb}, { 320_mb, 384_mb}, { 384_mb, 448_mb}, { 448_mb, 512_mb } } },
};
// $032D4-, $A8234
const std::vector<MOTHERBOARD_TABLE_T> motherboards = {
	// GLUE
	{
		0x32D4,
		0x1F3F, 0, 0x10, 0x00, 0xC0, 0x00, check_glu, 4,
		0x40800000, 0x58000000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0x50F10000, 0x50F12000, 0x50F06000, 0x50F02000, 0x50F14000, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// MDU
	{
		0x3378,
		0x3FF3F, 0, 0x00, 0x00, 0x00, 0x00, check_mdu, 5,
		0x40800000, 0x58000000, 0x50F00000, 0, 0, 0, 0, 0,
		0x50F10000, 0x50F12000, 0x50F06000, 0x50F02000, 0x50F14000, 0x50F26000, 0x50F24000, 0x50F18000,
		0x50F1E020, 0x50F0C020, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// OSS
	{
		0x341C,
		0x001F9007, 0x4, 0x00, 0x00, 0x00, 0x00, check_oss, 6,
		0x40800000, 0x58000000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0, 0, 0, 0, 0x50F10000, 0, 0, 0x50F08000,
		0x50F12020, 0x50F04020, 0x50F1A000, 0x50F1C000, 0x50F1E000, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// V8
	{
		0x34C0,
		0x1000773F, 0xA6, 0x00, 0x00, 0x00, 0x00, check_V8, 7,
		0x40A00000, 0x50F80000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0x50F10000, 0x50F12000, 0x50F06000, 0, 0x50F14000, 0x50F26000, 0x50F24000, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0x00A00000, 0, 0, 0,
		0, 0, 0, 0,
	},
	// MCU
	{
		0x3564,
		0x07A3181F, 0, 0x00, 0xC0, 0x00, 0x00, check_mcu, 8,
		0x40800000, 0x58000000, 0x50F00000, 0x50F0C020, 0x50F0C020, 0x50F1E000, 0, 0,
		0, 0, 0, 0x50F02000, 0x50F14000, 0, 0xF9800000, 0,
		0x50F1E020, 0x50F0C020, 0, 0, 0, 0x5000E000, 0, 0x50F0A000, 
		0x50F0F000, 0x50F0F402, 0xF9800000, 0, 0, 0x74, 0, 0,
		0, 0, 0, 0,
	},
	// JAWS
	{
		0x3608,
		0x00401F3F, 0x00010002, 0x00, 0x00, 0x00, 0x00, check_jaws, 9,
		0x40800000, 0x58000000, 0x50F00000, 0x50F0C020, 0x50F0C020, 0x50F16000, 0, 0,
		0x50F10000, 0x50F06000, 0x50F06000, 0x50F02000, 0x50F14000, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0x50F80000, 0, 
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// MSC
	{
		0xA8234,
		0x773F, 0x00130112, 0x00, 0x00, 0x00, 0x00, nullptr, 10,
		0x40800000, 0x58000000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0x50F10000, 0x50F12000, 0x50F06000, 0, 0x50F14000, 0x50F26000, 0x50F20000, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// Sonora
	{
		0xA82D8,
		0x2080773F, 0x00402926, 0x00, 0x00, 0x00, 0x00, nullptr, 11,
		0x40800000, 0x50FC0000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0x50F10000, 0x50F12000, 0x50F06000, 0, 0x50F14000, 0x50F26000, 0x50F24000, 0,
		0, 0, 0, 0, 0, 0, 0, 0x50F0A000,
		0, 0, 0, 0, 0, 0x74, 0, 0,
		0, 0, 0, 0,
	},
	// Niagara
	{
		0xA837C,
		0x00401F3F, 0x00212002, 0x00, 0x00, 0x00, 0x00, check_niagara, 12,
		0x40800000, 0x58000000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0x50F10000, 0x50F06000, 0x50F06000, 0x50F02000, 0x50F14000, 0, 0x50F20000, 0,
		0, 0, 0, 0, 0, 0, 0x50F80000, 0, 
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// MemcJr
	{
		0xA8420,
		0x0580181F, 0x00002000, 0x00, 0xC0, 0x00, 0x00, nullptr, 13,
		0x40800000, 0x58000000, 0x50F00000, 0x50F0C020, 0x50F0C020, 0x50F1E000, 0, 0,
		0, 0, 0, 0x50F02000, 0x50F14000, 0, 0xF9800000, 0,
		0, 0, 0, 0, 0, 0, 0, 0x50F0A000,
		0x50F10000, 0, 0xF9800000, 0, 0, 0x74, 0, 0,
		0, 0, 0, 0,
	},
	// F108
	{
		0xA84C4,
		0x4180181F, 0x00002000, 0x00, 0xC0, 0x00, 0x00, nullptr, 13,
		0x40800000, 0x58000000, 0x50F00000, 0x50F0C020, 0x50F0C020, 0x50F1E000, 0, 0,
		0, 0, 0, 0x50F02000, 0x50F14000, 0, 0x50F24000, 0,
		0, 0, 0, 0, 0, 0, 0, 0x50F0A000,
		0x50F10000, 0, 0, 0x50F1A000, 0, 0x74, 0, 0,
		0, 0, 0, 0,
	},
	// Ardbeg
	{
		0xA8568,
		0x0080773F, 0x18406126, 0x00, 0xC0, 0x00, 0x00, nullptr, 11,
		0x40800000, 0x50FC0000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000, 0, 0,
		0x50F10000, 0x50F12000, 0x50F06000, 0, 0x50F14000, 0x50F26000, 0x50F24000, 0,
		0, 0, 0, 0, 0, 0, 0, 0x50F0A000,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// Unknown
	{
		0xA8892,
		0, 0, 0x00, 0x00, 0x00, 0x00, check_null, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
	// MemcJr2
	{
		0xD3034,
		0x0580181F, 0x00002000, 0x00, 0xC0, 0x00, 0x00, nullptr, 13,
		0x40800000, 0x58000000, 0x50F00000, 0x50F0C020, 0x50F0C020, 0x50F1E000, 0, 0,
		0, 0, 0, 0x50F02000, 0x50F14000, 0, 0xF9800000, 0,
		0, 0, 0, 0, 0, 0, 0, 0x50F0A000,
		0x50F10000, 0, 0xF9800000, 0, 0, 0x74, 0, 0,
		0, 0, 0, 0,
	},
};
const VIA_DATA_T via_data[] = {
	// GLUE VIA1
	{ 0x01,	0x3F,	0x37,	0xB7,	0x00,	0x00, },
	// MDU VIA1
	{ 0x21,	0x2F,	0xF7,	0xF7,	0x00,	0x00, },
	// Egret VIA1
	{ 0x26,	0x2F,	0x48,	0x30,	0x00,	0x1C, },
	// OSS IOP VIA1
	{ 0x01,	0x07,	0x87,	0x87,	0x00,	0x00, },
	// MCU IOP VIA1
	{ 0x00,	0x00,	0xC9,	0xF1,	0x22,	0x1C, },
	// MCU VIA1
	{ 0x20,	0x28,	0xF7,	0xF7,	0x22,	0x00, },
	// JAWS VIA1
	{ 0x21,	0x28,	0x87,	0x87,	0x22,	0x00, },
	// GLUE VIA2
	{ 0x00,	0xC0,	0x07,	0x80,	0x60,	0xC0, },
	// MDU VIA2
	{ 0x00,	0xC0,	0x0F,	0x89,	0x00,	0xC0, },
	// MCU IOP VIA2
	{ 0x80,	0x80,	0x03,	0xC8,	0x22,	0xC0, },
	// MCU VIA2
	{ 0x80,	0x80,	0x06,	0x99,	0x22,	0xC0, },
	// JAWS VIA2
	{ 0x00,	0x00,	0x66,	0xBD,	0x22,	0x00, },
	// Niagara/MSC VIA1
	{ 0x00,	0x00,	0x00,	0x00,	0x02,	0x1C, },
	// Egret/Cuda VIA1	
	{ 0x20,	0x28,	0x88,	0xB0,	0x20,	0x1C, },
	// Ardbeg VIA1
	{ 0x20,	0x28,	0xB8,	0xB0,	0x00,	0x1C, },
	// PrimeTime VIA1
	{ 0x20,	0x28,	0xF8,	0xF0,	0x00,	0x1C, },
	// PrimeTime VIA2
	{ 0x00,	0x00,	0x0E,	0xFF,	0x26,	0xC0, },	
	// from Q650 ROM
	// IOSB VIA1
	{ 0x20, 0x28, 0xF7, 0xF7, 0x22, 0x1C }, 
	// IOSB VIA2
	{ 0x00,	0x00,	0x06,	0x99,	0x22,	0xC0, },	

};
uint16_t _B358C(uint16_t);
uint16_t _B375E(uint16_t);
uint16_t _B381E(uint16_t);
uint16_t _B3AB8(uint16_t);
uint16_t rom650_B3566(uint16_t);
const std::vector<MODEL_UNKOWN52> model_unknown52 = {
	{ 0xB33D2, {0x1EE, 0x214, 0x20C}, _B358C, {0x1D6, 0x096, 0x19C, 0x10E, 0x142}, 9 }, // old machine ,
	{ 0xB33F8, {0x000, 0x000, 0x000}, _B375E, {0x2F0, 0x070, 0x2B4, 0x286, 0x29C}, 9 }, // MSC
	{ 0xB341E, {0x000, 0x000, 0x000}, _B381E, {0x3F6, 0x04A, 0x3CE, 0x3AC, 0x3B6}, 9 }, // LC/IIsi/Q9x0
	{ 0xB3444, {0x000, 0x000, 0x000}, _B3AB8, {0x584, 0x024, 0x3A8, 0x386, 0x390}, 0x48E7  }, // LC5xx/LC6xx
	{ 0xB33D2, {0x1C8, 0x1EE, 0x1E6}, rom650_B3566, {0x1B0, 0x070, 0x176, 0x0E8, 0x11C}, 9 }, // Q6x0 
	
};
// $03260-, $03688-
const MACHINE_TABLE_T old_machines[] = {
	{ // IIx
		0x03688,
		&motherboards[0], 0, 0, 0x3D34, 0xDC00, 0x01, 4, 0x3FFF, 1, 0, 
		0x1F3F, 0x800, 0x40000008, 0,
		&via_data[0], &via_data[7], 0, &model_unknown52[0], 0xB2BD2, {}, 0, 0x0000
	},
	{ // SE/30
		0x036D0,
		&motherboards[0], 0, 0, 0x3D44, 0xDC00, 0x03, 4, 0x3FFF, 1, 0, 
		0x1F3F, 0x1800, 0x40000008, 0x40000000,
		&via_data[0], &via_data[7], 0, &model_unknown52[0], 0xB2BD2,  {}, 0, 0x0000
	},
	{ // IIcx
		0x03718,
		&motherboards[0], 0, 0, 0x3D54, 0xDC00, 0x02, 4, 0x3FFF, 1, 0, 
		0x1F3F, 0x0800, 0x40000008, 0x40000008,
		&via_data[0], &via_data[7], 0, &model_unknown52[0], 0xB2BD2,  {}, 0, 0x0000
	},
	{ // IIci
		0x03760,
		&motherboards[1], 0, 0x3C54, 0x3D64, 0xDC00, 0x05, 5, 0x3FFF, 1,  0, 
		0x773F, 0x0800, 0x56000000, 0x46000000,
		&via_data[1], nullptr, 0, &model_unknown52[0], 0xB2BD2,  {}, 0, 0x0000
	},
	{ // IIci with PGC
		0x037A8,
		&motherboards[1], 0, 0x3C54, 0x3D64, 0xDC00, 0x05, 5, 0x3FFF, 1, 0, 
		0x773F, 0x0801, 0x56000000, 0x56000000,
		&via_data[1], nullptr, 0, &model_unknown52[0], 0xB2BD2,  {}, 0, 0x0000
	},
	{ // IIfx
		0x037F0,
		&motherboards[2], 0, 0, 0x3D34, 0xDC00, 0x07, 6, 0x3FFF, 2, 0, 
		0, 0x0804, 0x56000000, 0x52000000,
		&via_data[3], nullptr, 0, &model_unknown52[0], 0xB2C20,  {}, 0, 0x0000
	},
	{ // IIsi
		0x03838,
		&motherboards[1], 0, 0x3C66, 0x3D84, 0xDC00, 0x0C, 5, 0x3FFF, 4,  0, 
		0x773F, 0x3926, 0x56000000, 0x16000000,
		&via_data[2], nullptr, 0, &model_unknown52[2], 0xB2C06,  {}, 0, 0x0000
	},
	{ // LC
		0x03880,
		&motherboards[3], 1, 0x3C78, 0x3D94, 0xDC00, 0x0D, 7, 0x3FFF, 4, 0, 
		0x773F, 0x01A6, 0x56000000, 0x54000000,
		&via_data[2], nullptr, 0x8CBAC, &model_unknown52[2], 0xB2C06,  {}, 0, 0x0000
	},
	{ // Quadra 700
		0x03958,
		&motherboards[4], 4, 0x3CA8, 0x3DB4, 0xDC00, 0x10, 8, 0x3FFF, 2, 0, 
		0x05A0183F, 0x3900,	0x56000000, 0x40000000,
		&via_data[5], &via_data[10], 0x8CB7E, &model_unknown52[0], 0xB2BD2,  {}, 0, 0x0000
	},
	{ // Quadra 900
		0x038C8,		
		&motherboards[4], 3, 0x3CA8, 0x3DA4, 0xDC00, 0x0E, 8, 0x3FFF, 2, 0, 
		0x27A31807, 0x43924, 0x56000000, 0x50000000,
		&via_data[4], &via_data[9], 0x8CB50, &model_unknown52[2], 0xB2C3A,  {}, 0, 0x0000
	},
	{ // Quadra 950
		0x03910,
		&motherboards[4], 3, 0x3CA8, 0x3DA4, 0xDC00, 0x14, 8, 0x3FFF, 2, 0, 
		0x07A31807, 0x43924, 0x56000000, 0x10000000,
		&via_data[4], &via_data[9], 0x8CB50, &model_unknown52[2], 0xB2C3A,  {}, 0, 0x0000
	},
	{ // PowerBook 170
		0x039A0,
		&motherboards[5], 5, 0x3C96, 0x3DC4, 0xDD00, 0x0F, 9, 0x3FFF, 4, 0, 
		0x00401F3F, 0x13902, 0x56000000, 0x12000000,
		&via_data[6], &via_data[11], 0xBCB22, &model_unknown52[0], 0xB2BEC, 0x70, 0, 0x0000
	},
	{ // PowerBook 180
		0xA8180,
		&motherboards[8], 8, 0xA876E, 0xA78F4, 0xDD00, 0x1B, 12, 0x3FFF, 4, 0, 
		0x00405F3F, 0x213902, 0x56000000, 0x12000000,
		&via_data[6], &via_data[11], 0xBCC36, &model_unknown52[0], 0xB2BEC, 0x46, 0, 0x0000
	},
	{ // Genaric GLUE
		0x039E8,
		&motherboards[0], 0, 0, 0x03D34, 0xDC00, 0xFD, 4, 0x3FFF, 1, 0, 
		0, 0, 0, 0,
		&via_data[0], &via_data[7], 0, &model_unknown52[0], 0xB2BD2, {}, 0, 0x0000
	},
	{ // Genaric MDU
		0x03A30,
		&motherboards[1], 0, 0x3C54, 0x03D34, 0xDC00, 0xFD, 5, 0x3FFF, 1, 0, 
		0, 0, 0, 0,
		&via_data[1], &via_data[8], 0, &model_unknown52[0], 0xB2BD2, {}, 0, 0x0000
	},
	{ // Genaric OSS
		0x03A78,
		&motherboards[2], 0, 0, 0x03D34, 0xDC00, 0xFD, 6, 0x3FFF, 2, 0, 
		0, 0, 0, 0,
		&via_data[3], nullptr, 0, &model_unknown52[0], 0xB2C20, {}, 0, 0x0000
	},
	{ // Genaric V8
		0x03AC0,
		&motherboards[3], 1, 0x3C78, 0x03D94, 0xCC00, 0xFD, 7, 0x7FFF, 4, 0, 
		0, 0, 0, 0,
		&via_data[2], nullptr, 0, &model_unknown52[2], 0xB2C06, {}, 0, 0x0000
	},
	{ // Genaric Jaws
		0x03B08,
		&motherboards[5], 5, 0x3C96, 0x03DC4, 0xDD00, 0xFD, 9, 0x3FFF, 2, 0, 
		0, 0x00010002, 0, 0,
		&via_data[6], &via_data[11], 0, &model_unknown52[0], 0xB2BEC, 0x70, 0, 0x0000
	},
	{ // Unknown
		0xA8912,
		&motherboards[12], 0, 0, 0, 0, 0xFD, 0, 0xFFFF, 1, 0, 
		0, 0, 0, 0,
		nullptr, nullptr, 0, &model_unknown52[0], 0, {}, 0, 0x0000
	},
};
const std::vector<MACHINE_TABLE_T> new_machines = {
	{
		// PowerBook Duo 210
		0xA7A30,
		&motherboards[6], 6, 0xA8738, 0xA87A4, 0xDD00, 0x17, 10, 0x3FFF, 4, 0, 
		0x773F, 0x00130112, 0x56000000, 0,
		&via_data[12], nullptr, 0x8C8BA, &model_unknown52[1], 0xB2BEC, 0x46, 0, 0x1004
	},
	{
		// PowerBook Duo 230
		0xA7A78,
		&motherboards[6], 6, 0xA8738, 0xA87A4, 0xDD00, 0x1A, 10, 0x3FFF, 4, 0, 
		0x773F, 0x00130112, 0x56000000, 0,
		&via_data[12], nullptr, 0x8C8BA, &model_unknown52[1], 0xB2BEC, 0x46, 0, 0x1005
	},
	{
		// PowerBook Duo 270c
		0xA7AC0,
		&motherboards[6], 6, 0xA8738, 0xA87A4, 0xDD00, 0x47, 10, 0x3FFF, 4, 0, 
		0x773F, 0x00130112, 0x56000000, 0,
		&via_data[12], nullptr, 0x8C8BA, &model_unknown52[1], 0xB2BEC, 0x46, 0, 0x1002
	},
	{
		// unused
		0xA7B08,
		&motherboards[7], 7, 0xA874A, 0xA87E4, 0xDC00, 0x22, 11, 0x3FFF, 4, 0,
		0x2000773F, 0x20402126, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0
	},
	{
		// LCIII
		0xA7B50,
		&motherboards[7], 7, 0xA874A, 0xA87E4, 0xDC00, 0x15, 11, 0x3FFF, 4, 0,
		0x2000773F, 0x20400126, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0x1
	},
	{
		// LCIII+
		0xA7B98,
		&motherboards[7], 7, 0xA874A, 0xA87E4, 0xDC00, 0x38, 11, 0x3FFF, 4, 0,
		0x2080773F, 0x20400126, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0x3
	},
	{
		// Unused
		0xA7BE0,
		&motherboards[7], 7, 0xA875C, 0xA87B4, 0xDC00, 0x16, 11, 0x3FFF, 4, 0,
		0x2080773F, 0x00403926, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0x2000
	},
	{
		// unused
		0xA7C28,
		&motherboards[7], 7, 0xA875C, 0xA87B4, 0xDC00, 0x24, 11, 0x3FFF, 4, 0,
		0x2080773F, 0x00403926, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0x2001
	},
	{
		// unused
		0xA7C70,
		&motherboards[7], 7, 0xA875C, 0xA87B4, 0xDC00, 0x23, 11, 0x3FFF, 4, 0,
		0x2080773F, 0x00403926, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0x2002
	},
	{
		// unused
		0xA7CB8,
		&motherboards[7], 7, 0xA875C, 0xA87B4, 0xDC00, 0x16, 11, 0x3FFF, 4, 0, 
		0x2080773F, 0x00403926, 0, 0,
		&via_data[13], nullptr, 0x8CC64, &model_unknown52[2], 0xB2C06, {}, 0, 0x2003
	},
	{
		// Quadra 605(20MHz)
		0xA7D90,
		&motherboards[9], 9, 0xA8792, 0xA8814, 0xDC00, 0x57, 13, 0x3FFF, 4, 0,
		0x0500183F, 0x23804926, 0, 0,
		&via_data[15], &via_data[16], 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x2224
	},
	{
		// Quadra 605(25MHz)
		0xA7DD8,
		&motherboards[9], 9, 0xA8792, 0xA8814, 0xDC00, 0x58, 13, 0x3FFF, 4, 0,
		0x0500183F, 0x23804926, 0, 0,
		&via_data[15], &via_data[16], 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x2225
	},
	{
		// Quadra 605(30MHz)
		0xA7E20,
		&motherboards[9], 9, 0xA8792, 0xA8814, 0xDC00, 0x59, 13, 0x3FFF, 4, 0,
		0x0500183F, 0x23804926, 0, 0,
		&via_data[15], &via_data[16], 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x2226
	},
	{
		// LC520
		0xA7D00,
		&motherboards[11], 7, 0xA874A, 0xA8804, 0xDC00, 0x32, 11, 0x3FFF, 4, 0,
		0x0000773F, 0x1B404926, 0, 0,
		&via_data[14], nullptr, 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x0100
	},
	{
		// LC550
		0xA7D48,
		&motherboards[11], 7, 0xA874A, 0xA8804, 0xDC00, 0x4A, 11, 0x3FFF, 4, 0,
		0x0000773F, 0x1B404926, 0, 0,
		&via_data[14], nullptr, 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x0101
	},
	{
		// unused
		0xA7E68,
		&motherboards[9], 9, 0x03CA8, 0xA87D4, 0xDC00, 0x5A, 13, 0x3FFF, 4, 0,
		0x0580181F, 0x23804926, 0, 0,
		&via_data[15], &via_data[16], 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x2231
	},
	{
		// unused
		0xA7EB0,
		&motherboards[9], 9, 0x03CA8, 0xA87D4, 0xDC00, 0x5B, 13, 0x3FFF, 4, 0, 
		0x0580181F, 0x23804926, 0, 0,
		&via_data[15], &via_data[16], 0x8CC92, &model_unknown52[3], 0xB2C06, {}, 0, 0x2232
	},
	{
		// LC630 #1
		0xA7EF8,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5C, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2252
	},
	{
		// LC630 #2
		0xA7F40,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5C, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2253
	},
	{
		// LC630 #3
		0xA7F88,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5C, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2250
	},
	{
		// LC630 #4
		0xA7FD0,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5C, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2256
	},
	{
		// LC630 #5
		0xA8018,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5C, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2257
	},
	{
		// LC630 #6
		0xA8060,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5C, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2254
	},
	{
		// LC580 #1
		0xA80A8,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5D, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x225A
	},
	{
		// LC580 #2
		0xA80F0,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5D, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x225B
	},
	{
		// LC580 #3
		0xA8138,
		&motherboards[10], 9, 0x03D22, 0xA8834, 0xDC80, 0x5D, 13, 0x3FFF, 4, 1,
		0x4180183F, 0x4B807926, 0, 0,
		&via_data[15], &via_data[16], 0xA47C2, &model_unknown52[3], 0xB2C06, {}, 0xA71C0, 0x2258
	},
	// from Q650 ROM

	{
		// Unknown Centris/Quadra
		0xD2B24,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x34, 13, 0x3FFF, 4, 0,
		0x2580183F, 0x803900, 0x56000000, 0x42000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Centris 650
		0xD2B64,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x18, 13, 0x3FFF, 4, 0,
		0x2580183F, 0x803900, 0x56000000, 0x46000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Quadra 800
		0xD2BE4,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x1D, 13, 0x3FFF, 2, 0,
		0x2580183F, 0x803900, 0x56000000, 0x12000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Quadra 650
		0xD2BA4,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x1E, 13, 0x3FFF, 2, 0,
		0x2580183F, 0x803900, 0x56000000, 0x52000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Unknown Centris/Quadra
		0xD2C24,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x2D, 13, 0x3FFF, 2, 0,
		0x2580183F, 0x803900, 0x56000000, 0x56000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Unknown Centris/Quadra
		0xD2C64,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x35, 13, 0x3FFF, 2, 0,
		0x2580183F, 0x803900, 0x56000000, 0x16000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Centris 610
		0xD2CA4,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x2E, 13, 0x3FFF, 4, 0,
		0x2580183F, 0x20803900, 0x56000000, 0x40000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Quadra 610
		0xD2CE4,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x2F, 13, 0x3FFF, 4, 0,
		0x2580183F, 0x20803900, 0x56000000, 0x44000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Unknown Centris/Quadra
		0xD2D24,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x33, 13, 0x3FFF, 4, 0,
		0x2580183F, 0x20803900, 0x56000000, 0x04000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// Unknown Centris/Quadra
		0xD2D64,
		&motherboards[13], 10, 0x03BEC, 0xD3234, 0xDC00, 0x39, 13, 0x3FFF, 4, 0,
		0x2580183F, 0x20803900, 0x56000000, 0x50000000,
		&via_data[17], &via_data[18], 0x8CAF2, &model_unknown52[4], 0xB2BD2, {}, 0, 0x2BAD
	},
	{
		// unknown
		0xA8912,
		&motherboards[12], 0, 0, 0, 0, 0xFD, 0, 0xFFFF, 1, 0,
		0, 0, 0, 0,
		nullptr, nullptr, 0, &model_unknown52[0], 0, {}, 0, 0
	},
};
const MOTHERBOARD_TABLE_T* hw_test_table[8] = {
	&motherboards[3],
	&motherboards[8],
	&motherboards[5],
	&motherboards[4],
	&motherboards[0],
	&motherboards[1],
	&motherboards[2],
	&motherboards[12],
};

void get_motherboard(uint32_t a0) {
	for( auto& m : motherboards ) {
		if( m.BASE == (a0&0xfffff) ) {
			motherboard = &m;
			return ;
		}
	}
	motherboard = nullptr;
}
void get_model(uint32_t a1) {
	for( auto& m : old_machines ) {
		if( m.BASE == (a1&0xfffff) ) {
			model = &m;
			return ;
		}
	}
	for( auto& m : new_machines ) {
		if( m.BASE == (a1&0xfffff) ) {
			model = &m;
			return ;
		}
	}
	model = nullptr;	
}
const uint32_t vbr_table[4] = {
	0x3DD8,					// overlay
	0x40803DD8,				// normal ROM
	0x40003DD8,				// flat ROM?
	0x00A03DD8,				// 24bit ROM?
};

void run_rom(uint32_t) {}
}
