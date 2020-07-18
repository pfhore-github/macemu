#pragma once
#include "io.hpp"
#include <SDL_timer.h>
#include <SDL_thread.h>
#include <variant>
#include <atomic>
#include <memory>
#include <deque>
#include <SDL_mutex.h>
#include "timer.h"
class SCC;
namespace IRQ_FLAG {
enum IRQ_FLAG_t {
	CA2 = 1,
	CA1 = 1 << 1,
	SREG = 1 << 2,
	CB2 = 1 << 3,
	CB1 = 1 << 4 ,
	TIMER2 = 1 << 5,
	TIMER1 = 1 << 6,
	SET = 1 << 7
};
}
namespace PCR_MODE {
enum PCR_MODE_t : uint32_t {
	INPUT_NEGATIVE = 0,
	INDEPENDENT_NEGATIVE,
	INPUT_POSITIVE,
	INDEPENDENT_POSITIVE,
	HANDSHAKE,
	PULSE,
	MANUAL_LOW,
	MANUAL_HIGH
};
}
namespace VIA_REG {
enum VIA_REG_t {
	RB = 0, /* reg B */
	RA_H, /* reg A with handshake */
	DDRB, /* reg B direction */
	DDRA, /* reg A direction */
	TIMER1_L, /* Timer1 low bit */
	TIMER1_H, /* Timer1 high bit */
	TIMER1_L_LATCH, /* Timer1 low bit latch */
	TIMER1_H_LATCH, /* Timer1 high bit latch */
	TIMER2_L, /* Timer2 low bit */
	TIMER2_H, /* Timer2 high bit */
	SR, /* shift register */
	ACR, /* auxiliary control register */
	PCR, /* peprpheral control register */
	IFR, /* interrupt flag register */
	IER, /* interrupt enable register */
	RA, /* reg A without handshake */
};
}
// 1tick(783.36kHz) = 1.27655 us
// macintosh CA2 is input only
class via_timer;
/** limited MOS 6522 VIA */
class VIA : public IO_BASE {
	friend void TimerInit();
	// clock timer
	std::unique_ptr<via_timer> timer1, timer2;
	bool t2_running;
	uint16_t t2_counter = 0;
	
	const uint8_t IRQ;
	bool ca1_old;
	bool cb1_old;
	bool ca2_old;
	bool cb2_old;
	union {
		struct {
			uint8_t low = 0;
			uint8_t high = 0;
		};
		uint16_t value;
	} timer1_latch, timer2_latch;
	void do_writeB(int i, bool v);

	void set_timer2();
	friend uint32_t timer_exec(uint32_t i, void* p);
protected:
	VIA(uint8_t irq);
	uint8_t irq_flg;
	uint8_t irA;
	uint8_t irB;
	uint8_t orA;
	uint8_t orB;
	uint8_t dirA;
	uint8_t dirB;
	uint8_t sr = 0;
	uint8_t sr_c = 0;
	union {
		uint8_t val;
		struct {
			bool PA_LATCH : 1;
			bool PB_LATCH : 1;
			unsigned int SR_MODE : 2;
			bool SR_OUT : 1;
			bool T2_CTL : 1;
			bool T1_FREERUN : 1;
			bool T1_PB7 : 1;
		};
	} a_ctl;
	union {
		uint8_t val;
		struct {
			bool CA1 : 1;
			unsigned int CA2 : 3;
			bool CB1 : 1;
			unsigned int CB2 : 3;
		};
	} p_ctl;
	uint8_t irq_enable;
	
	// peripheral
	virtual bool readA(int n) { return false; }
	virtual void writeA(int n, bool v) {}
	virtual bool readB(int n) { return false; }
	virtual void writeB(int n, bool v) {}
	virtual void cb2_out(uint8_t v) {} // for VIA1; ADB
	virtual void irq_pin();
	friend class ADB_VIA;
	
public:
	void ca1_in(bool v);		
	void cb1_in(bool v);		
	void ca2_in_push(bool v);	
	void cb2_in_push_byte(uint8_t v); // for ADB   
	void cb2_in_push(bool v);   
	void do_irq(int i);
	void reset_irq(int i);
	uint8_t read(int n) override ;
	void write(int n, uint8_t v) override ;
	void reset();
	virtual ~VIA();
	friend void adb_set_state(int);
};
// limit VIA2
class Via_Lite : public IO_BASE {
	uint8_t ora = 0;
	uint8_t orb = 0;
	uint8_t sr;
	uint8_t irq_enable = 0x7f;
	uint8_t irq_flg = 0;
protected:
	// PortA read
	virtual bool readA(int n);
	// portA write
	virtual void writeA(int n, bool v);

	// PortB read
	virtual bool readB(int n);
	// PortB write
	virtual void writeB(int n, bool v);
public:
	virtual void reset() { irq_enable = 0x7f; irq_flg = 0; }
	uint8_t read(int addr);
	void write(int addr, uint8_t v);
	virtual ~Via_Lite() {}
	   
};
