#ifndef ROMDATA_H__
#define ROMDATA_H__
#include <vector>
#include <stdint.h>
#define EMULATE_Q900 1
#define CPU 4
extern bool rom_overlay;
namespace constant {
constexpr uint32_t CHECKSUM = 0x3dc27823;
constexpr uint32_t BOOT_ADDR = 0x2a;
constexpr uint16_t MAJOR_V = 0x67c;
constexpr uint32_t MB_MCU_DATA = 0x03568;
constexpr uint32_t QUADRA_950_DATA = 0x038BC;
struct MOTHERBOARD {
    uint32_t HW_FLG1;
    uint32_t HW_FLG2;
    uint8_t via1_mask_a;
    uint8_t via1_mask_b;
    uint8_t via2_mask_a;
    uint8_t via2_mask_b;
    uint32_t checker;
    uint32_t id;
    uint32_t rom_addr;
    uint32_t exrom_addr;
    uint32_t via1_addr;
    uint32_t scc_r_addr;
    uint32_t scc_w_addr;
    uint32_t swim_addr;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t scsi_normal_addr;
    uint32_t scsi_pdma_addr;
    uint32_t scsi_handshake_addr;
    uint32_t via2_addr;
    uint32_t asc_addr;
    uint32_t rbv_addr;
    uint32_t vdac_addr;
    uint32_t scsi_dma_addr;
    uint32_t iop_swim_addr;
    uint32_t iop_scc_addr;
    uint32_t oss_addr;
    uint32_t iifx_exp0_addr;
    uint32_t iifx_exp1_addr;
    uint32_t mcu_ctl_addr;
    uint32_t jaws_ctl_addr;
    uint32_t ether_ctl_addr;
    uint32_t mcu_scsi_internal_addr;
    uint32_t mcu_scsi_external_addr;
    uint32_t quadra_ctl_addr;
};
constexpr MOTHERBOARD MCU{
    0x7a3181f, 0,          0, 0xc0,       0,          0,          0x03154,
    8,         0x40800000, 0, 0x50f00000, 0x50f0c020, 0x50f0c020, 0x50f1e000,
    0,         0,          0, 0,          0,          0x50f02000, 0x50f14000,
    0,         0xf9800000, 0, 0x50f1e020, 0x50f0c020, 0,          0,
    0,         0x5000e000, 0, 0x50f0a000, 0x50f0f000, 0x50f0f402, 0xf9800000};
struct MACHINE {
    uint32_t MEM_CONTROLLER;
    uint32_t RAM_TYPE;
    uint32_t unknwon;
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
    uint32_t VIA_DATA1;
    uint32_t VIA_DATA2;
};
constexpr MACHINE QUADRA_950{
    MB_MCU_DATA, 0x00003B80, 0,       0,       0xDC00,    0x14,
    0x08,        0x3fff,     2,       0,       0x7A31807, 0x40924,
    0x56000000,  0x10000000, 0x03B1A, 0x03B3E,
};
struct VIA_DATA {
    uint8_t REGA, DIRA, REGB, DIRB, PCR, ACR;
};
constexpr VIA_DATA VIA1_DATA{0x00, 0x00, 0xc9, 0xf1, 0x22, 0x1c};
constexpr VIA_DATA VIA2_DATA{0x80, 0x80, 0x03, 0xc8, 0x22, 0xc0};
} // namespace constant
void run_m68k(uint32_t until);
void run_m68k(const std::vector<uint32_t> &until);

inline uint32_t ROM_BASE() { return rom_overlay ? 0 : 0x40800000; }
void reset_all();

void call(uint32_t addr, uint32_t ret) ;


enum class HW_FLG1 {
    MASTER,
    EXROM,
    VIA1,
    SCC_R,
    SCC_W,
    SWIM,
    VIA2 = 11,
    ASC,
    RBV,
    VDAC,
    SCSI_DMA,
    IOP_SWIM,
    IOP_SCC,
    OSS,
    IIFX_EXP0,
    IIFX_EXP1,
    MCU,
    POWERBOOK,
    ETHERNET,
    SCSI_040,
    LOW_MEM_GLOBAL = 29
};
#endif