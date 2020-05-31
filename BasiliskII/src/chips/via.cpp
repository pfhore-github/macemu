#include "io.hpp"
#include "via.hpp"
#include "registers.hpp"
#include "machine.hpp"
#include <stdint.h>
#include "rbv.hpp"
// pfc / fq < ns / 1000000000
void nanosleep_x(double ns){
	uint64_t left = ns * SDL_GetPerformanceFrequency() / 1000000000;
	if( left > 10000000 ) {
		SDL_Delay(left/10000000*10);
		left %= 1000000;
	}
	uint64_t deadline = SDL_GetPerformanceCounter() + left;
	while( SDL_GetPerformanceCounter() <= deadline ) {
		__builtin_ia32_pause();
	}
}

class via_timer {
	const int c;
	VIA* via;	
	uint16_t base = 0;
	uint16_t begin = 0;
	const bool isTimer1;
	bool pulse = false;
	std::atomic<bool> running = false;
public:
	void latch(uint16_t v, bool repeat);
	uint16_t now() const { return begin - (CLOCK() - base); }
	void stop() { running = false; }
	~via_timer() { stop(); SDL_Delay(1); }
	friend int timer_repeat(void*);
	friend int timer_once(void*);
	void alarm() { via->do_irq(c); }
	via_timer(VIA* v, int c, bool is1) :c(c), via(v), isTimer1(is1) {}
};


int timer_repeat(void* p) {
	via_timer* self = static_cast<via_timer*>(p);
	self->pulse = false;
	while( self->running ) {
		nanosleep_x( self->begin * 1276.552287582 );
		if( self->running ) {			
			self->alarm();
			if( self->isTimer1 && self->via->a_ctl.T1_PB7 ) {
				self->pulse = ! self->pulse;
				self->via->writeB(7, self->pulse);
			}
		}
	}
	return 0;
}
int timer_once(void* p) {
	via_timer* self = static_cast<via_timer*>(p);
	nanosleep_x( self->begin * 1276.552287582 );
	if( self->running   ) {
		if( self->isTimer1 && self->via->a_ctl.T1_PB7) {
			self->via->writeB(7, true);
		}
		self->alarm();
	}
	return 0;
}	
void via_timer::latch(uint16_t t, bool repeat) {
	running = false;
	begin = t;
	base = CLOCK();
	running = true;
	SDL_Thread* timer = SDL_CreateThread(repeat ? timer_repeat : timer_once,
										"CLOCK", this);
	SDL_DetachThread(timer);
}


VIA::VIA(uint8_t irq)
	:timer1(std::make_unique<via_timer>(this, IRQ_FLAG::TIMER1, true)),
	 timer2(std::make_unique<via_timer>(this, IRQ_FLAG::TIMER2, false)),
	 IRQ(irq) {
	reset();
}

void VIA::ca1_in(bool v) {
	if( p_ctl.CA1 ) {
		if( ca1_old || ! v ) {
			ca1_old = v;
			return;
		}
	} else {
		if( !ca1_old || v ) {
			ca1_old = v;
			return;
		}
	}
	ca1_old = v;
	do_irq(IRQ_FLAG::CA1);
	if( ! handshake ) {
		irA = 0;
		for(int i = 0; i < 8; ++i ) {
			irA |= readA(i) << i;
		}
	} else {
		handshake = false;
		ca2_out(true);
	}
}

void VIA::cb1_in(bool v) {
	if( p_ctl.CB1 ) {
		if( cb1_old || ! v ) {
			cb1_old = v;
			return;
		}
	} else {
		if( !cb1_old || v ) {
			cb1_old = v;
			return;
		}
	}
	cb1_old = v;
	do_irq(IRQ_FLAG::CB1);
	if( ! handshake ) {
		irB = 0;
		for(int i = 0; i < 8; ++i ) {
			irB |= readB(i) << i;
		}
	} else {
		handshake = false;
		cb2_out(true);
	}
}


void VIA::do_irq(int i) {
	irq_flg |= i;
	if( irq_enable & i) {
		irq_flg |= 0x80;
		irq_pin();
	}
}

void VIA::reset_irq(int i) {
	irq_flg &= ~i;
}

void VIA::irq_pin() {
	cpu.irq(IRQ);
}



