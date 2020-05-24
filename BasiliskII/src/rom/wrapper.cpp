#include <stdint.h>
#include "registers.hpp"
#include "prototype.hpp"
#include "data.hpp"
#include "wrapper.hpp"
namespace ROM {
extern MB_TYPE mb_type;
}
namespace ROMWrapper {
using namespace ROM;
}
void init_rom_routines() {
	DEFINE_ROM( 0008C );		// boot
	DEFINE_ROM( 02E00 );		// init_hw
	DEFINE_ROM( 02F64 );		// getHWInfo
	DEFINE_ROM( 030AC );		// check_glu
	DEFINE_ROM( 030CA );		// check_mdu
	DEFINE_ROM( 0310A );		// check_oss
	DEFINE_ROM( 03166 );		// check_V8
	DEFINE_ROM( 031A0 );		// check_mcu
	DEFINE_ROM( 031AE );		// check_jaws
	DEFINE_ROM( 047CC );		// read_id_from_via
	DEFINE_ROM( 04052 );		// init_mmu
	DEFINE_ROM( 04640 );		// has_fpu
	DEFINE_ROM( 046AA );		// has_via
	DEFINE_ROM( 046AC );		// has_via/offset
	DEFINE_ROM( 046D8 );		// has_rbv
	DEFINE_ROM( 04B6C );		// test_model_id
	DEFINE_ROM( 04BE4 );		// get_model_id
#if 0
	rom_routines[0x2F64] = getHWInfo;
	rom_routines[0x2e00] = init_hw;
	rom_routines[0x2e8c] = reset_via;
	rom_routines[0x309C] = update_rom_flag;
	rom_routines[0x30AC] = check_glu;
	rom_routines[0x30CA] = check_mdu;
	rom_routines[0x310A] = check_oss;
	rom_routines[0x3166] = check_V8;
	rom_routines[0x31A0] = check_mcu;
	rom_routines[0x31AE] = check_jaws;
	rom_routines[0x31F2] = check_niagara;
	rom_routines[0x323A] = check_null;
	
	rom_routines[0x4052] = init_mmu;
	rom_routines[0x4640] = has_fpu;
	rom_routines[0x46AA] = has_via;
	rom_routines[0x46AC] = has_via_offset;
	rom_routines[0x46D8] = has_rbv;
	rom_routines[0x4700] = test_scsi_io;
	rom_routines[0x4708] = test_scsi_cmd;
	rom_routines[0x477C] = test_iop;
	rom_routines[0x47CC] = read_id_from_via;
	rom_routines[0x4866] = init_hw3;
	rom_routines[0x4A5E] = init_hw2;
	rom_routines[0x4A96] = test_scsi;
	rom_routines[0x4B6C] = test_model_id;
	rom_routines[0x4BE4] = get_model_id;

	rom_routines[0x7058] = play_asc_1;
	rom_routines[0x706E] = play_asc;

	uint32_t boot2_addr = read_rom32(0x8e2)+0x8e0;
	rom_routines[boot2_addr] = boot_second;
	rom_routines[0x45C1C] = play_chime_1;
	rom_routines[0x45C64] = play_easc;
	rom_routines[0x45E3C] = wait_until_easc_fifo_half;
	rom_routines[0x45EA2] = easc_beep;

	rom_routines[0x47840] = send_to_xxx_dword;
	rom_routines[0x478C2] = send_to_xxx;
	rom_routines[0x47928] = recv_from_xxx;
	rom_routines[0x47C40] = check_checksum;

	rom_routines[0x8AC00] = init_powerbook;
	rom_routines[0x8c400] = _8C400;
	rom_routines[0xB29C4] = init_pb_duo;
	rom_routines[0xB9F78] = init_modem_port;
	rom_routines[0xBA0EA] = read_modem_port;
	rom_routines[0xBD3EC] = check_easc_fifo_empty;
	rom_routines[0xBD41E] = check_easc_fifo_half;
	rom_routines[0xBD470] = cleanup_easc;
	rom_routines[0xBD494] = check_easc_fifo_done;
	rom_routines[0x7052] = _7052;
	rom_routines[0x85714] = init_adb;
	rom_routines[0xaa15c] = _AA15C;
	rom_routines[0xaa194] = skip_sr;

#endif
	// TODO: set preference
	init_machine(MB_TYPE::GLU);
	machine->model_id = 0xa55a0000;
	ROM::mb_type = MB_TYPE::MCU;
	//set_machine("");
}
