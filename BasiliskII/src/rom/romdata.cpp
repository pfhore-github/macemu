#include "./romdata.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include "sysdeps.h"
#include <SDL.h>
namespace ROM {
  void check_glue();
  void check_mdu();
  void check_oss();
  void check_v8();
  void check_mcu();
  void check_jaws();
}
namespace constant {
const MOTHERBOARD MOTHERDATA[6] = {
    // JAWS
    {0x4080360C,
     0x401F3F,
     0x10032,
     0,
     0,
     0,
     0,
     ROM::check_jaws,
     9,
     {0x40800000, 0x58000000, 0x50f00000, 0x50f04000, 0x50f04000, 0x50F16000, 0,
      0,          0x50F10000, 0x50F06000, 0x50F06000, 0x50F02000, 0x50F14000, 0,
      0,          0,          0,          0,          0,          0,          0,
      0,          0x50F80000, 0,          0,          0,          0}},
    // MCU
    {0x40803568,
     0x07A3181F,
     0,
     0,
     0xC0,
     0,
     0,
     ROM::check_mcu,
     8,
     {0x40800000, 0x58000000, 0x50f00000, 0x50f0C020, 0x50f0C020, 0x50F1E000, 0,
      0,          0,          0,          0,          0x50F02000, 0x50F14000, 0,
      0xF9800000, 0,          0x50F1E020, 0x50F0C020, 0,          0,          0,
      0x5000E000, 0,          0x50F0A000, 0x50F0F000, 0x50F0F402, 0xF9800000}},
    // GLUE
    {0x408032D8,
     0x00001F3F,
     0,
     0x10,
     0,
     0xc0,
     0,
     ROM::check_glue,
     4,
     {0x40800000, 0x58000000, 0x50f00000, 0x50f04000, 0x50f04000, 0x50F16000, 0,
      0,          0x50f10000, 0x50f12000, 0x50f06000, 0x50F02000, 0x50F14000, 0,
      0,          0,          0,          0,          0,          0,          0,
      0,          0,          0,          0,          0,          0}

    },
    // MDU
    {0x4080337C,
     0x0003FF3F,
     0,
     0,
     0,
     0,
     0,
     ROM::check_mdu,
     5,
     {0x40800000, 0x58000000, 0x50f00000, 0x50f04000, 0x50f04000, 0x50F16000,
      0,          0,          0x50f10000, 0x50f12000, 0x50f06000, 0x50F02000,
      0x50F14000, 0x50F26000, 0x50F24000, 0x50F18000, 0x50F1E020, 0x50F0C020,
      0,          0,          0,          0,          0,          0,
      0,          0,          0}},
    // OSS
    {0x40803420,
     0x001F9007,
     0x00000004,
     0,
     0,
     0,
     0,
     ROM::check_oss,
     6,
     {0x40800000, 0x58000000, 0x50f00000, 0,          0,          0,
      0,          0,          0,          0,          0,          0,
      0x50F10000, 0,          0,          0x50F08000, 0x50F12020, 0x50F04020,
      0x50F1A000, 0x5F1C000,  0x50F1E000, 0,          0,          0,
      0,          0,          0,          0,          0,          0}},

    // V8
    {0x408034C4,
     0x0000773f,
     0x000000a6,
     0,
     0,
     0,
     0,
     ROM::check_v8,
     7,
     {0x00A00000, 0x50F80000, 0x50f00000, 0x50F04000, 0x50F04000,
      0x50f16000, 0x50f10000, 0x50f12000, 0x50f06000, 0,
      0x50f14000, 0x50f26000, 0x50f24000, 0,          0,
      0,          0,          0,          0,          0,
      0,          0,          0,          0,          0}}};
enum class MDATA { JAWS = 0, MCU, GLUE, MDU, OSS, V8 };
const MACHINE MACHINEDATA[16] = {
    // SE/30
    {0x408036BC, &MOTHERDATA[int(MDATA::GLUE)], nullptr, nullptr, 0, 0x3CB8,
     0xDC00, 3, 4, 0x3fff, 1, 0, 0x00001f3f, 0, 0x40000008, 0x40000000, nullptr,
     nullptr},
    // IIcx
    {0x408036FC, &MOTHERDATA[int(MDATA::GLUE)], nullptr, nullptr, 0, 0x3CC8,
     0xDC00, 2, 4, 0x3fff, 1, 0, 0x00001f3f, 0, 0x40000008, 0x40000008, nullptr,
     nullptr},
    // IIci
    {0x4080373C, &MOTHERDATA[int(MDATA::MDU)], nullptr, nullptr, 0x3BE8, 0x3CD8,
     0xDC00, 5, 5, 0x3fff, 1, 0, 0x0000773f, 0, 0x56000000, 0x46000000, nullptr,
     nullptr},
    // IIci+PGC
    {0x4080377C, &MOTHERDATA[int(MDATA::MDU)], nullptr, nullptr, 0x3BE8, 0x3CD8,
     0xDC00, 5, 5, 0x3fff, 1, 0, 0x0000773f, 1, 0x56000000, 0x56000000, nullptr,
     nullptr},
    // IIfx
    {0x408037BC, &MOTHERDATA[int(MDATA::OSS)], nullptr, nullptr, 0, 0x3CA8, 0xDC00,
     7, 6, 0x3fff, 2, 0, 0, 4, 0x56000000, 0x52000000, nullptr, nullptr},
    // IIsi
    {0x408037FC, &MOTHERDATA[int(MDATA::MDU)], nullptr, nullptr, 0x3BF8, 0x3CF8,
     0xDC00, 0x0C, 5, 0x3fff, 4, 0, 0x773F, 0x126, 0x56000000, 0x16000000,
     nullptr, nullptr},
    // LC
    {0x4080383C, &MOTHERDATA[int(MDATA::V8)], nullptr, nullptr, 0x3C08, 0x3D08,
     0xDC00, 0x0D, 7, 0x3fff, 4, 0, 0x773F, 0x1A6, 0x56000000, 0x54000000,
     nullptr, nullptr},
    // Q700
    {0x408038FC, &MOTHERDATA[int(MDATA::MCU)], nullptr, nullptr, 0x3C38, 0x3D28,
     0xDC00, 0x10, 8, 0x3fff, 2, 0, 0x05A0183F, 0x00000900, 0x56000000,
     0x40000000, nullptr, nullptr},
    // Q900
    {0x4080387C, &MOTHERDATA[int(MDATA::MCU)], nullptr, nullptr, 0x3C38, 0x3D18,
     0xDC00, 0x0E, 8, 0x3fff, 2, 0, 0x07A3180F, 0x00040924, 0x56000000,
     0x50000000, nullptr, nullptr},
    // Q950
    {0x408038BC, &MOTHERDATA[int(MDATA::MCU)], nullptr, nullptr, 0x3C38, 0x3D18,
     0xDC00, 0x14, 8, 0x3fff, 2, 0, 0x07A3180F, 0x00040924, 0x56000000,
     0x10000000, nullptr, nullptr},
    // PB170
    {0x4080393C, &MOTHERDATA[int(MDATA::JAWS)], nullptr, nullptr, 0x3C28, 0x3D38,
     0xDD00, 0x0F, 9, 0x3fff, 4, 0, 0x00401F3F, 0x00010932, 0x56000000,
     0x12000000, nullptr, nullptr},
    // Any GLUE
    {0x4080397C, &MOTHERDATA[int(MDATA::GLUE)], nullptr, nullptr, 0, 0x3CA8, 0xDC00,
     0xFD, 4, 0x3fff, 1, 0, 0x00001f3f, 0, 0, 0, nullptr, nullptr},
    // Any MDU
    {0x408039BC, &MOTHERDATA[int(MDATA::MDU)], nullptr, nullptr, 0x3BE8, 0x3CA8,
     0xDC00, 0xFD, 5, 0x3fff, 1, 0, 0, 0, 0, 0, nullptr, nullptr},
    // Any OSS
    {0x408039FC, &MOTHERDATA[int(MDATA::OSS)], nullptr, nullptr, 0, 0x3CA8, 0xDC00,
     0xFD, 6, 0x3fff, 2, 0, 0, 4, 0, 0, nullptr, nullptr},
    // Any V8
    {0x40803A3C, &MOTHERDATA[int(MDATA::V8)], nullptr, nullptr, 0x3C08, 0x3D08,
     0xDC00, 0xFD, 7, 0x7fff, 4, 0, 0, 0, 0, 0, nullptr, nullptr},
    // PB170
    {0x40803A7C, &MOTHERDATA[int(MDATA::JAWS)], nullptr, nullptr, 0x3C28, 0x3D38,
     0xDD00, 0xFD, 9, 0x3fff, 2, 0, 0, 0x00010032, 0, 0, nullptr, nullptr},

};
} // namespace constant