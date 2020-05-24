#pragma once
#define DEFINE_ROM(addr) ROM::rom_routines[0x##addr] = ROMWrapper::run_##addr
