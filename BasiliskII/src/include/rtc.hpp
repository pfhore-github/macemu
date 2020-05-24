#pragma once
class CLK_PRAM {
	bool rw = false;
	uint16_t cmd = 0;
	int i = 0;
public:
	void send(uint8_t v);
	uint8_t read();
};


class VIA_RTC {
	uint8_t v;
	int8_t i;
	bool old_sig;
	CLK_PRAM cp;
public:
	VIA_RTC() :v(0), i(0), old_sig(false) {}
	/*
	 * send(); CLK=0|BIT, CLK=1|BIT
	 * recv(); CLK=0, CLK=1, (READ)
	 */
	void write(bool sig);
	bool read();
	void clock(bool sig);
};
