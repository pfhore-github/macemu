
#include "sysdeps.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"
#include "video.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "mmu_68040.h"
#include "newcpu.h"
#include <unordered_map>
#include <vector>

constexpr presult MMU_BUS_ERROR{false, false, false, false, 0,
                                false, 0,     false, true,  0};

constexpr presult MMU_BUS_INALID{false, false, false, false, 0,
                                 false, 0,     false, false, 0};
mmu_68040 mmu;

constexpr atc_entry_t ATC_INVALID{0, false, false, false, 0, false, 0};
// MMU enabled access
presult test_TTR(const uint32_t addr, bool is_code, bool rw) {
    const ttc_t *ttr = is_code ? mmu.ITTR : mmu.DTTR;
    uint8_t base = addr >> 24;
    for(int i = 0; i < 2; ++i) {
        if(ttr[i].E &&
           ((base & ~ttr->address_mask) ==
            (ttr[i].address_base & ~ttr->address_mask)) &&
           ((ttr[i].S & 2) || (ttr[i].S == !!regs.S))) {
            uint32_t raddr = base << 12 | ((addr >> 12) & 0xfff);
            return {true,          true,   ttr->W, false, ttr->CM,
                    ttr[i].S == 1, ttr->U, false,  false, raddr};
        }
    }
    return MMU_BUS_INALID;
}

presult mmu_68040::ptest(const uint32_t addr, bool rw, bool s) {
    try {
        uint32_t ri = (addr >> 25) << 2;
        uint32_t pi = (addr >> 18 & 0x7f) << 2;
        uint32_t pgi, atc_key = s << 31;
        if(tcr_p) {
            pgi = ((addr >> 13) & 0x1f) << 2;
            atc_key |= addr >> 13;
        } else {
            pgi = ((addr >> 12) & 0x3f) << 2;
            atc_key |= addr >> 12;
        }
        atc_entry_t *entry;
        bool wp = false;
        bool g;
        if(auto gi = atc_gcache.find(atc_key); gi != atc_gcache.end()) {
            entry = &gi->second;
            g = true;
        } else if(auto li = atc_lcache.find(atc_key); li != atc_lcache.end()) {
            entry = &li->second;
            g = false;
        } else {
            uint32_t mst = s ? srp : urp;
            uint32_t rtd = b_read32(mst | ri);
            if(!(rtd & 2)) {
                atc_lcache[atc_key] = ATC_INVALID;
                goto INVALID;
            }
            if(rtd & 4) {
                wp = true;
            }
            b_write32(mst | ri, rtd |= 8);
            uint32_t ptd_addr = (rtd & ~0x1ff) | pi;
            uint32_t ptd = b_read32(ptd_addr);
            if(!(ptd & 2)) {
                atc_lcache[atc_key] = ATC_INVALID;
                goto INVALID;
            }
            b_write32(ptd_addr, ptd |= 8);
            if(ptd & 4) {
                wp = true;
            }
            uint32_t pd_addr = (ptd & ~(tcr_p ? 0x3f : 0x7f)) | pgi;
            uint32_t pd = b_read32(pd_addr);
            switch(pd & 3) {
            case 0:
                goto INVALID;
            case 2:
                pd_addr = pd & ~3;
                pd = b_read32(pd_addr);
                if((pd & 1) == 0) {
                    goto INVALID;
                }
            }
            pd |= 8;
            if(pd & 4) {
                wp = true;
            }
            bool m = pd & 0x10;
            if(rw && !wp && !((pd & 0x80) && !s)) {
                pd |= 0x10;
                m = true;
            }
            b_write32(pd_addr, pd);
            atc_entry_t ent{
                pd >> (tcr_p ? 13 : 12),
                true,
                wp,
                m,
                uint8_t(pd >> 5 & 3),
                bool(pd & 0x80),
                uint8_t(pd >> 8 & 3),
            };
            g = pd >> 10 & 1;
            if(g) {
                atc_gcache[atc_key] = ent;
                entry = &atc_gcache[atc_key];
            } else {
                atc_lcache[atc_key] = ent;
                entry = &atc_lcache[atc_key];
            }
        }
        uint32_t xaddr = entry->addr;
        if(tcr_p) {
            xaddr = (xaddr << 1) | (addr & 1);
        }
        return {true,     false,    wp, entry->M, entry->cm,
                entry->S, entry->U, g,  false,    xaddr};
    } catch(BUS_ERROR_EX &) {
        return MMU_BUS_ERROR;
    }
INVALID:
    return MMU_BUS_INALID;
}
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s) {
    presult ttr_result = test_TTR(vaddr, code, rw);
    if(ttr_result.T) {
        if(ttr_result.W && rw) {
            throw BUS_ERROR_EX{};
        }
        return (ttr_result.addr << 12) | (vaddr & 0xfff);
    }
    if(!mmu.tcr_e) {
        return vaddr;
    }

    presult result = mmu.ptest(vaddr, rw, s);
    if(result.B) {
        // BUS ERROR during table search
        throw BUS_ERROR_EX{true};
    }
    if(!result.R) {
        // no entry for the address
        throw BUS_ERROR_EX{true};
    }
    // Supervisor table
    if(result.S && !s) {
        throw BUS_ERROR_EX{true};
    }

    // readonly table
    if(result.W && rw) {
        throw BUS_ERROR_EX{true};
    }

    return (result.addr << 12) | (vaddr & 0xfff);
}

