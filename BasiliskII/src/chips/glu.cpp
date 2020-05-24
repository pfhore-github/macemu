#include "io.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <assert.h>
#include <atomic>
#include "glu.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "scc.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "asc.hpp"
#include "scc_impl.hpp"
GLU::GLU() {
	via1 = std::make_shared<VIA1>();
	auto adb_via_bus = std::make_shared<ADB_VIA>();
	adb_bus = adb_via_bus;
	via2 = std::make_shared<VIA2>();
	asc = newPlaneASC();
	scc = std::make_shared<SCC>(std::make_shared<Z8530>());
	scsi = std::make_shared<Ncr5380>();
	// connect VIA1 <-> ADB
	via1->adb_bus = adb_via_bus;
	adb_via_bus->via = via1;		

}
std::shared_ptr<IO_BASE> GLU::get_io(uint32_t base) {
	switch((base>>13) & 0xf) {
	case 0  : return via1;
	case 1  : return via2;
	case 2  : return scc;
	case 8  : return scsi;
	case 10 : return asc;
	default : return {};
	}
}

uint8_t GLU::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0xf) {
	case 0  : return via1->read(addr>>9&0xf);
	case 1  : return via2->read(addr>>9&0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	default : bus_error(addr, attr); return 0;
	}
}
#include <stdio.h>
void GLU::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0xf) {
	case 0  : return via1->write(addr>>9&0xf, v);
	case 1  : return via2->write(addr>>9&0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	default :	bus_error(addr, attr);
	}
}
	
