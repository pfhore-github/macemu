#include "./romdata.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include "sysdeps.h"
#include <SDL.h>

void run_m68k(uint32_t until);
void run_m68k(const std::vector<uint32_t> &until);
void call(uint32_t addr, uint32_t ret);
extern bool rom_overlay;
extern uint32_t special_rom;
namespace ROM {
void get_hwinfo();
void get_scc_addr();
#if 0
void init_second() {
    // $008E0 -> $46C06
    regs.a[7] = 0x2600;
    regs.d[7] = 0;
    regs.d[6] = 0;
    regs.vbr = 0x40846980;
    regs.a[5] = regs.a[7];

    regs.d[2] = 0;
    regs.IM = 7;
    get_hwinfo();

    if(special_rom) {
        regs.a[1] = 0x40846C58;
        regs.a[3] = 0x4084B9CA;
        call(special_rom, 0x46c58);
        regs.d[2] = 0;
    }

    // 46C62
    regs.d[7] &= ~(1 << 27);
    regs.d[7] = (regs.d[7] & 0xffff0000) | 18;
    get_scc_addr();
    call(0x477c4, 0x46c80);
    if(!regs.z) {
        // $49AFA
        regs.a[7] = 0x2600;
        regs.d[2] = 0;
        call(0x470ba, 0x49b08);
        if(regs.d[7] & 1 << 26) {
//            regs.a[3] = constant::MCU.asc_addr;
//            regs.a[5] = constant::MCU.via1_addr;
            call(0x45c1c, 0x49b24);
        } else {
            if(special_rom) {
                regs.a[1] = 0x40846C58;
                regs.a[3] = 0x4084B9CA;
                call(special_rom, 0x49b4c);
                regs.d[2] = 0;
            }
            regs.d[7] &= ~(1 << 27);
            regs.d[7] |= (1 << 16);
            regs.d[7] |= (1 << 22);
            regs.d[7] = (regs.d[7] & 0xffff0000) | 12;
            call(0x4af4a, 0x49b64);
            // CINVA IC
        }
    }
    // 4A7E6
    regs.d[2] = 0;
    call(0x470ba, 0x46c8e);
//    regs.a[2] = constant::MCU.via1_addr;
    if(regs.d[2] & 1 << 24) {
        call(0x471c6, 0x4a802);
    }
}

const std::vector<uint8_t> v_47804 = {0x19, 0x81, 0x8d, 0x30, 0xf0, 0xa9,
                                      0x44, 0x8d, 0x31, 0xf0, 0x80, 0xfe,
                                      0xee, 0x7f, 0xee, 0x7f, 0xee, 0x7f};
void _477c4() {
    write8(regs.a[3] - 28, 50);
    regs.d[0] = 17;
    write16(regs.a[3] - 31, 0x7fee);
    for(auto v : v_47804) {
        write8(regs.a[3] - 31, v);
    }
}
// $4AE28
void get_scc_addr() {
    regs.a[3] = regs.a[6];
    regs.d[2] = 0;
    call(0x470ba, 0x4ae34);
    regs.a[6] = regs.a[3];
    regs.a[2] = constant::MCU.via1_addr;
    if(regs.d[0] & 1 << int(HW_FLG1::IOP_SCC)) {
        regs.a[3] = constant::MCU.iop_scc_addr;
        regs.d[3] = 0;
    } else {
        regs.a[3] = constant::MCU.scc_r_addr;
        regs.d[3] = constant::MCU.scc_w_addr - regs.a[3]; // (0)
    }
    regs.pc = regs.a[6];
    return;
}
#endif
} // namespace ROM