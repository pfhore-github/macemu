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
int asm_m68k(const char* a, int offset) {
  char temp1[] = "/tmp/asm_ret1XXXXXX";
  int i1 = mkstemp(temp1);
  close(i1);
  std::string r = fmt::format("m68k-linux-gnu-as -mcpu=68040 -o {}", temp1);
  FILE* asm_p = popen(r.c_str(), "w");
  fprintf(asm_p, "%s\n", a);
  pclose(asm_p);
  r = fmt::format("m68k-linux-gnu-objcopy -O binary {} /dev/stdout", temp1);
  asm_p = popen(r.c_str(), "r");
  int c, sz = 0;
  while(( c = fgetc(asm_p)) != EOF) {
    raw_write8(offset++, c);
    sz++;
  } 
  pclose(asm_p);
  unlink(temp1);
  return sz;
}