#include "./romdata.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include "sysdeps.h"
#include <SDL.h>
void reset_all();

void call(uint32_t addr, uint32_t ret) {
    regs.pc = ROM_BASE() | addr;
    regs.a[6] = ret;
    run_m68k(ret);
}
void init_mmu();
void init_hw1();
// 008C
void bios_init() {
    // S=TRUE, IM=7
    SET_SR(0x2700);

    // init_mmu
    init_mmu();

    reset_all();

    // init_hw1
    init_hw1();

    regs.pc = ROM_BASE() | 0x8e0;
    run_m68k(0);
}
void get_hwinfo();
void init_via();
void init_ctl_rg();
// $2E00
void init_hw1() {
    regs.a[7] = 0x2600;

    uint32_t off = constant::MCU.rom_addr; // read32(regs.a[0]);

    regs.a[0] += off;
    regs.a[1] += off;
    regs.a[6] += off;
    regs.a[7] += off;
    rom_overlay = false;

    regs.vbr = ROM_BASE() | 0x3dec;
    regs.d[2] = 0;
    get_hwinfo();

    init_via();

    init_ctl_rg();

    return;
}

// $2E8C
void init_via() {

    uint32_t via1 = regs.a[2] = constant::MCU.via1_addr;
    write8(via1 + 0x1e00, constant::VIA1_DATA.REGA);
    write8(via1 + 0x0600, constant::VIA1_DATA.DIRA);
    write8(via1 + 0x0000, constant::VIA1_DATA.REGB);
    write8(via1 + 0x0400, constant::VIA1_DATA.DIRB);
    write8(via1 + 0x1800, constant::VIA1_DATA.PCR);
    write8(via1 + 0x1600, constant::VIA1_DATA.ACR);
    write8(via1 + 0x1c00, 0x7f);

    uint32_t via2 = regs.a[2] = constant::MCU.via2_addr;
    write8(via2 + 0x1e00, read8( via2 + 0x1e00 ) | constant::VIA2_DATA.REGA);
    write8(via2 + 0x0600, constant::VIA2_DATA.DIRA);
    write8(via2 + 0x0000, constant::VIA2_DATA.REGB);
    write8(via2 + 0x0400, constant::VIA2_DATA.DIRB);
    write8(via2 + 0x1800, constant::VIA2_DATA.PCR);
    write8(via2 + 0x1600, constant::VIA2_DATA.ACR);
    write8(via2 + 0x1c00, 0x7f);
    write8(via2 + 0x0800, 0x6e);
    write8(via2 + 0x0A00, 0x19);

    regs.pc = regs.a[6];
    return;
}
// $2F18
void get_hwinfo() {
#if not(defined(ROM_LLE))
    regs.a[0] = ROM_BASE() | constant::MB_MCU_DATA;     // MCU
    regs.a[1] = ROM_BASE() | constant::QUADRA_950_DATA; // Quadra 950
    regs.d[0] = constant::QUADRA_950.HW_FLG1; //  read32(regs.a[1] + 24);
    regs.d[1] = constant::QUADRA_950.HW_FLG2; // read32(regs.a[1] + 28);
    regs.d[2] = constant::QUADRA_950.HW_FLG3 << 16 |
                constant::QUADRA_950.MACHINE_ID << 8 |
                constant::QUADRA_950.MB_ID; // read32(regs.a[1] + 16);
    regs.pc = regs.a[6];
#else

#endif
    return;
}

// $4052
void init_mmu() {
    // cinva bc; flash cache
    // disable cache
    regs.cacr_de = regs.cacr_ie = false;
    // disable paging
    regs.tcr_e = regs.tcr_p = false;
    // 0x800000,00 ~ 0xFF0000,00 (HIGH AREA(Nubus?))
    regs.DTTR[0].address_base = 0x80;
    regs.DTTR[0].address_mask = 0x7f;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].CM = 2;

    // 0x500000,00 ~ 0x5F0000,00 (IO Area)
    regs.DTTR[0].address_base = 0x50;
    regs.DTTR[0].address_mask = 0x0f;
    regs.DTTR[0].E = true;
    regs.DTTR[0].S = 2;
    regs.DTTR[0].CM = 2;

    regs.pc = regs.a[6];

    return;
}

