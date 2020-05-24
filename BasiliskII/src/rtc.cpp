#include "io.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "../include/xpram.h"
#include "../include/rtc.hpp"
#define RTC_FLG_READ            (1<<7)
#define RTC_FLG_WRITE_PROTECT   (1<<7)
#define RTC_CMD_READ(r)         (RTC_FLG_READ | (r << 2))
#define RTC_CMD_WRITE(r)        (r << 2)
#define RTC_REG_SECONDS_0       0
#define RTC_REG_SECONDS_1       1
#define RTC_REG_SECONDS_2       2
#define RTC_REG_SECONDS_3       3
#define RTC_REG_WRITE_PROTECT   13
// XPRAM_READ: $1011_1aaa_0aaa_aa00
// XPRAM_WRITE: $0011_1aaa_0aaa_aa00
#define RTC_REG_XPRAM           14
#define RTC_CMD_XPRAM_READ      (RTC_CMD_READ(RTC_REG_XPRAM) << 8)
#define RTC_CMD_XPRAM_WRITE     (RTC_CMD_WRITE(RTC_REG_XPRAM) << 8)
#define RTC_CMD_XPRAM_ARG(a)    (((a & 0xE0) << 3) | ((a & 0x1F) << 2))
void CLK_PRAM::send(uint8_t v) {
	switch(i) {
	case -1:
		if( ! rw ) {
			printf("write CMD:%04x[%02x]\n", cmd, v);
			cmd = 0;
			i = 0;
			break;
		}
		/* fall through */
	case 0 :
		rw = v & 0x80;
		v &= 0x7f;
		if( (v & 0x78) == (RTC_REG_XPRAM << 2) ) {
			cmd = v << 8;
			i = 1;
		} else {
			cmd |= v;
			i = -1;
		}
		break;
	case 1 :
		cmd |= v;
		i = -1;
	}
}
uint8_t CLK_PRAM::read() {
	if( (cmd & 0x7800) == 0x3800 ) {
		// XPRAM
		uint8_t reg = (cmd>>2 & 0x1f) | (cmd>>3 & 0xe0);
		printf("read XPRAM@%02x(%02x)\n", reg, XPRAM[reg]);
		cmd = 0;
		i = 0;
		return XPRAM[reg];
	} else {
		printf("read CMD:%04x\n", cmd);
		cmd = 0;
		i = 0;
		return 0xff;
	}
}

void VIA_RTC::write(bool sig) {
	if( i == -1 )
		i = 7;
	v = (v &~ (1 << i)) | (sig<<i);
	if( i == 0 ) {
		cp.send(v);
		i = 8;
	}
}
bool VIA_RTC::read() {
	if( i == -1 ) {
		v = cp.read();
		i = 7;
	}
	return v >> i & 1;
}
void VIA_RTC::clock(bool sig) {
	if( ! old_sig && sig ) {
		--i;
	}
	old_sig = sig;
}
