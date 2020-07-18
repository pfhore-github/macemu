#pragma once
#include <memory>
#include <bitset>
#include <optional>
#ifdef __linux__
#define ATTR_WEAK __attribute__((weak))
#else
#define ATTR_WEAK
#endif
class ASC;
// rom routine header
extern uint32_t rom_base;
class VIA;
class IO_BASE;
class SCC_IOP;
class SCC;
namespace ROM {
class EASC_param;
class ASC_param;
void run_rom(uint32_t addr) ATTR_WEAK;
// direct call
void boot() ATTR_WEAK;					// $0008C-
void goto_boot2() ATTR_WEAK;				// $008E0

// $02XXX routines
void init_hw() ATTR_WEAK;					// $02E00
void reset_via() ATTR_WEAK;				// $02E8C
void getHWInfo(uint8_t gestalt_id) ATTR_WEAK;	// $02F64
void update_rom_flag() ATTR_WEAK;				// $0309C
bool check_glu() ATTR_WEAK;				// $030AC
bool check_mdu() ATTR_WEAK;				// $030CA
bool check_oss() ATTR_WEAK;				// $0310A
bool check_V8() ATTR_WEAK;				// $03166
bool check_mcu() ATTR_WEAK;				// $031A0
bool check_jaws() ATTR_WEAK;				// $031AE
bool check_niagara() ATTR_WEAK;			// $031F2
bool check_null() ATTR_WEAK;				// $0323A
// $04XXX routines
void init_mmu() ATTR_WEAK;				   // $04052
bool has_fpu() ATTR_WEAK;              	   // $04640
bool test_iifx_exp1(uint32_t base) ATTR_WEAK;			   // $04672
bool has_via(uint32_t base, uint32_t offset = 0) ATTR_WEAK; // $046AA($046AC)
bool has_rbv(uint32_t base) ATTR_WEAK;					  // $046D8
bool test_scsi_io(uint32_t base) ATTR_WEAK;				  // $04700
bool test_scsi_cmd(uint32_t base) ATTR_WEAK;				  // $04708
bool test_iop(uint32_t base) ATTR_WEAK;	   // $0477C
bool has_ether(uint32_t base) ATTR_WEAK;	   // $047B0
bool check_LMG1(uint32_t base) ATTR_WEAK;	   // $047BA
uint32_t read_id_from_via() ATTR_WEAK;		   // $047CC
void init_hw3() ATTR_WEAK;				   // $04866
void init_hw2(uint32_t offset) ATTR_WEAK;	   // $04A5E
void test_scsi() ATTR_WEAK;				   // $04A96
uint32_t remap_rom(uint32_t b) ATTR_WEAK;	   // $04AFE
void check_flag() ATTR_WEAK;				   // $04B0C
// use X internally
bool test_model_id() ATTR_WEAK;			   // $04B6C-$04BDC
void halt() ATTR_WEAK;					   // $04BE2
std::optional<uint16_t> get_model_id() ATTR_WEAK;	// $04BE4-$04C04

void play_asc_0() ATTR_WEAK;				// $7052
void play_asc_2() ATTR_WEAK;	// $07058
void play_asc_3() ATTR_WEAK;	// $0705E
void play_asc_5() ATTR_WEAK;	// $0706A
void play_asc(const ASC_param& p) ATTR_WEAK;				// $0706E-


void play_chime_1() ATTR_WEAK;			// $45C1C-$45C2A
void play_chime_4() ATTR_WEAK;			// $45C52-$45C60
void play_easc(int m) ATTR_WEAK;		  // $45C64-$45E3A
void wait_until_easc_fifo_half() ATTR_WEAK; // $45E3C-$45E46
void easc_beep(uint16_t r2_h, uint16_t r2_l,
			   uint16_t r4_h, uint16_t r4_l, int16_t volume) ATTR_WEAK; // $45EA2-$45F3A
bool ex_handler(int vec) ATTR_WEAK;
void boot_second() ATTR_WEAK;				// $4727A
void getHWInfo2(uint8_t gestaltId) ATTR_WEAK;				// $47752-$4775C
bool send_to_pb_key() ATTR_WEAK;							// $47826-$47840
bool send_to_pb_dword(uint32_t data) ATTR_WEAK; // $47840-$478A2
bool send_to_pb_byte(uint8_t data,bool* is_msc = nullptr) ATTR_WEAK;	// $478C2-$47926
std::optional<uint8_t> recv_from_pb(bool* is_msc = nullptr) ATTR_WEAK;	// $47928-$479B6
void check_checksum() ATTR_WEAK;			// $47C40-$47C6A
void _45EA2() ATTR_WEAK;
uint16_t init_scc_iop_fw() ATTR_WEAK; // $47EF6-$47F30
void _4B53C() ATTR_WEAK;
void sad_mac( uint32_t d6) ATTR_WEAK; // $4AA98
void init_powerbook() ATTR_WEAK;					// $8AC00-
void init_adb() ATTR_WEAK;				// $85714-
void boot2() ATTR_WEAK;					// $8AC00-
bool ram_check() ATTR_WEAK;				// $A0000-
struct check_simm_size_t {
	uint32_t size;
	uint32_t end;
};
struct ram_result {
	uint32_t sum_size;
	uint32_t mem_size;
	bool ok;
	operator bool() { return ok; }
};
struct RAM_SLOT_T {
	uint32_t unit;
	std::vector<std::pair<uint32_t, uint32_t>> ranges;
};

ram_result check_simm_slots(const RAM_SLOT_T* ram_slots) ATTR_WEAK; // $A01C6
check_simm_size_t check_simm_size(uint32_t base, uint32_t end, uint32_t data, uint32_t unit) ATTR_WEAK; // $A0242
std::bitset<4> check_simm(uint32_t base, uint32_t check) ATTR_WEAK; // $A0286
uint32_t get_memory_size(uint16_t sc, uint32_t value) ATTR_WEAK; // $A06E6
void _AA15C() ATTR_WEAK;					// $AA15C-; talk_adb
void _AA182() ATTR_WEAK;					// $AA182-$AA192; read_adb1
void skip_sr() ATTR_WEAK;					// $A1194-$AA1A0
void init_pb_duo() ATTR_WEAK;				// $B29C4
void comm_modem_port(uint32_t d6) ATTR_WEAK;					 // $B98BC
uint32_t modem_port_send1(uint32_t retry, uint32_t c1) ATTR_WEAK; // $B9A4E-
void _B997C() ATTR_WEAK;
struct RW_offset {
	uint32_t via_offset;
	uint32_t scc_offset;
};
RW_offset get_scc_addr() ATTR_WEAK; // $B9F46-$B9F76
void init_modem_port() ATTR_WEAK;			// $B9F78-$B9FBE
void write_modem_port_byte( uint8_t val) ATTR_WEAK; // $B9FDA-$B9FF4
void write_modem_port_cstr(const char* c) ATTR_WEAK; // $B9FF6-
void reset_VIA1_timer2() ATTR_WEAK; // $BA090
std::pair<int8_t, uint8_t> read_modem_port( ) ATTR_WEAK; // $BA0EA-BA118
int8_t from_ascii(int8_t c) ATTR_WEAK;	// $BA11A-$BA132
uint32_t read_modem_cmd_dword( int size) ATTR_WEAK;			 // $BA138-$BA172
uint8_t to_ascii(uint8_t c) ATTR_WEAK;	// $BA174-$BA180
void write_modem_integer( uint32_t value, int16_t len) ATTR_WEAK; // $BA182-$BA1E4
void read_modem_cmd1( char* dst, uint32_t count) ATTR_WEAK; // $BA964-$BA9CC
int check_easc_fifo_empty() ATTR_WEAK; // $BD3EC-$BD418
int check_easc_fifo_half() ATTR_WEAK;	 // $BD41E-$BD434
bool easc_is_sonora() ATTR_WEAK;		// $BD43A-$BD48E
int check_easc_fifo_done() ATTR_WEAK;	// $BD494-$BD4BE
void _B358C() ATTR_WEAK;
#undef ATTR_WEAK
}
// indirect call
namespace ROMWrapper {
#include "prototype2.hpp"
}
