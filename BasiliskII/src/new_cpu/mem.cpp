#include "registers.hpp"
#include "mmu/mmu.hpp"
#include "SDL_endian.h"
#include "../include/machine.hpp"
extern uint8_t *RAMBaseHost;
extern uint8_t *ROMBaseHost;
extern uint32_t RAMSize;
extern uint32_t ROMSize;
void bus_error(uint32_t addr, int attr) {
	uint16_t ssw =
		((attr >> 3) & 0x1f) | // TT&TM
		((attr&3) <<5)  |	 // SIZE
		(ATTR_IS_WRITE(attr) << 8); // RW 
	throw Exception::AccessFault( addr, ssw, ATTR_IS_INST(attr) );
}
template<typename T>
T* ram_addr(uint32_t addr, int attr) {
	return static_cast<T*>(machine->get_ram_addr(addr, attr));
}
template<typename T>
T* rom_addr(uint32_t addr, int attr) {
	return static_cast<T*>(static_cast<void*>(&ROMBaseHost[addr&(ROMSize-1)]));
}

static uint8_t read_ram_b(uint32_t addr, int attr) {
	auto p = ram_addr<uint8_t>(addr, attr);
	return p ? *p : 0;
}
static uint16_t read_ram_w(uint32_t addr, int attr) {
	auto p = ram_addr<uint16_t>(addr, attr);
	return p ? SDL_SwapBE16(*p) : 0;
}
static uint32_t read_ram_l(uint32_t addr, int attr) {
	auto p = ram_addr<uint32_t>(addr, attr);
	return p ? SDL_SwapBE32(*p) : 0;
}
static void write_ram_b(uint32_t addr, uint8_t v, int attr) {
	auto p = ram_addr<uint8_t>(addr, attr);
	if( p ) { *p = v; }
}
static void write_ram_w(uint32_t addr, uint16_t v, int attr) {
	auto p = ram_addr<uint16_t>(addr, attr);
	if( p ) { *p = SDL_SwapBE16(v); }
}
static void write_ram_l(uint32_t addr, uint32_t v, int attr) {
	auto p = ram_addr<uint32_t>(addr, attr);
	if( p ) { *p = SDL_SwapBE32(v); }
}

static uint8_t pread_b(uint32_t addr, int attr) {
	int high = (addr >> 28);
	switch( high ) {
	case 0 :
		// RAM/ROM(init)
		if( machine->rom_mirror ) {
			return *rom_addr<uint8_t>(addr, attr);
		}
		/* fall through */
	case 1 :
	case 2 :
	case 3 : 
		// RAM
		return read_ram_b(addr, attr);
	case 4 :
		// ROM
		machine->rom_mirror = false;
		return *rom_addr<uint8_t>(addr, attr);
	case 5 :
		// IO
		return machine->io_read_b(addr, attr);
	case 9   :
	case 0xA :
	case 0xB :
	case 0xC :
	case 0xD :
	case 0xE :
		// NuBus super
		if( machine->nubus[high-9] && machine->nubus[high-9]->read_b ) {
			return machine->nubus[high-9]->read_b(addr & 0xfffffff);
		} else {
			bus_error(addr, attr);
		}
	case 0xF : {
		// NuBus std
		int slot = addr >> 24 & 0xf;
		if( slot >= 9 && slot <= 0xe && machine->nubus[slot-9] && machine->nubus[slot-9]->read_b ) {
			return machine->nubus[slot-9]->read_b(addr & 0xffffff);
		} else {
			bus_error(addr, attr);
		}
	}
	default : 
		bus_error(addr, attr);
	}
}