void VIA::set_timer2() {
	if( ! a_ctl.T2_CTL ) {
		timer2->latch( timer2_latch.value, false );
	} else {
		t2_counter = timer2_latch.value;
		t2_running = true;
	}

}

void VIA::ca2_in_push(bool v) {
	switch( p_ctl.CA2 ) {
	case PCR_MODE::INPUT_NEGATIVE :
	case PCR_MODE::INDEPENDENT_NEGATIVE :
		if( ca2_old && ! v ) {
			do_irq( IRQ_FLAG::CA2 );
		}
		break;
	case PCR_MODE::INPUT_POSITIVE :
	case PCR_MODE::INDEPENDENT_POSITIVE :
		if( ! ca2_old && v ) {
			do_irq( IRQ_FLAG::CA2 );
		}
		break;
	default :
		break;
	}
	ca2_old = v;
}

void VIA::cb2_in_push(bool v) {
	sr = sr << 1 | v;
	if( ++sr_c >= 8 ) {
		do_irq( IRQ_FLAG::SREG );
		sr_c = 0;
	}
	switch( p_ctl.CB2 ) {
	case PCR_MODE::INPUT_NEGATIVE :
	case PCR_MODE::INDEPENDENT_NEGATIVE :
		if( cb2_old && ! v ) {
			do_irq( IRQ_FLAG::CB2 );
		}
		break;
	case PCR_MODE::INPUT_POSITIVE :
	case PCR_MODE::INDEPENDENT_POSITIVE :
		if( ! cb2_old && v ) {
			do_irq( IRQ_FLAG::CB2 );
		}
		break;
	default :
		break;
	}
	cb2_old = v;
}

