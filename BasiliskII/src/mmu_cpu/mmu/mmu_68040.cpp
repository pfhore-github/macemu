
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
                                false, 0,     false, 0};

constexpr presult MMU_BUS_INALID{false, false, false, false, 0,
                                 false, 0,     false, 0};
mmu_68040_cmn mmu;
mmu_68040 mmu_d, mmu_i;

constexpr atc_entry_t ATC_INVALID{0, false, false, false, 0, false, 0};
// MMU enabled access
presult mmu_68040::test_TTR(const uint32_t addr) {
    uint8_t base = addr >> 12;
    for(int i = 0; i < 2; ++i) {
        if(TTR[i].E &&
           ((base & ~TTR[i].address_mask) ==
            (TTR[i].address_base & ~TTR[i].address_mask)) &&
           ((TTR[i].S & 2) || (static_cast<bool>(TTR[i].S) == regs.S))) {
            return {true,          true,     TTR[i].W, false, TTR[i].CM,
                    TTR[i].S == 1, TTR[i].U, false,    addr};
        }
    }
    return MMU_BUS_INALID;
}

presult mmu_68040::ptest(const uint32_t addr, bool rw, bool s) {
    uint32_t xaddr = 0;
    uint32_t pgi, atc_key = 0;
    auto atc_cache_g = s ? &atc_cache_g_s : &atc_cache_g_u;
    auto atc_cache_l = s ? &atc_cache_l_s : &atc_cache_l_u;
    bool g = false;
    uint32_t ri = (addr >> 13) << 2;
    uint32_t pi = (addr >> 6 & 0x7f) << 2;
    if(mmu.tcr_p) {
        pgi = ((addr >> 1) & 0x1f) << 2;
        atc_key = addr >> 1;
    } else {
        pgi = (addr & 0x3f) << 2;
        atc_key = addr;
    }
    atc_entry_t *entry = nullptr;
    bool wp = false;
    if(auto gi = atc_cache_g->find(atc_key); gi != atc_cache_g->end()) {
        entry = &gi->second;
        g = true;
    } else if(auto li = atc_cache_l->find(atc_key); li != atc_cache_l->end()) {
        entry = &li->second;
    }
    if(!entry) {
        uint32_t mst = s ? mmu.srp : mmu.urp;
        uint32_t rtd = b_read32(mst | ri);
        if(!(rtd & 2)) {
            goto INVALID;
        }
        if(rtd & 4) {
            wp = true;
        }
        b_write32(mst | ri, rtd |= 8);
        uint32_t ptd_addr = (rtd & ~0x1ff) | pi;
        uint32_t ptd = b_read32(ptd_addr);
        if(!(ptd & 2)) {
            goto INVALID;
        }
        b_write32(ptd_addr, ptd |= 8);
        if(ptd & 4) {
            wp = true;
        }
        uint32_t pd_addr = (ptd & ~(mmu.tcr_p ? 0x3f : 0x7f)) | pgi;
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
            pd >> (mmu.tcr_p ? 13 : 12),
            true,
            wp,
            m,
            uint8_t(pd >> 5 & 3),
            bool(pd & 0x80),
            uint8_t(pd >> 8 & 3),
        };
        g = pd >> 10 & 1;
        if(g) {
            (*atc_cache_g)[atc_key] = ent;
            entry = &(*atc_cache_g)[atc_key];
        } else {
            (*atc_cache_l)[atc_key] = ent;
            entry = &(*atc_cache_l)[atc_key];
        }
    }
    xaddr = entry->addr;
    if(mmu.tcr_p) {
        xaddr = (xaddr << 1) | (addr & 1);
    }
    return {true, false, wp, entry->M, entry->cm, entry->S, entry->U, g, xaddr};

INVALID:
    (*atc_cache_l)[atc_key] = ATC_INVALID;
    return MMU_BUS_INALID;
}
uint32_t mmu_68040::do_mmu(uint32_t vaddr, bool rw, bool s) {
    presult ttr_result = test_TTR(vaddr >> 12);
    if(ttr_result.T) {
        if(ttr_result.W && rw) {
            throw BUS_ERROR_EX{.addr = vaddr, .atc = false};
        }
        return vaddr;
    }
    if(!mmu.tcr_e) {
        return vaddr;
    }
    try {
        presult result = ptest(vaddr >> 12, rw, s);
        if(!result.R) {
            // no entry for the address
            throw BUS_ERROR_EX{.addr = vaddr};
        }
        // Supervisor table
        if(result.S && !s) {
            throw BUS_ERROR_EX{.addr = vaddr};
        }

        // readonly table
        if(result.W && rw) {
            throw BUS_ERROR_EX{.addr = vaddr};
        }

        return (result.addr << 12) | (vaddr & 0xfff);
    } catch(BUS_ERROR_EX &e) {
        // BUS ERROR during table search
        e.addr = vaddr;
        throw e;
    }
}

