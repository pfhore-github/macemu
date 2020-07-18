#pragma once
#include "adb.h"
#include "devices/adb.hpp"
#include "rtc.hpp"
#include "via.hpp"
#include "machine.hpp"
namespace VIA1_PORT_A {
enum {
	UNUSED = 0,
	CPU_ID0,
	CPU_ID1,
	vSYNC,
	CPU_ID2,
	OVERLAY = CPU_ID2,
	SWIM_HEAD_SEL,
	CPU_ID3,
	REV8 = CPU_ID3,
	SCC_WRITE	
};
}
namespace VIA1_PORT_B {
enum {
	RTC_DATA,
	RTC_CLK,
	RTC_ENABLE,
	ADB_INT,
	ADB_STATE0,
	ADB_STATE1,
	IICI_PARITY_ENABLE,
	SE30_VSYNC = IICI_PARITY_ENABLE,
	SOUND_ENABLE
};
}
class ADB_VIA;
class VIA1 : public VIA {
	// reg A
	bool sync;
	bool fd_head;
	uint8_t last_cmd;
	// reg B
	VIA_RTC rtc;
	bool rtc_enable;
	// SR
	uint8_t adb_v = 0;
	uint8_t adb_state = 0;
	void adb_set_state(int b);
protected:
	bool readA(int n) override;
	bool readB(int n) override;
	void writeA(int n, bool v) override; 
	void writeB(int n, bool v) override;
	void cb2_out(uint8_t v) override;
public:
	uint8_t read(int n) override ;
	std::unique_ptr<ADBController> adb;
	std::atomic<bool> adb_int;
	std::atomic<bool> scc_wr_req;
	VIA1(int i = 1);
};

