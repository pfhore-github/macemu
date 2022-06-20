#ifndef ROMDATA_H__
#define ROMDATA_H__
#include <stdint.h>
#include <vector>
#define EMULATE_Q900 1
#define CPU 4
extern bool rom_overlay;
namespace constant {
namespace header {
constexpr uint32_t CHECKSUM = 0x3DC27823;
constexpr uint32_t BOOT_ADDR = 0x4080002A;
constexpr uint32_t RESET_ADDR = 0x4080008C;
constexpr uint16_t MAJOR_V = 0x067C;
constexpr uint16_t MINOR_V = 0x17F2;
constexpr uint32_t RSRC_MAP = 0x07EC10;
constexpr uint32_t XV_ADDR = 0x40801240;
constexpr uint32_t TRAP_TABLE = 0x408CA8E0;
} // namespace header
enum class ADDR {
    ROM, EXTROM, VIA1, SCC_R, SCC_W, SWIM, UNKNOWN1, UNKNOWN2,
    SCSI_NORMAL, SCSI_PDMA, SCSI_HANDSHAKE, VIA2,
    ASC, RBV, VDAC, SCSI_DMA, IOP_SWIM, IOP_SCC,
    OSS, EXP0, EXP1, MCU_CTL, PB_CTL, ETHER, SCSI_INT,
    SCSI_EXT, QUADRA_VCTL
};
constexpr uint32_t MB_MCU_DATA = 0x40803568;
constexpr uint32_t QUADRA_950_DATA = 0x408038BC;
struct MOTHERBOARD {
    uint32_t ROM_ADDR;
    uint32_t HW_FLG1;
    uint32_t HW_FLG2;
    uint8_t via1_mask_a;
    uint8_t via1_mask_b;
    uint8_t via2_mask_a;
    uint8_t via2_mask_b;
    void (*checker) ();
    uint32_t id;
    uint32_t addr[32];
};
struct MACHINE {
    uint32_t ROM_ADDR;
    const MOTHERBOARD* MEM_CONTROLLER;
    void* RAM_TYPE;
    void* VRAM_TYPE;
    uint32_t unknwon1;
    uint32_t unknwon2;
    uint16_t HW_FLG3;
    uint8_t MACHINE_ID;
    uint8_t MB_ID;
    uint16_t unknwon3;
    uint8_t gen2;
    uint8_t pad;
    uint32_t HW_FLG1;
    uint32_t HW_FLG2;
    uint32_t MODEL_MASK;
    uint32_t MODEL_ID;
    void* VIA_DATA1;
    void* VIA_DATA2;
};

struct VIA_DATA {
    uint8_t REGA, DIRA, REGB, DIRB, PCR, ACR;
};

extern const MOTHERBOARD MOTHERDATA[6];
extern const MACHINE MACHINEDATA[16];

constexpr VIA_DATA VIA1_DATA{0x00, 0x00, 0xc9, 0xf1, 0x22, 0x1c};
constexpr VIA_DATA VIA2_DATA{0x80, 0x80, 0x03, 0xc8, 0x22, 0xc0};
} // namespace constant
void run_m68k(uint32_t until);
void run_m68k(const std::vector<uint32_t> &until);

void reset_all();

void call(uint32_t addr, uint32_t ret);
void jmp(uint32_t addr);

#endif