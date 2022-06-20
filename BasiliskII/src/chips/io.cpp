#include "sysdeps.h"
#include <SDL.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include "via.h"
#include "via.h"
extern VIA *via1;
extern VIA *via2;
//extern SCC_IOP* scc;
void mcu_ctl_write(uint32_t addr, uint32_t v);
uint32_t mcu_ctl_read(uint32_t addr);
uint8_t mcu_scsi_r(uint32_t addr);
uint8_t readIO8(uint32_t addr) {
    switch((addr >> 13) & 0x1f) {
    case 0:
        // VIA1
        return via1->read(addr >> 9 & 0xf);
    case 1:
        // VIA 2
        return via2->read(addr >> 9 & 0xf);
    case 4:
    // Ethernet PRIME
    case 5:
    // Ethernet
    case 6:
    // IOP for SCC
    case 7:
        // MCU/SCSI
        if((addr & 0x1fff) < 0x1000) {
            // MCU ctl
            return mcu_ctl_read((addr & 0xfff) >> 2);
        } else if((addr & 0xffff) < 0x1400) {
            // intenral SCSI
            return mcu_scsi_r(addr>>4 & 0xf);
        } else if((addr & 0xffff) < 0x1800) {
            // extenral SCSI
            return mcu_scsi_r(addr>>4 & 0xf);
        }
    case 10:
    // EASC
    case 15:
    // IOP: SWIM & ADB
    case 20:
        //        YANCC ctl
        break;
    }
    throw BUS_ERROR_EX{};
}
uint32_t special_rom = 0;
uint16_t readIO16(uint32_t addr) { throw BUS_ERROR_EX{}; }
uint32_t readIO32(uint32_t addr) {
    if(addr == 0x5ffffffc) {
        return 0;
    } else if(addr == 0x58000000 && special_rom) {
        return 0xaaaa5555;
    } else if(addr == 0x58000004 && special_rom) {
        return special_rom;
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        // VIA1
        break;
    case 1:
        // VIA 2
        break;
    case 4:
    // Ethernet PRIME
    case 5:
    // Ethernet
    case 6:
    // IOP for SC
    case 7:
        // MCU/SCSI
        if((addr & 0x1fff) < 0x1000) {
            // MCU ctl
            return mcu_ctl_read((addr & 0xfff) >> 2);
        } else if((addr & 0xffff) < 0x1400) {
            // intenral SCSI
        } else if((addr & 0xffff) < 0x1800) {
            // extenral SCSI
        }
    case 10:
    // EASC
    case 15:
    // IOP: SWIM & ADB
    case 20:
        //        YANCC ctl
        break;
    }
    throw BUS_ERROR_EX{};
}
void writeIO8(uint32_t addr, uint8_t v) {
    switch((addr >> 13) & 0x1f) {
    case 0:
        // VIA1
        via1->write(addr >> 9 & 0xf, v);
        break;
    case 1:
        // VIA 2
        via2->write(addr >> 9 & 0xf, v);
        break;
    case 4:
    // Ethernet PRIME
    case 5:
    // Ethernet
    case 6:
    // IOP for SC
    case 7:
        // MCU/SCSI

    case 10:
    // EASC
    case 15:
    // IOP: SWIM & ADB
    case 20:
        //        YANCC ctl
        break;
    default:
        throw BUS_ERROR_EX{};
    }
}
void writeIO16(uint32_t addr, uint16_t v) { throw BUS_ERROR_EX{}; }
void writeIO32(uint32_t addr, uint32_t v) {
    switch((addr >> 13) & 0x1f) {
    case 0:
        // VIA1
        break;
    case 1:
        // VIA 2
        break;
    case 4:
    // Ethernet PRIME
    case 5:
    // Ethernet
    case 6:
    // IOP for SC
    case 7:
        // MCU/SCSI
        if((addr & 0x1fff) < 0x1000) {
            // MCU ctl
            mcu_ctl_write((addr & 0xfff) >> 2, v);
        } else if((addr & 0xffff) < 0x1400) {
            // intenral SCSI
        } else if((addr & 0xffff) < 0x1800) {
            // extenral SCSI
        }
    case 10:
    // EASC
    case 15:
    // IOP: SWIM & ADB
    case 20:
        //        YANCC ctl
        break;
    default:
        throw BUS_ERROR_EX{};
    }
}