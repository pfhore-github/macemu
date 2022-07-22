#include "./romdata.h"
#include "ex_stack.h"
#include "mbus.h"
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "sysdeps.h"
#include "../chips/via.h"
#include "../chips/iop.h"
#include <SDL.h>
void reset_all();
using namespace constant;

void call(uint32_t addr, uint32_t ret) {
    regs.pc = 0x40800000 | addr;
    regs.a[6] = 0x40800000 | ret;
    run_m68k(ret);
}

void jmp(uint32_t addr) {
    regs.pc = 0x40800000 | addr;
    run_m68k(0);
}

uint32_t do_op_movec_from(int o);
void do_op_movec_to(int op, uint32_t v);

namespace ROM {
void bios_init();
void init_mmu();
void check_via();
void check_rbv();
void get_via_machine_id();
void check_scsi_dma();
void check_scsi_mdu();
void boot() {
    bios_init(); // $008C
}
// 008C
void bios_init() {
    // S=TRUE, IM=7
    SET_SR(0x2700);

    // init_mmu
    init_mmu(); // $04052

    reset_all();

    // init_hw1
    call(0x2e00, 0xa6);

    // init_pb
    call(0x8ac00, 0xb4);

    call(0x8e0, 0);
}
void get_hwinfo();
void init_via();
void init_ctl_rg();
void ex_handler1() {
    regs.z = regs.d[7] & 1 << 27;
    jmp(regs.a[6]);
}
// $02E00
static const uint32_t VBR1[] = {0x3DEC, 0x40803DEC, 0x40003DEC, 0x00A03DEC};
void init_hw1() {
    regs.a[7] = 0x2600;
    // $02E06 -$02E1C
    for(auto vbr : VBR1) {
        if((regs.pc & 0xfff00000) == (vbr & 0xfff00000)) {
            regs.vbr = vbr;
            break;
        }
    }

    //    uint32_t off = MCU.rom_addr; // read32(regs.a[0]);

    regs.vbr = 0x40803dec;
    regs.d[2] = 0;
    get_hwinfo();

    //    init_via();

    //    init_ctl_rg();

    return;
}
#if 0
// $2E8C
void init_via() {

    uint32_t via1 = regs.a[2] = MCU.via1_addr;
    write8(via1 + 0x1e00, VIA1_DATA.REGA);
    write8(via1 + 0x0600, VIA1_DATA.DIRA);
    write8(via1 + 0x0000, VIA1_DATA.REGB);
    write8(via1 + 0x0400, VIA1_DATA.DIRB);
    write8(via1 + 0x1800, VIA1_DATA.PCR);
    write8(via1 + 0x1600, VIA1_DATA.ACR);
    write8(via1 + 0x1c00, 0x7f);

    uint32_t via2 = regs.a[2] = MCU.via2_addr;
    write8(via2 + 0x1e00, read8(via2 + 0x1e00) | VIA2_DATA.REGA);
    write8(via2 + 0x0600, VIA2_DATA.DIRA);
    write8(via2 + 0x0000, VIA2_DATA.REGB);
    write8(via2 + 0x0400, VIA2_DATA.DIRB);
    write8(via2 + 0x1800, VIA2_DATA.PCR);
    write8(via2 + 0x1600, VIA2_DATA.ACR);
    write8(via2 + 0x1c00, 0x7f);
    write8(via2 + 0x0800, 0x6e);
    write8(via2 + 0x0A00, 0x19);

    regs.pc = regs.a[6];
    return;
}
#endif
const MOTHERBOARD *HWINFO_A0;
const MACHINE *HWINFO_A1;
uint32_t HWINFO_D0;
uint32_t HWINFO_D1;
uint32_t HWINFO_D2;
// $2F18
void get_hwinfo() {
    uint32_t callee = regs.a[6];
#if !!defined(ROM_LLE)
    regs.a[0] = MB_MCU_DATA;        // MCU
    regs.a[1] = QUADRA_950_DATA;    // Quadra 950
    regs.d[0] = QUADRA_950.HW_FLG1; //  read32(regs.a[1] + 24);
    regs.d[1] = QUADRA_950.HW_FLG2; // read32(regs.a[1] + 28);
    regs.d[2] = QUADRA_950.HW_FLG3 << 16 | QUADRA_950.MACHINE_ID << 8 |
                QUADRA_950.MB_ID; // read32(regs.a[1] + 16);
    regs.pc = regs.a[6];
#else
    uint8_t arg = regs.d[2] & 0xff;
    if(arg) {
        for(const auto &m : MACHINEDATA) {
            regs.a[1] = (HWINFO_A1 = &m)->ROM_ADDR;
            if(HWINFO_A1->MACHINE_ID != arg) {
                continue;
            }
            goto FOUND;
        }
    } else {
        for(const auto &mb : MOTHERDATA) {
            regs.a[0] = (HWINFO_A0 = &mb)->ROM_ADDR;
            try {
                (*HWINFO_A0->checker)();
            } catch(BUS_ERROR_EX &e) {
                regs.pc = 0x40802f58;
            }
            if(regs.pc == 0x40802f64) {
                break;
            }
        }
        regs.a[2] = regs.d[0];
        // get_via_machine_id
        get_via_machine_id(); // $047AE
        HWINFO_D2 = HWINFO_A0->id;
        for(const auto &m : MACHINEDATA) {
            regs.a[1] = (HWINFO_A1 = &m)->ROM_ADDR;
            if(HWINFO_A1->MB_ID != HWINFO_A0->id) {
                continue;
            }
            if((regs.d[1] & HWINFO_A1->MODEL_MASK) != HWINFO_A1->MODEL_ID) {
                continue;
            }
            goto FOUND;
        }
    }
GENERIC:
    HWINFO_D0 = regs.d[0] = HWINFO_A0->HW_FLG1;
    if(HWINFO_D0 & 1 << int(ADDR::RBV)) {
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::RBV)] + 19;
        try {
            check_rbv(); // $046D6
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::RBV)));
        }
    }
    if(HWINFO_D0 & 1 << int(ADDR::VIA2)) {
        HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::VIA2)));
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::VIA2)] + 0x1c00;
        regs.d[2] = 0;
        try {
            check_via(); // $046AA
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 = (regs.d[0] |= 1 << int(ADDR::VIA2));
        }
    }

    if(HWINFO_D0 & 1 << int(ADDR::IOP_SWIM)) {
        HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::IOP_SWIM)));
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::IOP_SWIM)];
        try {
            call(0x477A, 0x2FF2);
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 &= ~(1 << int(ADDR::SWIM));
            HWINFO_D0 |= 1 << int(ADDR::SWIM);
            regs.d[0] = HWINFO_D0;
        }
    }

    if(HWINFO_D0 & 1 << int(ADDR::IOP_SCC)) {
        HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::IOP_SCC)));
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::IOP_SCC)];
        try {
            call(0x477A, 0x3010);
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 |= 1 << int(ADDR::IOP_SCC);
            HWINFO_D0 &= ~(1 << int(ADDR::SCC_R));
            HWINFO_D0 &= ~(1 << int(ADDR::SCC_W));
            regs.d[0] = HWINFO_D0;
        }
    }

    if(HWINFO_D0 & 1 << int(ADDR::SCSI_DMA)) {
        HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::SCSI_DMA)));
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::SCSI_DMA)];
        try {
            check_scsi_dma(); // $046FE
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 |= 1 << int(ADDR::SCSI_DMA);
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_NORMAL));
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_PDMA));
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_HANDSHAKE));
            regs.d[0] = HWINFO_D0;
        }
    }

    // $03D48
    if(HWINFO_D0 & 1 << int(ADDR::SCSI_INT)) {
        HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::SCSI_INT)));
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::SCSI_INT)];
        try {
            check_scsi_mdu(); // $04706
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 |= ~(1 << int(ADDR::SCSI_INT));
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_DMA));
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_NORMAL));
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_PDMA));
            HWINFO_D0 &= ~(1 << int(ADDR::SCSI_HANDSHAKE));
            if(HWINFO_D0 & 1 << int(ADDR::SCSI_EXT)) {
                HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::SCSI_EXT)));
                regs.a[2] =
                    HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::SCSI_EXT)];
                try {
                    check_scsi_mdu(); // $04706
                } catch(BUS_ERROR_EX &e) {
                    regs.z = false;
                }
                if(!regs.z) {
                    HWINFO_D0 |= ~(1 << int(ADDR::SCSI_INT));
                }
            }
        }
        regs.d[0] = HWINFO_D0;
    }

    if(HWINFO_D0 & 1 << int(ADDR::EXP1)) {
        HWINFO_D0 = (regs.d[0] &= ~(1 << int(ADDR::EXP1)));
        regs.a[2] = HWINFO_A1->MEM_CONTROLLER->addr[int(ADDR::EXP1)];
        try {
            call(0x4670, 0x3DA0);
        } catch(BUS_ERROR_EX &e) {
            regs.z = false;
        }
        if(!regs.z) {
            HWINFO_D0 |= 1 << int(ADDR::EXP1);
            regs.d[0] = HWINFO_D0;
        }
    }
    // $0304A
    goto DONE;
