#include "compiler/compiler.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "newcpu.h"
inline int from_bcd(uint8_t v) { return ((v >> 4) & 0xf) * 10 + (v & 0xf); }
inline uint8_t to_bcd(int v) { return ((v / 10) % 10) << 4 | (v % 10); }

std::function<void()> compile_nbcd(uint16_t op, int type, int reg, int dm) {
    if(type == 1) {
        // LINK.L
        return [reg]() {
            int32_t disp = FETCH32();
            PUSH32(regs.a[reg]);
            regs.a[reg] = regs.a[7];
            regs.a[7] += disp;
        };
    } else {
        return EA_Update8(type, reg, [](uint8_t v1) {
            int v = 100 - from_bcd(v1) - regs.x;
            bool cc = (v1 % 100) != 0;
            regs.x = regs.c = cc;
            if(cc) {
                regs.z = false;
            }
            return to_bcd(v);
        });
    }
}

uint8_t do_sbcd(uint8_t a, uint8_t b) {
    int v = from_bcd(a) - from_bcd(b) - regs.x;
    if(v < 0) {
        regs.c = true;
        v += 100;
    } else {
        regs.c = false;
    }
    regs.x = regs.c;
    regs.z = v == 0;
    return to_bcd(v);
}

uint8_t do_pack(uint16_t v) { return (v & 0xf) | (v >> 4 & 0xf0); }
uint16_t do_unpk(uint8_t v) { return (v & 0xf0) << 4 | (v & 0x0f); }

uint8_t do_abcd(uint8_t a, uint8_t b) {
    int v = from_bcd(a) + from_bcd(b) + regs.x;
    if(v > 99) {
        regs.c = true;
        v -= 100;
    } else {
        regs.c = false;
    }
    regs.x = regs.c;
    if( v ) {
        regs.z = false;
    }
    return to_bcd(v);
}