// $047AE
void init_via_dir() {
    uint32_t via_v = 0;
    {
        uint32_t via1 = constant::MCU.via1_addr;
        uint8_t reg_a_oldv = read8(via1 + 0x1e00);
        uint8_t reg_a_dir = read8(via1 + 0x0600);
        write8(via1 + 0x0600, reg_a_dir & constant::MCU.via1_mask_a);
        uint8_t reg_a_newv = read8(via1 + 0x1e00);
        write8(via1 + 0x0600, reg_a_dir);
        write8(via1 + 0x1e00, reg_a_oldv);

        uint8_t reg_b_oldv = read8(via1 + 0x0000);
        uint8_t reg_b_dir = read8(via1 + 0x0400);
        write8(via1 + 0x0400, reg_b_dir & constant::MCU.via1_mask_b);
        uint8_t reg_b_newv = read8(via1 + 0x0000);
        write8(via1 + 0x0400, reg_b_dir);
        write8(via1 + 0x0000, reg_b_oldv);

        via_v = reg_a_newv << 24 | reg_b_newv << 16;
    }

    {
        uint32_t via2 = constant::MCU.via2_addr;
        uint8_t reg_a_oldv = read8(via2 + 0x1e00);
        uint8_t reg_a_dir = read8(via2 + 0x0600);
        write8(via2 + 0x0600, reg_a_dir & constant::MCU.via2_mask_a);
        uint8_t reg_a_newv = read8(via2 + 0x1e00);
        write8(via2 + 0x0600, reg_a_dir);
        write8(via2 + 0x1e00, reg_a_oldv);

        uint8_t reg_b_oldv = read8(via2 + 0x0000);
        uint8_t reg_b_dir = read8(via2 + 0x0400);
        write8(via2 + 0x0400, reg_b_dir & constant::MCU.via2_mask_b);
        uint8_t reg_b_newv = read8(via2 + 0x0000);
        write8(via2 + 0x0400, reg_b_dir);
        write8(via2 + 0x0000, reg_b_oldv);

        via_v |= reg_a_newv << 8 | reg_b_newv;
    }
    regs.d[1] = via_v;
    regs.pc = regs.a[6];
}
void init_ctl_rg() {

    // MCU init
    uint32_t mcu = constant::MCU.mcu_ctl_addr;
    uint32_t value = 0x124f0810;
    uint32_t via2 = constant::MCU.via2_addr;
    write8(via2 + 0x0400, read8(via2 + 0x0400) & ~(1 << 5));
    if(read8(via2) & 1 << 5) {
        value = 0x138b0810;
    }
    for(int d = 0; d < 32; ++d) {
        write32(mcu + (d << 2), value);
        mcu += 4;
        value >>= 1;
    }
    regs.d[4] = 0;
    write32(mcu + 0x80, 0);
    write32(mcu + 0xa0, 0xffff);
    write32(mcu + 0xa4, 0xffff);
    write32(mcu + 0xa8, 0xffff);
    write32(mcu + 0xac, 0xffff);
    write32(mcu + 0xb0, 0xffff);
    write32(mcu + 0xb4, 0xffff);
    write32(mcu + 0xb8, 0xffff);

    uint32_t v = !(read32(mcu + 0x48) & 1);
    write32(mcu + 0x84, v);

    return;
}

void init_second();
std::unordered_map<uint32_t, void (*)()> rom_functions = {
    {0x0008c, bios_init},   {0x008E0, init_second}, {0x04052, init_mmu},
    {0x02e00, init_hw1},    {0x02E8c, init_via},    {0x02f18, get_hwinfo},
    {0x47ae, init_via_dir}, {0x04848, init_ctl_rg}};