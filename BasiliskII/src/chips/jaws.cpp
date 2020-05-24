#include "io.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <assert.h>
#include <atomic>
#include "jaws.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "scc.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "asc.hpp"
#include "scc_impl.hpp"
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
	bool ack;
	friend uint8_t UT_get_JawsVia2(const std::shared_ptr<IO_BASE>& via);
	friend void UT_get_sawsVia2(const std::shared_ptr<IO_BASE>& via, uint8_t v);
	uint8_t val;
protected:	
	bool readA(int n) override {
		ack = true;
		return val >> n & 1;
	}
	bool readB(int n) override {
		switch(n) {
		case 1 : // handshake responce
			return ack;
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
	}
	void writeB(int n, bool v) override {
		switch( n ) {
		case 2 : // write handshake
			ack = v;
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
JAWS::JAWS() {
	via1 = std::make_shared<VIA1>();
	via2 = std::make_shared<JawsVia2>();
	asc = newEASC();
	scc = std::make_shared<SCC>(std::make_shared<Z85C80>());
	scsi = std::make_shared<Ncr5380>();
	pb = std::make_shared<JAWS_REG>();
	pb2 = std::make_shared<JAWS_REG2>();
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
	scc = std::make_shared<SCC_IOP>(std::make_shared<Z85C80>());
	scsi = std::make_shared<Ncr5380>();
	pb = std::make_shared<PB_REG>();
}
std::shared_ptr<IO_BASE> Niagara::get_io(uint32_t base) {
	switch((base>>13)&0x7f) {
	case 0 : return via1;
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
	case 0 : return via1->read(addr >> 9 & 0xf);
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
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 1 : return via2->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 64 : return pb->write(addr, v);
	default : bus_error(addr, attr);
	}
}
