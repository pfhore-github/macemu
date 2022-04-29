#define BOOST_TEST_MODULE mmu_cpu
#define BOOST_TEST_DYN_LINK
#include "SDL.h"
#include "sysdeps.h"
#include <boost/test/unit_test.hpp>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#include "cpu_emulation.h"
#include "main.h"
#include "newcpu.h"
#include "test_common.h"
#include <fmt/core.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
std::vector<uint8_t> RAM;
uint8 *ROMBaseHost;
void init_m68k();

struct MyGlobalFixture {
    MyGlobalFixture() {
        init_m68k();
        RAM.resize(0x100000);
    }

    ~MyGlobalFixture() {}
};

InitFix::InitFix() {
    RAM.clear();
    RAM.resize(0x100000);
    memset(regs.r, 0, sizeof(uint32_t) * 16);
    regs.pc = 0;
    regs.v = regs.c = regs.n = regs.x = regs.z = false;
    regs.S = false;
    memset(&regs.FPCR, 0, sizeof(regs.FPCR));
}
BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);

bool done = false;
uint32_t ROMSize = 0x3fff;
void Exit680x0() { done = true; }
uint8_t raw_read8(uint32_t addr) { return RAM[addr]; }
uint16_t raw_read16(uint32_t addr) {
    auto p = reinterpret_cast<const uint16_t *>(&RAM[addr]);
    return SDL_SwapBE16(*p);
}

uint32_t raw_read32(uint32_t addr) {
    auto p = reinterpret_cast<const uint32_t *>(&RAM[addr]);
    return SDL_SwapBE32(*p);
}
void raw_write8(uint32_t addr, uint8_t v) { RAM[addr] = v; }
void raw_write16(uint32_t addr, uint16_t v) {
    auto p = reinterpret_cast<uint16_t *>(&RAM[addr]);
    *p = SDL_SwapBE16(v);
}
void raw_write32(uint32_t addr, uint32_t v) {
    auto p = reinterpret_cast<uint32_t *>(&RAM[addr]);
    *p = SDL_SwapBE32(v);
}
#include <stdio.h>
#include <unistd.h>

static std::unordered_map<std::string, std::string> asmcodes;
void asm_m68k(const char *a, int offset) {
    auto f = asmcodes.find(a);
    if(f == asmcodes.end()) {
        char tmp[] = "/tmp/asm_ret_XXXXXX";
        int tmps = mkstemp(tmp);
        close(tmps);
        auto p1_f = fmt::format("m68k-linux-gnu-as -mcpu=68040 -o {}", tmp);
        FILE *p1 = popen(p1_f.c_str(), "w");
        fprintf(p1, "%s\n", a);
        if(pclose(p1)) {
            BOOST_FAIL("assemble failure");
        }
        p1_f =
            fmt::format("m68k-linux-gnu-objcopy -O binary {} /dev/stdout", tmp);
        p1 = popen(p1_f.c_str(), "r");
        std::string opc;
        int c;
        while((c = getc(p1)) != -1) {
            opc += c;
        }
        pclose(p1);
        asmcodes[a] = opc;
        unlink(tmp);
        f = asmcodes.find(a);
    }
    const auto &ret = f->second;
    for(char c : ret) {
        raw_write8(offset++, c);
    }
}

void EmulOp(uint16 opcode, M68kRegisters *r) {}

void exception_check(int e)
{
    regs.M = false;
    regs.isp = regs.a[7] = 0x1000;
    regs.vbr = 0x3000;
    raw_write32( 0x3000 + e * 4, 0x5000);
    m68k_do_execute();
    if( e ) {
        BOOST_TEST( regs.S );
        BOOST_TEST( regs.pc == 0x5000 );
        BOOST_TEST( (raw_read16( regs.a[7] + 6) & 0xfff) == e * 4);
    } else {
        BOOST_TEST( regs.pc != 0x5000 );
    }
}