static uint16_t pread_w(uint32_t addr, int attr) {
	if( addr & 1 ) {
		uint16_t re = pread_b(addr, attr) << 8;
		try {
			re |= pread_b(addr+1, attr);
		} catch( Exception::AccessFault& e) {
			e.addr = addr;
			e.ssw |= 1 << 11; // MA
			throw e;
		}		
		return re;
	} else {
		int high = (addr >> 28);
		switch( high ) {
		case 0 :
			// RAM/ROM(init)
			if( machine->rom_mirror ) {
				return SDL_SwapBE16(*rom_addr<uint16_t>(addr, attr));
			}
			/* fall through */
		case 1 :
		case 2 :
		case 3 :
			// RAM
			return read_ram_w(addr, attr);
		case 4 :
			// ROM
			machine->rom_mirror = false;
			return SDL_SwapBE16(*rom_addr<uint16_t>(addr, attr));
		case 5 :
			// IO
			return machine->io_read_w(addr, attr);
		case 9   :
		case 0xA :
		case 0xB :
		case 0xC :
		case 0xD :
		case 0xE :
			// NuBus super
			if( machine->nubus[high-9] && machine->nubus[high-9]->read_w ) {
				return machine->nubus[high-9]->read_w(addr & 0xfffffff);
			} else {
				bus_error(addr, attr);
			}
		case 0xF : {
			// NuBus std
			int slot = addr >> 24 & 0xf;
			if( slot >= 9 && slot <= 0xe && machine->nubus[slot-9] && machine->nubus[slot-9]->read_w) {
				return machine->nubus[slot-9]->read_w(addr & 0xffffff);
			} else {
				bus_error(addr, attr);
			}
		}
		default : 
			bus_error(addr, attr);
		}
	}
}

static uint32_t pread_l(uint32_t addr, int attr) {
	if( addr & 1 ) {
		uint32_t re = pread_b(addr, attr) << 24;
		try {
			re |= pread_b(addr+1, attr) << 16;
			re |= pread_b(addr+2, attr) << 8;
			re |= pread_b(addr+3, attr);
		} catch( Exception::AccessFault& e) {
			e.addr = addr;
			e.ssw |= 1 << 11; // MA
			throw e;
		}		
		return re;
	} else if( addr & 2 ) {
		uint32_t re = pread_w(addr, attr) << 16;
		try {
			re |= pread_w(addr+2, attr);
		} catch( Exception::AccessFault& e) {
			e.addr = addr;
			e.ssw |= 1 << 11; // MA
			throw e;
		}		
		return re;
	} else {
		int high = (addr >> 28);
		switch( high ) {
		case 0 :
			// RAM/ROM(init)
			if( machine->rom_mirror ) {
				return SDL_SwapBE32(*rom_addr<uint32_t>(addr, attr));
			}
			/* fall through */
		case 1 :
		case 2 :
		case 3 :
			// RAM
			return read_ram_l(addr, attr);
		case 4 :
			// ROM
			machine->rom_mirror = false;
			return SDL_SwapBE32(*rom_addr<uint32_t>(addr, attr));
		case 5 :
			// IO
			if ( (addr& 0xfffffff) == 0xffffffc ) { 
				// ModelID
				return machine->model_id;
			} else if( (addr & 0xfffffff ) == 0x8000000 ||
					   (addr & 0xfffffff ) == 0xfc0000 ||
					   (addr & 0xfffffff ) == 0xf80000) {
				// unknown header($AAAA5555)
			} else if( (addr & 0xfffffff ) == 0x8000004 ||
					   (addr & 0xfffffff ) == 0xfc0004 ||
					   (addr & 0xfffffff ) == 0xf80004) {
				// unknown address
			}
			return machine->io_read_l(addr, attr);
		case 9   :
		case 0xA :
		case 0xB :
		case 0xC :
		case 0xD :
		case 0xE :
			// NuBus super
			if( machine->nubus[high-9] && machine->nubus[high-9]->read_l ) {
				return machine->nubus[high-9]->read_l(addr & 0xfffffff);
			} else {
				bus_error(addr, attr);
			}
		case 0xF : {
			// NuBus std
			int slot = addr >> 24 & 0xf;
			if( slot >= 9 && slot <= 0xe && machine->nubus[slot-9] && machine->nubus[slot-9]->read_l ) {
				return machine->nubus[slot-9]->read_l(addr & 0xffffff);
			} else {
				bus_error(addr, attr);
			}
		}
		default : 
			bus_error(addr, attr);
		}
	}
}


