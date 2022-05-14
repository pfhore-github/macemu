#include "sysdeps.h"
#include <SDL.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
uint32_t mcu_ctl_d[0x1000 >> 2];
void mcu_ctl_write(uint32_t addr, uint32_t v) { mcu_ctl_d[addr] = v; }
uint32_t mcu_ctl_read(uint32_t addr) {
    return mcu_ctl_d[addr];
}