void mmu_68040::pflushn_an(uint32_t atc_key) { atc_lcache.erase(atc_key); }
void mmu_68040::pflush_an(uint32_t atc_key) {
    atc_gcache.erase(atc_key);
    atc_lcache.erase(atc_key);
}
void mmu_68040::pflushn() { atc_lcache.clear(); }
void mmu_68040::pflush() {
    atc_gcache.clear();
    atc_lcache.clear();
}
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];

// CINV( used only for jit)
OP(cinv_push_dc) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    //    uint32_t addr = regs.a[reg];
    switch(type) {
    case 1:
        break; // CINVL DC, (AN)
    case 2:
        break; // CINVP DC, (AN)
    case 3:
        break; // CINVA DC
    case 5:
        break; // CPUSHL DC, (AN)
    case 6:
        break; // CPUSHP DC, (AN)
    case 7:
        break; // CPUSHA DC
    default:
        ILLEGAL_INST();
    }
}

OP(cinv_push_ic) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    //    uint32_t addr = regs.a[reg];
    switch(type) {
    case 1:
        break; // CINVL IC, (AN)
    case 2:
        break; // CINVP IC, (AN)
    case 3:
        break; // CINVA IC
    case 5:
        break; // CPUSHL IC, (AN)
    case 6:
        break; // CPUSHP IC, (AN)
    case 7:
        break; // CPUSHA IC
    default:
        ILLEGAL_INST();
    }
}

OP(cinv_push_bc) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    //    uint32_t addr = regs.a[reg];
    switch(type) {
    case 1:
        break; // CINVL BC, (AN)
    case 2:
        break; // CINVP BC, (AN)
    case 3:
        break; // CINVA BC
    case 5:
        break; // CPUSHL BC, (AN)
    case 6:
        break; // CPUSHP BC, (AN)
    case 7:
        break; // CPUSHA BC
    default:
        ILLEGAL_INST();
    }
}

OP(pflush) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    uint32_t addr = regs.a[reg];
    uint32_t atc_key;
    if(mmu.tcr_p) {
        atc_key = addr >> 13;
    } else {
        atc_key = addr >> 12;
    }
    switch(type) {
    case 0: // pflushn (an)
        if(regs.dfc == 1 || regs.dfc == 2) {
            mmu.pflushn_an(atc_key);
        } else {
            mmu.pflushn_an(0x80000000 | atc_key);
        }
        break;
    case 1: // pflush (an)
        if(regs.dfc == 1 || regs.dfc == 2) {
            mmu.pflush_an(atc_key);
        } else {
            mmu.pflush_an(0x80000000 | atc_key);
        }
        break;
    case 2: // pflushan
        mmu.pflushn();
        break;
    case 3: // plusha
        mmu.pflush();
        break;
    default:
        ILLEGAL_INST();
    }
}