void bus_read(uint32_t addr, int attr, void* dst) {
	attr &= ~ (1 << 2);
	switch(ATTR_SZ(attr)) {
	case ATTR_SZ_B:
		*(uint8_t*)dst = pread_b( addr, attr );
		break;
	case ATTR_SZ_W:
		*(uint16_t*)dst = pread_w( addr, attr );
		break;
	case ATTR_SZ_L:
		*(uint32_t*)dst = pread_l( addr, attr );
		break;
	case ATTR_SZ_LN:
		if( addr & 15 ) {
			*(uint8_t*)dst = pread_b( addr, attr );
			try {
				for(int i = 1; i < 16; ++ i ) {
					*((uint8_t*)dst+i) = pread_b( addr+i, attr );
				}
			} catch( Exception::AccessFault& e) {
				e.addr = addr;
				e.ssw |= 1 << 11; // MA
				throw e;
			}		
		} else {
			memcpy(dst, ram_addr<void>(addr, attr), 16);
		}
		break;
	}
}

static void pwrite_b(uint32_t addr, int attr, uint8_t b) {
	int high = (addr >> 28);
	switch( high ) {
	case 0 :
		// RAM/ROM(init)
		if( machine->rom_mirror ) {
			bus_error(addr, attr);
		}
		/* fall through */
	case 1 :
	case 2 :
	case 3 :
		// RAM
		write_ram_b(addr, b, attr);
		break;
	case 4 :
		// ROM
		machine->rom_mirror = false;
		bus_error(addr, attr);
		break;
	case 5 :
		// IO
		machine->io_write_b(addr, b, attr);
		break;
	case 9   :
	case 0xA :
	case 0xB :
	case 0xC :
	case 0xD :
	case 0xE :
		// NuBus super
		if( machine->nubus[high-9] && machine->nubus[high-9]->write_b ) {
			machine->nubus[high-9]->write_b(addr & 0xfffffff, b);
			return;
		} else {
			bus_error(addr, attr);
		}
	case 0xF : {
		// NuBus std
		int slot = addr >> 24 & 0xf;
		if( slot >= 9 && slot <= 0xe && machine->nubus[slot-9] && machine->nubus[slot-9]->write_b ) {
			machine->nubus[slot-9]->write_b(addr & 0xffffff, b);
			return;
		} else {
			bus_error(addr, attr);
		}
	}
	default : 
		bus_error(addr, attr);
	}
}

static void pwrite_w(uint32_t addr, int attr, uint16_t w) {
	if( addr & 1 ) {
		pwrite_b(addr, attr, w >> 8);
		try {
			pwrite_b(addr+1, attr, w);
		} catch( Exception::AccessFault& e) {
			e.addr = addr;
			e.ssw |= 1 << 11; // MA
			throw e;
		}		
	} else {
		int high = (addr >> 28);
		switch( high ) {
		case 0 :
			// RAM/ROM(init)
			if( machine->rom_mirror ) {
				bus_error(addr, attr);
			}
			/* fall through */
		case 1 :
		case 2 :
		case 3 :
			// RAM
			write_ram_w(addr, w, attr);
			break;
		case 4 :
			// ROM
			machine->rom_mirror = false;
			bus_error(addr, attr);
			break;
		case 5 :
			// IO
			machine->io_write_w(addr, w, attr);
			break;
		case 9   :
		case 0xA :
		case 0xB :
		case 0xC :
		case 0xD :
		case 0xE :
			// NuBus super
			if( machine->nubus[high-9] && machine->nubus[high-9]->write_w ) {
				machine->nubus[high-9]->write_w(addr & 0xfffffff, w);
				return;
			} else {
				bus_error(addr, attr);
			}
		case 0xF : {
			// NuBus std
			int slot = addr >> 24 & 0xf;
			if( slot >= 9 && slot <= 0xe && machine->nubus[slot-9] && machine->nubus[slot-9]->write_w ) {
				machine->nubus[slot-9]->write_w(addr & 0xffffff, w);
				return;
			} else {
				bus_error(addr, attr);
			}
		}
		default : 
			bus_error(addr, attr);
		}
	}
}