uint8_t VIA::read(int n) {
	uint8_t v = 0;
	switch(n&0xf) {
	case VIA_REG::RB :
		v = orB & dirB;
		if( a_ctl.PB_LATCH ) {
			v |= irB & ~dirB;
		} else {
			for(int i = 0; i < 8; ++i ) {
				if( !(dirB & 1 << i) ) {
					v |= readB(i) << i;
				}
			}
		}
		if( ! p_ctl.CB1 ) {
			irq_flg &= ~(IRQ_FLAG::CB1);
		}
		if( p_ctl.CB2 == PCR_MODE::INPUT_NEGATIVE ||
			p_ctl.CB2 == PCR_MODE::INPUT_POSITIVE ) {
			irq_flg &= ~(IRQ_FLAG::CB2);
		}
		return v;
	case VIA_REG::DDRB : return dirB;
	case VIA_REG::DDRA : return dirA;
	case VIA_REG::TIMER1_L : 
		irq_flg &= ~ IRQ_FLAG::TIMER1;
		return timer1->now() & 0xff;
	case VIA_REG::TIMER1_H : return timer1->now() >> 8;
	case VIA_REG::TIMER1_L_LATCH : return timer1_latch.low;
	case VIA_REG::TIMER1_H_LATCH : return timer1_latch.high;
	case VIA_REG::TIMER2_L :
		irq_flg &= ~ IRQ_FLAG::TIMER2;
		if( a_ctl.T2_CTL ) {
			return t2_counter & 0xff;
		} else {
			return timer2->now() & 0xff;
		}
	case VIA_REG::TIMER2_H :
		if( a_ctl.T2_CTL ) {
			return t2_counter >> 8;
		} else {
			return timer2->now() >> 8;
		}
	case VIA_REG::SR : 
		irq_flg &= ~IRQ_FLAG::SREG;
		return sr;
	case VIA_REG::ACR : return a_ctl.val;
	case VIA_REG::PCR : return p_ctl.val;
	case VIA_REG::IFR : return irq_flg;
	case VIA_REG::IER :
		nanosleep_x(1400);
		return irq_enable | 0x80; /* Apple-VIA's IER always returns high bit */
	case VIA_REG::RA :
		if( a_ctl.PA_LATCH ) {
			v = irA;
		} else {
			for(int i = 0; i < 8; ++i ) {
				v |= readA(i) << i;
			}
		}
		return v;
	case VIA_REG::RA_H :
		if( a_ctl.PA_LATCH ) {
			v = irA;
		} else {
			for(int i = 0; i < 8; ++i ) {
				v |= readA(i) << i;
			}
		}
		irq_flg &= ~(IRQ_FLAG::CA1);
		switch( p_ctl.CA2 ) {
		case PCR_MODE::INPUT_NEGATIVE :
		case PCR_MODE::INPUT_POSITIVE :
			irq_flg &= ~IRQ_FLAG::CA2;
			break;
		case PCR_MODE::HANDSHAKE:
			handshake = true;
			ca2_out(false);
			break;
		case PCR_MODE::PULSE:
			ca2_out(false);
			ca2_out(true);
			break;
		default :
			break;
		}
		return v;

	default: return 0;
	}
}
void VIA::do_writeB(int i, bool v) {
	writeB(i, v);
	if( i == 6 && a_ctl.T2_CTL ) {
		--t2_counter;
		if( t2_counter == 0 && t2_running ) {
			t2_running = false;
			do_irq(IRQ_FLAG::TIMER2);
		}
	}
}
void VIA::write(int n, uint8_t v) {
	switch(n&0xf) {
	case VIA_REG::RB :
		orB = v;
		for(int i = 0; i < 8; ++i ) {
			if( dirB & 1 << i ) {
				do_writeB(i, orB & (1<<i));
			}
		}
		if( ! p_ctl.CB1 ) {
			irq_flg &= ~IRQ_FLAG::CB1;
		}
		switch( p_ctl.CB2 ) {
		case PCR_MODE::INPUT_NEGATIVE:
		case PCR_MODE::INPUT_POSITIVE:
			irq_flg &= ~IRQ_FLAG::CB2;
			return;
		case PCR_MODE::HANDSHAKE:
			handshake = true;
			cb2_out(false);
			return;
		case PCR_MODE::PULSE:
			cb2_out(false);
			cb2_out(true);
			return;
		default:
			return;
		}
	case VIA_REG::DDRB :
		for(int i = 0; i < 8; ++i ) {
			if( !(dirB & 1 << i) && (v & 1 << i) ) {
				do_writeB(i, orB & (1<<i));
			}
		}
		dirB = v;
		return;
	case VIA_REG::DDRA :
		for(int i = 0; i < 8; ++i ) {
			if( !(dirA & 1 << i) && (v & 1 << i) ) {
				this->writeA(i, orA & (1<<i));
			}
		}
		dirA = v;
		return;

	case VIA_REG::TIMER1_H :
		timer1->stop();		
		timer1_latch.high = v;
		irq_flg &= ~ IRQ_FLAG::TIMER1;
		if( a_ctl.T1_PB7 ) {
			writeB(7, false);
		}
		timer1->latch( timer1_latch.value, a_ctl.T1_FREERUN);
		return;
	case VIA_REG::TIMER1_L :
	case VIA_REG::TIMER1_L_LATCH : timer1_latch.low = v; return;
	case VIA_REG::TIMER1_H_LATCH : timer1_latch.high = v; return;
	case VIA_REG::TIMER2_L : timer2_latch.low = v; return;
	case VIA_REG::TIMER2_H :
		timer2->stop();
		timer2_latch.high = v;
		set_timer2();
		irq_flg &= ~ IRQ_FLAG::TIMER2;		
		return;
	case VIA_REG::SR :
		irq_flg &= ~IRQ_FLAG::SREG;
		sr = v;
		if( a_ctl.SR_OUT ) {
			// immidiate shift out everything
			for(int i = 7; i >= 0; --i ) {
				cb2_out(v >> i & 1);
			}
			do_irq(IRQ_FLAG::SREG);
		}
		return;
	case VIA_REG::ACR :
		a_ctl.val = v;
		// stop old timer
		timer1->stop();
		timer2->stop();
		t2_running = false;
		return;
	case VIA_REG::PCR :
		p_ctl.val = v;
		if( p_ctl.CB2 == PCR_MODE::MANUAL_LOW ) {
			cb2_out(false);
		}
		if( p_ctl.CB2 == PCR_MODE::MANUAL_HIGH ) {
			cb2_out(true);
		}
		if( p_ctl.CA2 == PCR_MODE::MANUAL_LOW ) {
			ca2_out(false);
		}
		if( p_ctl.CA2 == PCR_MODE::MANUAL_HIGH ) {
			ca2_out(true);
		}
		
		return;
	case VIA_REG::IFR :	irq_flg = v; return;
	case VIA_REG::IER : 
		if( v & 0x80 ) {
			irq_enable |= (v&0x7f);
		} else {
			irq_enable &= ((~v)&0x7f);
		}
		return;
	case VIA_REG::RA_H :
		orA = v;
		for(int i = 0; i < 8; ++i ) {
			if( dirA & 1 << i ) {
				writeA(i, orA & (1<<i));
			}
		}
		if( ! p_ctl.CA1 ) {
			irq_flg &= ~IRQ_FLAG::CA1;
		}
		switch( p_ctl.CA2 ) {
		case PCR_MODE::INPUT_NEGATIVE :
		case PCR_MODE::INPUT_POSITIVE :
			irq_flg &= ~IRQ_FLAG::CA2;
			return;
		case PCR_MODE::HANDSHAKE:
			handshake = true;
			ca2_out(false);
			return;
		case PCR_MODE::PULSE:
			ca2_out(false);
			ca2_out(true);
			return;
		default :
			return;
		}
	case VIA_REG::RA :
		orA = v;
		for(int i = 0; i < 8; ++i ) {
			if( dirA & 1 << i ) {
				writeA(i, orA & (1<<i));
			}
		}
		if( ! p_ctl.CA1 ) {
			irq_flg &= ~IRQ_FLAG::CA1;
		}
		if( p_ctl.CA2 == PCR_MODE::INPUT_NEGATIVE ||
			p_ctl.CA2 == PCR_MODE::INPUT_POSITIVE ) {
			irq_flg &= ~IRQ_FLAG::CA2;
		}
		break;		
	}
}

