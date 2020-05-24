#pragma once
#include "adb.h"
#include "rtc.hpp"
#include "via.hpp"
enum class VIA2_PORT_A {
	IRQ_9,
	IRQ_A,
	IRQ_B,
	IRQ_C,
	IRQ_D,
	IRQ_E,
	RAM_LOW,
	IRQ_VIDEO = RAM_LOW,
	RAM_HIGH
};
namespace VIA2_PORT_B {
enum VIA2_PORT_B_t {
	CACHE_CTL = 0,
	LOCK_NUBUS,
	POWER_OFF,
	MODE32,
	RBV_CACHE = MODE32,
	NUBUS_TFR1,
	NUBUS_TFR0,
	SND_JCK,
	VBL
};
}
class VIA2 : public VIA {
	bool sjack;
protected:
	bool readA(int n) override;
	bool readB(int n) override;
	void writeA(int n, bool v) override; 
	void writeB(int n, bool v) override;
public:
	VIA2(int i = 2) :VIA(i), sjack(false) {}
};