FOUND:
    // $02F3A
    regs.a[0] = (HWINFO_A0 = HWINFO_A1->MEM_CONTROLLER)->ROM_ADDR;
    HWINFO_D0 = regs.d[0] = HWINFO_A1->HW_FLG1;
    if(HWINFO_D0 == 0) {
        goto GENERIC;
    }

DONE:
    HWINFO_D2 = regs.d[2] = HWINFO_A1->HW_FLG3;

    // $03DBA
    if(!(HWINFO_D2 & (1 << 28))) {
        call(0x4640, 0x3DCC);
        if(!regs.z) {
            HWINFO_D2 &= ~(1 << 28);
            regs.d[2] = HWINFO_D2;
        }
    }

    // $03050
    regs.d[1] = HWINFO_D1 = HWINFO_A1->HW_FLG2;
    if(HWINFO_D1 == 0) {
        regs.d[1] = HWINFO_D1 = HWINFO_A0->HW_FLG2;
    }
    regs.d[7] &= ~(1 << 27);
#endif
    regs.pc = callee;
    return;
}
constexpr uint32_t CHECK_OK = 0x40802f64;
constexpr uint32_t CHECK_NG = 0x40802F58;
// $03060
void check_glue() {
    regs.a[2] = HWINFO_A0->addr[int(ADDR::VIA1)] + 0x1c00;
    regs.d[2] = 0x20000;
    check_via(); // $046AA
    regs.pc = regs.z ? CHECK_OK : CHECK_NG;
}