void VIA::reset() {
	irA = orA = dirA = 0;
	irB = orB = dirB = 0;
	p_ctl.val = a_ctl.val = 0;
	irq_flg = 0;
	irq_enable = 0x7f;
	handshake = false;
	cb1_old = ca1_old = false;
	cb2_old = ca2_old = false;
}


struct machine_id_table {
	uint16_t mid_reg; // $5ffffffc
	bool pa1;
	bool pa2;
	bool pa4;
	bool pa6;
	bool pb3;
};
using std::make_pair;

VIA::~VIA() {}

void set_machine(const std::string& s) {
}

uint8_t Via_Lite::read(int n) {
	uint8_t v = 0;
	switch(n ) {
	case RBV_REG::RB :
		irq_flg &= ~(IRQ_FLAG::CB1|IRQ_FLAG::CB2);
		for(int i = 0; i < 8; ++i ) {
			v |= (readB(i) << i);
		}
		return v;
	case RBV_REG::EXP : return 0;
	case RBV_REG::RA :
		irq_flg &= ~(IRQ_FLAG::CA1|IRQ_FLAG::CA2);
		for(int i = 0; i < 8; ++i ) {
			v |= (readA(i) << i);
		}
		return v;
	case RBV_REG::IFR : return irq_flg;
	case RBV_REG::IER : return irq_enable | 0x80;
	default: return 0;
	}
}

void Via_Lite::write(int n, uint8_t v) {
	switch(n ) {
	case RBV_REG::RB :
		irq_flg &= ~(IRQ_FLAG::CB1|IRQ_FLAG::CB2);
		for(int i = 0; i < 8; ++i ) {
			writeB(i, v & (1<<i));
		}
		return;
	case RBV_REG::RA :
		irq_flg &= ~(IRQ_FLAG::CA1|IRQ_FLAG::CA2);
		for(int i = 0; i < 8; ++i ) {
			writeA(i, v & (1<<i));
		}
		return;
	case RBV_REG::IFR : irq_flg = v; return;
	case RBV_REG::IER : 
		if( v & 0x80 ) {
			irq_enable |= (v&0x7f);
		} else {
			irq_enable &= ((~v)&0x7f);
		}
		return;
	}
}

bool Via_Lite::readA(int) { return false; }
void Via_Lite::writeA(int, bool) {}
bool Via_Lite::readB(int) { return false; }
void Via_Lite::writeB(int, bool) {}
