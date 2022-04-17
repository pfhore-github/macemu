#define BOOST_TEST_MODULE mmu_cpu
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "SDL.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include "sysdeps.h"

#include "cpu_emulation.h"
#include "main.h"

#include "newcpu.h"
#include "fpu/fpu.h"
#include "test_common.h"
std::vector<uint8_t> RAM;
uint8* ROMBaseHost;
void init_m68k ();

struct MyGlobalFixture {
  MyGlobalFixture() {
    init_m68k();
    RAM.resize(0x100000);
  }
 
  ~MyGlobalFixture() {
  }
};

InitFix::InitFix() {
    RAM.clear();
    RAM.resize(0x100000);
    memset(regs.r, 0, sizeof(uint32_t)*16);
    regs.pc = 0;
}
BOOST_TEST_GLOBAL_FIXTURE( MyGlobalFixture );
#include <stdio.h>
#include <fmt/core.h>
#include <sys/types.h>
#include <unistd.h>


uint8_t raw_read8(uint32_t addr) {
  return RAM[addr];
}
uint16_t raw_read16(uint32_t addr) {
  auto p = reinterpret_cast<const uint16_t*>( &RAM[addr]);
	return SDL_SwapBE16(*p);
}

uint32_t raw_read32(uint32_t addr) {
  auto p = reinterpret_cast<const uint32_t*>( &RAM[addr]);
	return SDL_SwapBE32(*p);
}
void raw_write8(uint32_t addr, uint8_t v) {
  RAM[addr] = v;
}
void raw_write16(uint32_t addr, uint16_t v) {
  auto p = reinterpret_cast<uint16_t*>( &RAM[addr]);
  *p = SDL_SwapBE16(v);
}
void raw_write32(uint32_t addr, uint32_t v) {
  auto p = reinterpret_cast<uint32_t*>( &RAM[addr]);
  *p = SDL_SwapBE32(v);
}
extern std::unordered_map<std::string, std::vector<int>> asmcodes;
int asm_m68k(const char* a, int offset) {
  auto f = asmcodes.find(a);
  if( f == asmcodes.end() ) {
    BOOST_FAIL("no test opcodes");
    return 0;
  }
  const auto& ret = f->second;
  int size = 0;
  for( uint16_t v : ret ) {
    raw_write16(offset, v);
    offset += 2;
    size += 2;
  } 
  return size;
}