// $0307E
void check_mdu() {
    regs.a[2] = HWINFO_A0->addr[int(ADDR::VIA1)] + 0x1c00;
    regs.d[2] = 0x40000;
    check_via(); // $046AA
    if(!regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    regs.d[2] -= 0x20000;
    check_via(); // $046AA
    if(regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    read32(HWINFO_A0->addr[int(ADDR::RBV)]);
    read32(HWINFO_A0->addr[int(ADDR::VDAC)]);
    regs.pc = CHECK_OK;
    return;
}

// $30BE
void check_oss() {
    regs.a[2] = HWINFO_A0->addr[int(ADDR::VIA1)] + 0x1c00;
    regs.d[2] = 0x40000;
    check_via(); // $046AA
    if(!regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    regs.d[2] -= 0x20000;
    check_via(); // $046AA
    if(regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    try {
        uint32_t oss = HWINFO_A0->addr[int(ADDR::OSS)];
        uint8_t d1 = read8(oss);
        int d;
        for(d = 6; d >= 0; --d) {
            write8(oss, d);
            if((read8(oss) & 7) == d) {
                break;
            }
        }
        write8(oss, d1);
        regs.pc = d > 0 ? CHECK_NG : CHECK_OK;
        return;
    } catch(BUS_ERROR_EX &e) {
        regs.pc = CHECK_NG;
        return;
    }
}

// $311A
void check_v8() {
    regs.a[2] = HWINFO_A0->addr[int(ADDR::VIA1)] + 0x1c00;
    regs.d[2] = 0;
    check_via(); // $046AA
    if(!regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    regs.d[2] = 0x40000;
    check_via(); // $046AA
    if(regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    try {
        read8(HWINFO_A0->addr[int(ADDR::RBV)]);
        read8(HWINFO_A0->addr[int(ADDR::VDAC)]);
        regs.pc = CHECK_OK;
        return;
    } catch(BUS_ERROR_EX &e) {
        regs.pc = CHECK_NG;
        return;
    }
}

// $03154
void check_mcu() {
    try {
        read32(HWINFO_A0->addr[int(ADDR::MCU_CTL)]);
        regs.pc = CHECK_OK;
        return;
    } catch(BUS_ERROR_EX &e) {
        regs.pc = CHECK_NG;
        return;
    }
}
// $03162
void check_jaws() {
    regs.a[2] = HWINFO_A0->addr[int(ADDR::VIA1)] + 0x1c00;
    regs.d[2] = 0x100000;
    check_via(); // $046AA
    if(!regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    regs.d[2] >>= 1;
    check_via(); // $046AA
    if(regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    check_via(); // $046AA
    if(regs.z) {
        regs.pc = CHECK_NG;
        return;
    }
    try {
        read8(HWINFO_A0->addr[int(ADDR::PB_CTL)]);
        regs.pc = CHECK_OK;
        return;
    } catch(BUS_ERROR_EX &e) {
        regs.pc = CHECK_NG;
        return;
    }
}
// $4052
void init_mmu() {
    constexpr int CACR = 2;
    constexpr int TCR = 3;
    constexpr int DTTR0 = 6;
    constexpr int DTTR1 = 7;
    uint32_t cacr_v = 1 << 31;
    do_op_movec_to(CACR, cacr_v);
    if(do_op_movec_from(CACR) & 1 << 31) {
        // 68040
        // cinva bc; flash cache

        // disable cache
        do_op_movec_to(CACR, 0);
        // disable paging
        do_op_movec_to(TCR, 0);
        // 0x800000,00 ~ 0xFF0000,00 (HIGH AREA(Nubus?))
        do_op_movec_to(DTTR0, 0x807fC040);

        // 0x500000,00 ~ 0x5F0000,00 (IO Area)
        do_op_movec_to(DTTR1, 0x500FC040);
    } else {
        do_op_movec_to(CACR, 0x2000);
        if(do_op_movec_from(CACR)) {
            // pmove 0x00000000, %TC
        }
    }
    regs.pc = regs.a[6];
    return;
}
inline uint8_t negb(uint8_t v) { return (v ^ 0xff) + 1; }
// $046AA
void check_via() {
    uint32_t a2 = regs.a[2];
    read8(a2 + regs.d[2]);
    uint8_t d1l = 0xff;
    uint8_t oldv = read8(a2);
    write8(a2, d1l);
    do {
        d1l = negb(d1l) * 2;
        if(d1l == 0) {
            break;
        }
        d1l = negb(d1l);
        if(read8(a2 + regs.d[2]) != static_cast<uint8_t>(d1l)) {
            break;
        }
    } while(read8(a2) == d1l);
    write8(a2, 0x7f);
    write8(a2, oldv);
    regs.z = (oldv == 0);
    regs.pc = regs.a[6];
    return;
}

// $046D6
void check_rbv() {
    uint32_t a2 = regs.a[2];
    uint8_t oldv = read8(a2);
    uint8_t d1l = 0xff;
    write8(a2, d1l);
    do {
        d1l = negb(d1l);
        write8(a2, d1l);
        d1l *= 2;
        if(d1l == 0) {
            break;
        }
        d1l = negb(d1l);
    } while(((read8(a2) ^ d1l) & 0x9f) == 0);
    write8(a2, 0x7f);
    write8(a2, oldv);
    regs.z = (oldv == 0);
    regs.pc = regs.a[6];
    return;
}
// $046FE
void check_scsi_dma() {
    read32(regs.a[2] + 0x80);
    regs.z = true;
    regs.pc = regs.a[6];
    return;
}
// $04712
void check_scsi_mdu() {
    read8(regs.a[2] + 0xc0);
    regs.z = read8(regs.a[2] + 0xc0) == 4;
    regs.pc = regs.a[6];
    return;
}

// $0477A
void check_iop() {
    uint32_t a2 = regs.a[2];
    uint8_t d1H = read8(a2 - 28);
    uint8_t d2 = 6 & d1H;
    uint16_t d1L = read16(a2-31);
    d2 |= 2;
    write8(a2-28, d2);
    regs.pc = regs.a[6];
    return;
}
// $047AE
void get_via_machine_id() {
    uint32_t via_v = 0;
    if(HWINFO_D0 & 1 << int(ADDR::VIA1)) {
        uint8_t reg_a_oldv = via1.read_reg_a();
        uint8_t reg_a_dir = via1.read_dir_a();
        via1.write_dir_a(reg_a_dir & HWINFO_A0->via1_mask_a);
        uint8_t reg_a_newv = via1.read_reg_a();
        via1.write_dir_a(reg_a_dir);
        via1.write_reg_a(reg_a_oldv);

        uint8_t reg_b_oldv = via1.read_reg_b();
        uint8_t reg_b_dir = via1.read_dir_b();
        via1.write_dir_b(reg_b_dir & HWINFO_A0->via1_mask_b);
        uint8_t reg_b_newv = via1.read_reg_b();
        via1.write_dir_b(reg_b_dir);
        via1.write_reg_b(reg_b_oldv);

        via_v = reg_a_newv << 24 | reg_b_newv << 16;
    }
    if(HWINFO_D0 & 1 << int(ADDR::VIA2)) {
        uint8_t reg_a_oldv = via2.read_reg_a();
        uint8_t reg_a_dir = via2.read_dir_a();
        via2.write_dir_a(reg_a_dir & HWINFO_A0->via2_mask_a);
        uint8_t reg_a_newv = via2.read_reg_a();
        via2.write_dir_a(reg_a_dir);
        via2.write_reg_a(reg_a_oldv);

        uint8_t reg_b_oldv = via2.read_reg_b();
        uint8_t reg_b_dir = via2.read_dir_b();
        via2.write_dir_b(reg_b_dir & HWINFO_A0->via2_mask_b);
        uint8_t reg_b_newv = via2.read_reg_b();
        via2.write_dir_b(reg_b_dir);
        via2.write_reg_b(reg_b_oldv);
        via_v |= reg_a_newv << 8 | reg_b_newv;
    }
    regs.d[1] = via_v;
    regs.pc = regs.a[6];
}
#if 0

void init_ctl_rg() {

    // MCU init
    uint32_t mcu = MCU.mcu_ctl_addr;
    uint32_t value = 0x124f0810;
    uint32_t via2 = MCU.via2_addr;
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
#endif
void init_second();
} // namespace ROM
std::unordered_map<uint32_t, void (*)()> rom_functions = {
    {0x0002a, ROM::boot},
    {0x0008c, ROM::bios_init},
    //    {0x008E0, ROM::init_second},
    //    {0x02e00, ROM::init_hw1},
    //    {0x02E8c, ROM::init_via},
    {0x02f18, ROM::get_hwinfo},
    {0x04052, ROM::init_mmu},
    {0x03060, ROM::check_glue},
    {0x0307E, ROM::check_mdu},
    {0x030BE, ROM::check_oss},
    {0x0311A, ROM::check_v8},
    {0x03154, ROM::check_mcu},
    {0x03162, ROM::check_jaws},
    {0x046AA, ROM::check_via},
    {0x046D6, ROM::check_rbv},
    {0x046FE, ROM::check_scsi_dma},
    {0x04706, ROM::check_scsi_mdu},
    {0x0477A, ROM::check_iop},
    {0x047AE, ROM::get_via_machine_id},
    //    {0x04848, ROM::init_ctl_rg}
};