void mmu_68040_cmn::pflushn_an(uint32_t atc_key) {
    switch(regs.dfc) {
    case 1:
    case 2:
        mmu_d.atc_cache_l_u.erase(atc_key);
        mmu_i.atc_cache_l_u.erase(atc_key);
        break;
    case 5:
    case 6:
        mmu_d.atc_cache_l_s.erase(atc_key);
        mmu_i.atc_cache_l_s.erase(atc_key);
    }
}
void mmu_68040_cmn::pflush_an(uint32_t atc_key) {
    switch(regs.dfc) {
    case 1:
    case 2:
        mmu_d.atc_cache_l_u.erase(atc_key);
        mmu_i.atc_cache_l_u.erase(atc_key);
        mmu_d.atc_cache_g_u.erase(atc_key);
        mmu_i.atc_cache_g_u.erase(atc_key);
        break;
    case 5:
    case 6:
        mmu_d.atc_cache_l_s.erase(atc_key);
        mmu_i.atc_cache_l_s.erase(atc_key);
        mmu_d.atc_cache_g_s.erase(atc_key);
        mmu_i.atc_cache_g_s.erase(atc_key);
        break;
    }
}
void mmu_68040_cmn::pflushn() {
    switch(regs.dfc) {
    case 1:
    case 2:
        mmu_d.atc_cache_l_u.clear();
        mmu_i.atc_cache_l_u.clear();
        break;
    case 5:
    case 6:
        mmu_d.atc_cache_l_s.clear();
        mmu_i.atc_cache_l_s.clear();
        break;
    }
}
void mmu_68040_cmn::pflush() {
    switch(regs.dfc) {
    case 1:
    case 2:
        mmu_d.atc_cache_l_u.clear();
        mmu_i.atc_cache_l_u.clear();
        mmu_d.atc_cache_g_u.clear();
        mmu_i.atc_cache_g_u.clear();
        break;
    case 5:
    case 6:
        mmu_d.atc_cache_l_s.clear();
        mmu_i.atc_cache_l_s.clear();
        mmu_d.atc_cache_g_s.clear();
        mmu_i.atc_cache_g_s.clear();
        break;
    }
}
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];