OP(ptest) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    uint32_t addr = regs.a[reg];
    bool rw;
    if(type == 1) {
        rw = false;
    } else if(type == 5) {
        rw = true;
    } else {
        ILLEGAL_INST();
    }
    presult result = test_TTR(addr, !(regs.dfc & 1), rw);
    if(!result.T) {
        result = mmu.ptest(addr, rw, true);
    }
    mmu.MMUSR.PA = result.addr << 12;
    mmu.MMUSR.B = result.B;
    mmu.MMUSR.G = result.G;
    mmu.MMUSR.U = result.U;
    mmu.MMUSR.S = result.S;
    mmu.MMUSR.CM = result.CM;
    mmu.MMUSR.M = result.M;
    mmu.MMUSR.W = result.W;
    mmu.MMUSR.T = result.T;
    mmu.MMUSR.R = result.R;
}

uint32_t do_op_movec_from(int o) {
    switch(o) {
    case 0x000:
        return regs.sfc;
    case 0x001:
        return regs.dfc;
    case 0x800:
        return regs.usp;
    case 0x801:
        return regs.vbr;
    case 0x002:
        return regs.cacr_de << 31 | regs.cacr_ie << 15;
    case 0x803:
        return regs.msp;
    case 0x804:
        return regs.isp;
    case 0x003:
        return mmu.tcr_e << 15 | mmu.tcr_p << 14;
    case 0x004:
        return mmu.ITTR[0].address_base << 24 | mmu.ITTR[0].address_mask << 16 |
               mmu.ITTR[0].E << 15 | mmu.ITTR[0].S << 13 | mmu.ITTR[0].U << 8 |
               mmu.ITTR[0].CM << 5 | mmu.ITTR[0].W << 2;
    case 0x005:
        return mmu.ITTR[1].address_base << 24 | mmu.ITTR[1].address_mask << 16 |
               mmu.ITTR[1].E << 15 | mmu.ITTR[1].S << 13 | mmu.ITTR[1].U << 8 |
               mmu.ITTR[1].CM << 5 | mmu.ITTR[1].W << 2;
    case 0x006:
        return mmu.DTTR[0].address_base << 24 | mmu.DTTR[0].address_mask << 16 |
               mmu.DTTR[0].E << 15 | mmu.DTTR[0].S << 13 | mmu.DTTR[0].U << 8 |
               mmu.DTTR[0].CM << 5 | mmu.DTTR[0].W << 2;
    case 0x007:
        return mmu.DTTR[1].address_base << 24 | mmu.DTTR[1].address_mask << 16 |
               mmu.DTTR[1].E << 15 | mmu.DTTR[1].S << 13 | mmu.DTTR[1].U << 8 |
               mmu.DTTR[1].CM << 5 | mmu.DTTR[1].W << 2;
    case 0x805:
        return mmu.MMUSR.PA | mmu.MMUSR.B << 11 | mmu.MMUSR.G << 10 |
               mmu.MMUSR.U << 8 | mmu.MMUSR.S << 7 | mmu.MMUSR.CM << 5 |
               mmu.MMUSR.M << 4 | mmu.MMUSR.W << 2 | mmu.MMUSR.T << 1 |
               mmu.MMUSR.R;
    case 0x806:
        return mmu.urp;
    case 0x807:
        return mmu.srp;
    default:
        ILLEGAL_INST();
    }
}

