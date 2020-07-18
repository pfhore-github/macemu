#include "io.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <assert.h>
#include <atomic>
#include "jaws.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "scc.hpp"
#include "z8530.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "asc.hpp"
#include "msc.hpp"
#include "powerbook.hpp"
void QuitEmulator();
uint8_t JAWS_REG::read(int addr) {
	return 0;
}

void JAWS_REG::write(int addr, uint8_t v) {
}
uint8_t JAWS_REG2::read(int addr) {
	return 0;
}

void JAWS_REG2::write(int addr, uint8_t v) {
}

class JawsVia2 : public VIA2 {
	friend uint8_t UT_get_JawsVia2(const std::shared_ptr<IO_BASE>& via);
	friend void UT_get_sawsVia2(const std::shared_ptr<IO_BASE>& via, uint8_t v);
	uint8_t val = 0;
	bool tran_mode = false;
	bool ready;
protected:	
	bool readA(int n) override {
		return val >> n & 1;
	}
	bool readB(int n) override {
		switch(n) {
		case PB_TRANS_READY : // transport complete
			return ready;
		default:
			return false;
		}
	}
	void writeA(int n, bool v) override {
		if( v ) {
			val |= 1 << n;
		} else {
			val &= ~(1<<n);
		}
		if( n == 7 ) {
			machine->pb_ex->c_in = val;
		}
	}
	void writeB(int n, bool v) override {
		switch( n ) {
		case PB_TRANS_READ_MODE : // transport mode
			if( v ) {
				if( ! std::exchange(tran_mode, v) ) {
					// read
					if( auto v2 = machine->pb_ex->pop_out() ) {
						val = *v2;
					}
				} 
				ready = true;
			} else {
				tran_mode = v;
				ready = false;
				// write
				if( machine->pb_ex->c_in ) {
					machine->pb_ex->cmd( *machine->pb_ex->c_in ) ;
					machine->pb_ex->c_in = {};
				}
			}
			break;
		default:
			break;
		}
	}

};
uint8_t UT_get_JawsVia2(const std::shared_ptr<IO_BASE>& via) {
	auto real_via = std::dynamic_pointer_cast<JawsVia2>( via );
	if( ! real_via ) {
		return 0;
	}
	return real_via->val;
}
void UT_get_sawsVia2(const std::shared_ptr<IO_BASE>& via, uint8_t v) {
	auto real_via = std::dynamic_pointer_cast<JawsVia2>( via );
	if( real_via ) {
		real_via->val = v;
	}
}
JAWS::JAWS(bool is_pb170) {
	via1 = std::make_shared<VIA1>();
	via2 = std::make_shared<JawsVia2>();
	asc = newEASC();
	scc = newZ8530();
	scsi = std::make_shared<Ncr5380>();
	pb = std::make_shared<JAWS_REG>();
	pb2 = std::make_shared<JAWS_REG2>();
	pb_ex = std::make_shared<PB_EX_REG>();

	if( is_pb170 ) {
		model_map[0] = true;
		model_map[1] = false;
		model_map[2] = true;
		model_map[3] = false;
	}

}
std::shared_ptr<IO_BASE> JAWS::get_io(uint32_t base) {
	switch((base>>13) & 0x7f) {
	case 0 : return via1;
	case 1 : return via2;
	case 2 : return scc;
	case 8 : return scsi;
	case 10 : return asc;
	case 64 : return pb;
	case 90 : return pb2;
	default : return {};
	}
}
uint8_t JAWS::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x7f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 1 : return via2->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0x1fff);
	case 64 : return pb->read(addr&0x1fff);
	case 90 : return pb2->read(addr&0x1fff);
	default : bus_error(addr, attr);
	}
}
void JAWS::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x7f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 1 : return via2->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0x1fff, v);
	case 64 : return pb->write(addr&0x1ffff, v);
	case 72 : return pb->write(addr&0x1ffff, v);
	case 73 : return pb->write(addr&0x1ffff, v);
	case 74 : return pb->write(addr&0x1ffff, v);
	case 90 : return pb2->write(addr&0x1fff, v);
	default : bus_error(addr, attr);
	}
}

Niagara::Niagara() {
	auto via1_ = std::make_shared<VIA1>(1);
	via1 = via1_;
	via2 = std::make_shared<VIA2>();
	asc = newEASC(0xbb);
	scc = newZ85C80( true );
	scsi = std::make_shared<Ncr5380>();
	pb = std::make_shared<PB_REG>();
	pb_ex = std::make_shared<PB_EX_REG>();
	// PB 180
	model_map[0] = true;
	model_map[1] = false;
	model_map[2] = true;
	model_map[3] = false;

}
std::shared_ptr<IO_BASE> Niagara::get_io(uint32_t base) {
	switch((base>>13)&0x7f) {
	case 0 :
		if( ((base>>13) & 0x700) != 0x700 ) {
			return {};
		} else {
			return via1;
		}
	case 1 : return via2;
	case 2 : return scc;
	case 8 : return scsi;
	case 10 : return asc;
	case 64 : return pb;
	default : return {};
	}
}
uint8_t Niagara::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x7f) {
	case 0 :
		if( ((addr>>13) & 0x700) != 0x700 ) {
			bus_error(addr, attr);
			return 0;
		} else {
			return via1->read(addr >> 9 & 0xf);
		}		
	case 1 : return via2->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 64 : return pb->read(addr);
	default : bus_error(addr, attr);
	}
}
void Niagara::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x7f) {
	case 0 :
		if( ((addr>>13) & 0x700) != 0x700 ) {
			return bus_error(addr, attr);
		} else {
			return via1->write(addr >> 9 & 0xf, v);
		}
	case 1 : return via2->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 64 : return pb->write(addr, v);
	default : bus_error(addr, attr);
	}
}