static void pwrite_l(uint32_t addr, int attr, uint32_t l) {
	if( addr & 1 ) {
		pwrite_b(addr, attr, l >> 24);
		try {
			pwrite_b(addr+1, attr, l >> 16);
			pwrite_b(addr+2, attr, l >> 8);
			pwrite_b(addr+3, attr, l);
		} catch( Exception::AccessFault& e) {
			e.addr = addr;
			e.ssw |= 1 << 11; // MA
			throw e;
		}		
	} else if( addr & 2 ) {
		pwrite_w(addr, attr, l >> 16);
		try {
			pwrite_w(addr+2, attr, l);
		} catch( Exception::AccessFault& e) {
			e.addr = addr;
			e.ssw |= 1 << 11; // MA
			throw e;
		}		
	} else {
		int high = (addr >> 28);
		switch( high ) {
		case 0 :
			// RAM/ROM(init)
			if( machine->rom_mirror ) {
				bus_error(addr, attr);
			}
			/* fall through */
		case 1 :
		case 2 :
		case 3 :
			// RAM
			write_ram_l(addr, l, attr);
			break;
		case 4 :
			// ROM
			machine->rom_mirror = false;
			bus_error(addr, attr);
			break;
		case 5 :
			if( addr == 0x5ffffffc ) {
				// ModelID request?
				if( l == 0 || l == ~0u )
					return;
			}
			// IO
			machine->io_write_l(addr, l, attr);
			break;
		case 9   :
		case 0xA :
		case 0xB :
		case 0xC :
		case 0xD :
		case 0xE :
			// NuBus super
			if( machine->nubus[high-9] ) {
				machine->nubus[high-9]->write_l(addr & 0xfffffff, l);
				return;
			} else {
				bus_error(addr, attr);
			}
		case 0xF : {
			// NuBus std
			int slot = addr >> 24 & 0xf;
			if( slot >= 9 && slot <= 0xe && machine->nubus[slot-9] && machine->nubus[slot-9]->write_l ) {
				machine->nubus[slot-9]->write_l(addr & 0xffffff, l);
				return;
			} else {
				bus_error(addr, attr);
			}
		}
		default : 
			bus_error(addr, attr);
		}
	}
}

void bus_write(uint32_t addr, int attr, const void* src) {
	attr |= 1 << 2;
	switch(ATTR_SZ(attr)) {
	case ATTR_SZ_B:
		pwrite_b(addr, attr, *(const uint8_t*)src);
		break;
	case ATTR_SZ_W:
		pwrite_w(addr, attr, *(const uint16_t*)src);
		break;
	case ATTR_SZ_L:
		pwrite_l(addr, attr, *(const uint32_t*)src);
		break;
	case ATTR_SZ_LN:
		if( addr & 15 ) {
			pwrite_b(addr, attr, *(const uint8_t*)src);
			try {
				for(int i = 1; i < 16; ++ i ) {
					pwrite_b(addr + i, attr, *((const uint8_t*)src + i));
				}
			} catch( Exception::AccessFault& e) {
				e.addr = addr;
				e.ssw |= 1 << 11; // MA
				throw e;
			}
		} else {
			memcpy(ram_addr<void>(addr, attr), src, 16);
		}
		break;
	}
}

uint8_t read_b(uint32_t va) {
	uint8_t v;
	int attr = ATTR(cpu.S, false)|ATTR_SZ_B;
	bus_read(mmu->to_real(va, attr), attr, &v);
	return v;
}
uint16_t read_w(uint32_t va) {
	uint16_t v;
	int attr = ATTR(cpu.S, false)|ATTR_SZ_W;
	bus_read(mmu->to_real(va, attr), attr, &v);
	return v;
}
uint32_t read_l(uint32_t va) {
	uint32_t v;
	int attr = ATTR(cpu.S, false)|ATTR_SZ_L;
	bus_read(mmu->to_real(va, attr), attr, &v);
	return v;
}
void read_16(uint32_t va, uint8_t* dst) {
	int attr = ATTR(cpu.S, false)|ATTR_SZ_LN;
	bus_read(mmu->to_real(va, attr), attr, dst);
}

void write_b(uint32_t va, uint8_t b) {
	int attr = ATTR(cpu.S, false)|ATTR_SZ_B;
	bus_write(mmu->to_real(va, attr), attr, &b);
}
void write_w(uint32_t va, uint16_t w) {
	int attr = ATTR(cpu.S, false)|ATTR_SZ_W;
	bus_write(mmu->to_real(va, attr), attr, &w);
}
void write_l(uint32_t va, uint32_t l) {
	int attr = ATTR(cpu.S, false)|ATTR_SZ_L;
	bus_write(mmu->to_real(va, attr), attr, &l);
}
void write_16(uint32_t va, const uint8_t* src) {
	int attr = ATTR(cpu.S, false)|ATTR_SZ_LN;
	bus_write(mmu->to_real(va, attr), attr, src);
}