// CINV( used only for jit)
void op_cinv_push_dc(uint16_t xop, int dm, int type, int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.traced = true;
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

void op_cinv_push_ic(uint16_t xop, int dm, int type, int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.traced = true;
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

void op_cinv_push_bc(uint16_t xop, int dm, int type, int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.traced = true;
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

void op_pflush(uint16_t xop, int dm, int type, int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.traced = true;
    uint32_t addr = regs.a[reg];
    uint32_t atc_key;
    if(mmu.tcr_p) {
        atc_key = addr >> 13;
    } else {
        atc_key = addr >> 12;
    }
    switch(type) {
    case 0: // pflushn (an)
        mmu.pflushn_an(atc_key);
        break;
    case 1: // pflush (an)
        mmu.pflush_an(atc_key);
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

void op_ptest(uint16_t xop, int dm, int type, int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.traced = true;
    uint32_t addr = regs.a[reg] >> 12;
    bool rw;
    if(type == 1) {
        rw = true;
    } else if(type == 5) {
        rw = false;
    } else {
        ILLEGAL_INST();
    }
    presult result;
    switch(regs.dfc) {
    case 1:
    case 5:
        result = mmu_d.test_TTR(addr);
        break;
    case 2:
    case 6:
        result = mmu_i.test_TTR(addr);
        break;
    default:
        ILLEGAL_INST();
    }
    if(result.T) {
        mmu.MMUSR.PA = 0;
        mmu.MMUSR.B = false;
        mmu.MMUSR.G = false;
        mmu.MMUSR.U = 0;
        mmu.MMUSR.S = false;
        mmu.MMUSR.CM = 0;
        mmu.MMUSR.M = false;
        mmu.MMUSR.W = false;
        mmu.MMUSR.T = true;
        mmu.MMUSR.R = true;
        return;
    }
    try {
        switch(regs.dfc) {
        case 1:
            result = mmu_d.ptest(addr, rw, false);
            break;
        case 2:
            result = mmu_i.ptest(addr, rw, false);
            break;
        case 5:
            result = mmu_d.ptest(addr, rw, true);
            break;
        case 6:
            result = mmu_i.ptest(addr, rw, true);
            break;
        }
        mmu.MMUSR.PA = result.addr << 12;
        mmu.MMUSR.B = false;
        mmu.MMUSR.G = result.G;
        mmu.MMUSR.U = result.U;
        mmu.MMUSR.S = result.S;
        mmu.MMUSR.CM = result.CM;
        mmu.MMUSR.M = result.M;
        mmu.MMUSR.W = result.W;
        mmu.MMUSR.T = result.T;
        mmu.MMUSR.R = result.R;
    } catch(BUS_ERROR_EX &) {
        mmu.MMUSR.B = true;
        mmu.MMUSR.PA = 0;
        mmu.MMUSR.G = false;
        mmu.MMUSR.U = 0;
        mmu.MMUSR.S = false;
        mmu.MMUSR.CM = 0;
        mmu.MMUSR.M = false;
        mmu.MMUSR.W = false;
        mmu.MMUSR.T = false;
        mmu.MMUSR.R = false;
    }
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
        return mmu_i.TTR[0].address_base << 24 |
               mmu_i.TTR[0].address_mask << 16 | mmu_i.TTR[0].E << 15 |
               mmu_i.TTR[0].S << 13 | mmu_i.TTR[0].U << 8 |
               mmu_i.TTR[0].CM << 5 | mmu_i.TTR[0].W << 2;
    case 0x005:
        return mmu_i.TTR[1].address_base << 24 |
               mmu_i.TTR[1].address_mask << 16 | mmu_i.TTR[1].E << 15 |
               mmu_i.TTR[1].S << 13 | mmu_i.TTR[1].U << 8 |
               mmu_i.TTR[1].CM << 5 | mmu_i.TTR[1].W << 2;
    case 0x006:
        return mmu_d.TTR[0].address_base << 24 |
               mmu_d.TTR[0].address_mask << 16 | mmu_d.TTR[0].E << 15 |
               mmu_d.TTR[0].S << 13 | mmu_d.TTR[0].U << 8 |
               mmu_d.TTR[0].CM << 5 | mmu_d.TTR[0].W << 2;
    case 0x007:
        return mmu_d.TTR[1].address_base << 24 |
               mmu_d.TTR[1].address_mask << 16 | mmu_d.TTR[1].E << 15 |
               mmu_d.TTR[1].S << 13 | mmu_d.TTR[1].U << 8 |
               mmu_d.TTR[1].CM << 5 | mmu_d.TTR[1].W << 2;
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
        mmu_i.TTR[0].address_base = v >> 24 & 0xff;
        mmu_i.TTR[0].address_mask = v >> 16 & 0xff;
        mmu_i.TTR[0].E = v >> 15 & 1;
        mmu_i.TTR[0].S = v >> 13 & 3;
        mmu_i.TTR[0].U = v >> 8 & 3;
        mmu_i.TTR[0].CM = v >> 5 & 3;
        mmu_i.TTR[0].W = v >> 2 & 1;
        return;
    case 0x005:
        mmu_i.TTR[1].address_base = v >> 24 & 0xff;
        mmu_i.TTR[1].address_mask = v >> 16 & 0xff;
        mmu_i.TTR[1].E = v >> 15 & 1;
        mmu_i.TTR[1].S = v >> 13 & 3;
        mmu_i.TTR[1].U = v >> 8 & 3;
        mmu_i.TTR[1].CM = v >> 5 & 3;
        mmu_i.TTR[1].W = v >> 2 & 1;
        return;
    case 0x006:
        mmu_d.TTR[0].address_base = v >> 24 & 0xff;
        mmu_d.TTR[0].address_mask = v >> 16 & 0xff;
        mmu_d.TTR[0].E = v >> 15 & 1;
        mmu_d.TTR[0].S = v >> 13 & 3;
        mmu_d.TTR[0].U = v >> 8 & 3;
        mmu_d.TTR[0].CM = v >> 5 & 3;
        mmu_d.TTR[0].W = v >> 2 & 1;
        return;
    case 0x007:
        mmu_d.TTR[1].address_base = v >> 24 & 0xff;
        mmu_d.TTR[1].address_mask = v >> 16 & 0xff;
        mmu_d.TTR[1].E = v >> 15 & 1;
        mmu_d.TTR[1].S = v >> 13 & 3;
        mmu_d.TTR[1].U = v >> 8 & 3;
        mmu_d.TTR[1].CM = v >> 5 & 3;
        mmu_d.TTR[1].W = v >> 2 & 1;
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
    memset(&mmu_i.TTR[0], 0, sizeof(ttc_t));
    memset(&mmu_i.TTR[1], 0, sizeof(ttc_t));
    memset(&mmu_d.TTR[0], 0, sizeof(ttc_t));
    memset(&mmu_d.TTR[1], 0, sizeof(ttc_t));
    memset(&mmu.MMUSR, 0, sizeof(mmusr_t));
}
void init_mmu_opc() {
    opc_map[01721] = op_cinv_push_dc;
    opc_map[01722] = op_cinv_push_ic;
    opc_map[01723] = op_cinv_push_bc;
    opc_map[01724] = op_pflush;
    opc_map[01725] = op_ptest;
}
