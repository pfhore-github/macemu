#include "ncr5380.hpp"
uint8_t Ncr5380::read(int addr) {
	switch(addr) {
	case 0 : return current_data;
	case 1 :
		return RST << 7 | AIP << 6 | LA << 5 | ACK << 4 |
			BSY << 3 | SEL << 2 | ATN << 1 | BUS; // Init cmd
	case 2 : return mode;
	case 3 : return target_cmd;
	case 4 : return scsi_status;
	case 5 : return bus_status;
	case 6 : return input_data;
	case 7 : return irq;
	}
	return 0;
}
void Ncr5380::write(int addr, uint8_t v) {}
	
uint8_t Ncr5380Ext::read(int addr) {
	switch(addr) {
	default : return Ncr5380::read(addr);
	}
}
void Ncr5380Ext::write(int addr, uint8_t v) {
	switch(addr) {
	default : Ncr5380::write(addr, v); break;
	}	
}
	
