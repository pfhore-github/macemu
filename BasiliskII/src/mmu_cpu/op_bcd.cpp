#include "cpu_emulation.h"
#include "main.h"
#include "sysdeps.h"
#include <limits.h>
#include <stdlib.h>

#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"
#include "op.h"

int from_bcd(uint8_t v) { return ((v >> 4) & 0xf) * 10 + (v & 0xf); }
uint8_t to_bcd(int v) { return ((v / 10) % 10) << 4 | (v % 10); }
uint8_t do_nbcd(uint8_t v1) {
    int v = 100 - from_bcd(v1) - regs.x;
    bool cc = (v1 % 100) != 0;
    regs.x = regs.c = cc;
    if(cc) {
        regs.z = false;
    }
    return to_bcd(v);
}
OP(nbcd) {
    if(type == 1) {
        op_link_l(reg);
    } else {
        EA_Update8(type, reg, do_nbcd);
    }
}
void op_sbcd_d(int dm, int reg) {
    uint8_t src = regs.d[reg];
    uint8_t dst = regs.d[dm];
    int v = from_bcd(dst) - from_bcd(src) - regs.x;
    if(v < 0) {
        regs.c = true;
        v += 100;
    } else {
        regs.c = false;
    }
    regs.x = regs.c;
    regs.z = v == 0;
    WRITE_D8(dm, to_bcd(v));
}

void op_sbcd_a(int dm, int reg) {
    uint8_t src = read8(--regs.a[reg]);
    uint8_t dst = read8(--regs.a[dm]);
    int v = from_bcd(dst) - from_bcd(src) - regs.x;
    if(v < 0) {
        regs.c = true;
        v += 100;
    } else {
        regs.c = false;
    }
    regs.x = regs.c;
    regs.z = v == 0;
    write8(regs.a[dm], to_bcd(v));
}

void op_pack_d(int dm, int reg) {
    uint16_t src = regs.d[reg];
    uint16_t adjust = FETCH();
    uint16_t dst = src + adjust;
    WRITE_D16(dm, (dst & 0xf) | (dst >> 4 & 0xf0));
}

void op_pack_a(int dm, int reg) {
    uint16_t adjust = FETCH();
    uint8_t src1 = read8(--regs.a[reg]);
    uint8_t src2 = read8(--regs.a[reg]);
    uint16_t dst = (src1 | src2 << 8) + adjust;
    write8(--regs.a[dm], (dst & 0xf) | (dst >> 4 & 0xf0));
}

void op_unpk_d(int dm, int reg) {
    uint16_t src = (regs.d[reg] & 0xf0) << 4 | (regs.d[reg] & 0x0f);
    uint16_t adjust = FETCH();
    WRITE_D16(dm, src + adjust);
}

void op_unpk_a(int dm, int reg) {
    uint16_t adjust = FETCH();
    uint8_t src = read8(--regs.a[reg]);
    uint16_t dst = ((src & 0xf0) << 4 | (src & 0x0f)) + adjust;
    write16(regs.a[dm] -= 2, dst);
}
uint8_t do_abcd(uint8_t a, uint8_t b) {
    int v = from_bcd(a) + from_bcd(b) + regs.x;
    if(v > 99) {
        regs.c = true;
        v -= 100;
    } else {
        regs.c = false;
    }
    regs.x = regs.c;
    regs.z = v == 0 ? false : regs.z;
    return to_bcd(v);
}
void op_abcd_d(int dm, int reg) {
    uint8_t src = regs.d[reg];
    uint8_t dst = regs.d[dm];
    WRITE_D8(dm, do_abcd(dst, src));
}

void op_abcd_a(int dm, int reg) {
    uint8_t src = read8(--regs.a[reg]);
    uint8_t dst = read8(--regs.a[dm]);
    write8(regs.a[dm], do_abcd(dst, src));
}