void do_op_movec_to(int op, uint32_t v) {
    switch(op) {
    case 0x000:
        regs.sfc = v & 7;
        return;
    case 0x001:
        regs.dfc = v & 7;
        return;
    case 0x800:
        regs.usp = v;
        return;
    case 0x801:
        regs.vbr = v;
        return;
    case 0x002:
        regs.cacr_de = v >> 31 & 1;
        regs.cacr_ie = v >> 15 & 1;
        return;
    case 0x803:
        regs.msp = v;
        return;
    case 0x804:
        regs.isp = v;
        return;
    case 0x003:
        mmu.tcr_e = v >> 15 & 1;
        mmu.tcr_p = v >> 14 & 1;
        return;
    case 0x004:
        mmu.ITTR[0].address_base = v >> 24 & 0xff;
        mmu.ITTR[0].address_mask = v >> 16 & 0xff;
        mmu.ITTR[0].E = v >> 15 & 1;
        mmu.ITTR[0].S = v >> 13 & 3;
        mmu.ITTR[0].U = v >> 8 & 3;
        mmu.ITTR[0].CM = v >> 5 & 3;
        mmu.ITTR[0].W = v >> 2 & 1;
        return;
    case 0x005:
        mmu.ITTR[1].address_base = v >> 24 & 0xff;
        mmu.ITTR[1].address_mask = v >> 16 & 0xff;
        mmu.ITTR[1].E = v >> 15 & 1;
        mmu.ITTR[1].S = v >> 13 & 3;
        mmu.ITTR[1].U = v >> 8 & 3;
        mmu.ITTR[1].CM = v >> 5 & 3;
        mmu.ITTR[1].W = v >> 2 & 1;
        return;
    case 0x006:
        mmu.DTTR[0].address_base = v >> 24 & 0xff;
        mmu.DTTR[0].address_mask = v >> 16 & 0xff;
        mmu.DTTR[0].E = v >> 15 & 1;
        mmu.DTTR[0].S = v >> 13 & 3;
        mmu.DTTR[0].U = v >> 8 & 3;
        mmu.DTTR[0].CM = v >> 5 & 3;
        mmu.DTTR[0].W = v >> 2 & 1;
        return;
    case 0x007:
        mmu.DTTR[1].address_base = v >> 24 & 0xff;
        mmu.DTTR[1].address_mask = v >> 16 & 0xff;
        mmu.DTTR[1].E = v >> 15 & 1;
        mmu.DTTR[1].S = v >> 13 & 3;
        mmu.DTTR[1].U = v >> 8 & 3;
        mmu.DTTR[1].CM = v >> 5 & 3;
        mmu.DTTR[1].W = v >> 2 & 1;
        return;
    case 0x805:
        mmu.MMUSR.PA = v & 0xFFFFF000;
        mmu.MMUSR.B = v >> 11 & 1;
        mmu.MMUSR.G = v >> 10 & 1;
        mmu.MMUSR.U = v >> 8 & 3;
        mmu.MMUSR.S = v >> 7 & 1;
        mmu.MMUSR.CM = v >> 5 & 3;
        mmu.MMUSR.M = v >> 4 & 1;
        mmu.MMUSR.W = v >> 2 & 1;
        mmu.MMUSR.T = v >> 1 & 1;
        mmu.MMUSR.R = v & 1;
        return;
    case 0x806:
        mmu.urp = v;
        return;
    case 0x807:
        mmu.srp = v;
        return;
    default:
        ILLEGAL_INST();
    }
}

void init_mmu() {
    mmu.urp = mmu.srp = 0;
    mmu.tcr_e = mmu.tcr_p = false;
    memset(&mmu.ITTR[0], 0, sizeof(ttc_t));
    memset(&mmu.ITTR[1], 0, sizeof(ttc_t));
    memset(&mmu.DTTR[0], 0, sizeof(ttc_t));
    memset(&mmu.DTTR[1], 0, sizeof(ttc_t));
    memset(&mmu.MMUSR, 0, sizeof(mmusr_t));
}
void init_mmu_opc() {
    opc_map[01721] = op_cinv_push_dc;
    opc_map[01722] = op_cinv_push_ic;
    opc_map[01723] = op_cinv_push_bc;
    opc_map[01724] = op_pflush;
    opc_map[01725] = op_ptest;
}
