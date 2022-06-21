/*
 *  cpu_emulation.h - Definitions for Basilisk II CPU emulation module (UAE
 * 0.8.10 version)
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

#include "SDL.h"
#include "memory.h"
#include <stdint.h>
#include <string.h>

/*
 *  Memory system
 */

// RAM and ROM pointers (allocated and set by main_*.cpp)
extern uint32_t RAMBaseMac;  // RAM base (Mac address space), does not include
                             // Low Mem when != 0
extern uint8_t *RAMBaseHost; // RAM base (host address space)
extern uint32_t RAMSize;     // Size of RAM

extern uint32_t ROMBaseMac;  // ROM base (Mac address space)
extern uint8_t *ROMBaseHost; // ROM base (host address space)
extern size_t ROMSize;       // Size of ROM

#if !REAL_ADDRESSING && !DIRECT_ADDRESSING
// If we are not using real or direct addressing, the Mac frame buffer gets
// mapped to this location. The memory must be allocated by VideoInit().
// If multiple monitors are used, they must share the frame buffer
const uint32_t MacFrameBaseMac = 0xa0000000;
extern uint8_t *MacFrameBaseHost; // Frame buffer base (host address space)
extern uint32_t MacFrameSize;     // Size of frame buffer
#endif
extern int MacFrameLayout; // Frame buffer layout (see defines below)

// Possible frame buffer layouts
enum {
    FLAYOUT_NONE,     // No frame buffer
    FLAYOUT_DIRECT,   // Frame buffer is in MacOS layout, no conversion needed
    FLAYOUT_HOST_555, // 16 bit, RGB 555, host byte order
    FLAYOUT_HOST_565, // 16 bit, RGB 565, host byte order
    FLAYOUT_HOST_888  // 32 bit, RGB 888, host byte order
};

// Mac memory access functions
static inline uint32_t ReadMacInt32(uint32_t addr) { return read32(addr); }
static inline uint32_t ReadMacInt16(uint32_t addr) { return read16(addr); }
static inline uint32_t ReadMacInt8(uint32_t addr) { return read8(addr); }
static inline void WriteMacInt32(uint32_t addr, uint32_t l) {
    write32(addr, l);
}
static inline void WriteMacInt16(uint32_t addr, uint32_t w) {
    write16(addr, w);
}
static inline void WriteMacInt8(uint32_t addr, uint32_t b) { write8(addr, b); }
static inline uint8_t *Mac2HostAddr(uint32_t addr) {
    return reinterpret_cast<uint8_t *>(&RAM[addr]);
}
static inline uint32_t Host2MacAddr(uint8_t *addr) {
    return addr - reinterpret_cast<uint8_t *>(RAM.data());
}

static inline void *Mac_memset(uint32_t addr, int c, size_t n) {
    return memset(Mac2HostAddr(addr), c, n);
}
static inline void *Mac2Host_memcpy(void *dest, uint32_t src, size_t n) {
    return memcpy(dest, Mac2HostAddr(src), n);
}
static inline void *Host2Mac_memcpy(uint32_t dest, const void *src, size_t n) {
    return memcpy(Mac2HostAddr(dest), src, n);
}
static inline void *Mac2Mac_memcpy(uint32_t dest, uint32_t src, size_t n) {
    return memcpy(Mac2HostAddr(dest), Mac2HostAddr(src), n);
}

/*
 *  680x0 emulation
 */

// Initialization
extern bool Init680x0(void); // This routine may want to look at CPUType/FPUType
                             // to set up the apropriate emulation
extern void Exit680x0(void);
extern void InitFrameBufferMapping(void);

// 680x0 dynamic recompilation activation flag
#if USE_JIT
extern bool UseJIT;
#else
const bool UseJIT = false;
#endif

// 680x0 emulation functions
struct M68kRegisters;
extern void Start680x0(void); // Reset and start 680x0
extern "C" void
Execute68k(uint32_t addr,
           M68kRegisters *r); // Execute 68k code from EMUL_OP routine
extern "C" void
Execute68kTrap(uint16_t trap,
               M68kRegisters *r); // Execute MacOS 68k trap from EMUL_OP routine

// Interrupt functions
extern void TriggerInterrupt(
    void); // Trigger interrupt level 1 (InterruptFlag must be set first)
extern void TriggerNMI(void); // Trigger interrupt level 7

#endif