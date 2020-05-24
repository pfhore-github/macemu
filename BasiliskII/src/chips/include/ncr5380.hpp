#pragma once
#include "io.hpp"
class Ncr5380 : public IO_BASE {
protected:
	uint8_t current_data;
	bool RST, AIP, LA, ACK, BSY, SEL, ATN, BUS;
	uint8_t mode;
	uint8_t target_cmd;
	uint8_t scsi_status = 4;
	uint8_t bus_status;
	uint8_t input_data;
	uint8_t irq;
public:
	virtual uint8_t read(int);
	virtual void write(int, uint8_t);
};
/* NCR 5380+(with "true DMA") */
class Ncr5380Ext : public Ncr5380 {
public:
	uint8_t read(int) override;
	void write(int, uint8_t) override;
};
