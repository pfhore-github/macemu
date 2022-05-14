#include "sysdeps.h"
#include <SDL.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include "via.h"
VIA via2(2);
static inline uint64_t CLOCK() {
    return SDL_GetPerformanceCounter() * 7833600 /
           SDL_GetPerformanceFrequency();
}
class via_timer {
    const IRQ_FLAG c;
    VIA *via;
    SDL_TimerID timer;
    uint16_t base = 0;
    uint16_t begin = 0;
    const bool isTimer1;
    bool pulse = false;
    bool repeat;

  public:
    friend uint32_t timer_exec(uint32_t i, void *p);
    void latch(uint16_t v, bool repeat);
    uint16_t now() const { return begin - (CLOCK() - base); }
    void stop() { SDL_RemoveTimer(timer); }
    ~via_timer() {
        stop();
        SDL_Delay(0);
    }
    void alarm() { via->do_irq(c); }
    via_timer(VIA *v, IRQ_FLAG c, bool is1) : c(c), via(v), isTimer1(is1) {}
};

uint32_t timer_exec(uint32_t i, void *p) {
    via_timer *self = static_cast<via_timer *>(p);
    self->alarm();
    if(self->isTimer1 && self->via->a_ctl.T1_PB7) {
        self->via->writeB(7, std::exchange(self->pulse, !self->pulse));
    }
    if(self->repeat) {
        self->base = CLOCK();
        return i;
    } else {
        return 0;
    }
}

void via_timer::latch(uint16_t t, bool rep) {
    stop();
    begin = t;
    base = CLOCK();
    repeat = rep;
    pulse = true;
    timer = SDL_AddTimer(begin * 1.2766 / 1000.0, timer_exec, this);
}

VIA::VIA(uint8_t irq)
    : timer1(std::make_unique<via_timer>(this, IRQ_FLAG::TIMER1, true)),
      timer2(std::make_unique<via_timer>(this, IRQ_FLAG::TIMER2, false)),
      IRQ(irq) {
    reset();
}

void VIA::ca1_in(bool v) {
    bool old = std::exchange(ca1_old, v);
    if(p_ctl.CA1) {
        if(old || !v) {
            return;
        }
    } else {
        if(!old || v) {
            return;
        }
    }
    do_irq(IRQ_FLAG::CA1);
    irA = 0;
    for(int i = 0; i < 8; ++i) {
        irA |= readA(i) << i;
    }
}

void VIA::cb1_in(bool v) {
    bool old = std::exchange(cb1_old, v);
    if(p_ctl.CB1) {
        if(old || !v) {
            return;
        }
    } else {
        if(!old || v) {
            return;
        }
    }
    do_irq(IRQ_FLAG::CB1);
    irB = 0;
    for(int i = 0; i < 8; ++i) {
        irB |= readB(i) << i;
    }
}

void VIA::do_irq(IRQ_FLAG i) {
    irq_flg.set(int(i));
    if(irq_enable.test(int(i))) {
        irq_flg |= 0x80;
        irq_pin();
    }
}

void VIA::reset_irq(IRQ_FLAG i) { irq_flg.reset(int(i)); }

void VIA::irq_pin() { regs.irq |= 1 << IRQ; }

void VIA::set_timer2() {
    if(!a_ctl.T2_CTL) {
        timer2->latch(timer2_latch, false);
    } else {
        timer2->stop();
        t2_counter = timer2_latch;
        t2_running = true;
    }
}

void VIA::ca2_in_push(bool v) {
    bool old = std::exchange(ca2_old, v);
    switch(PCR_MODE(p_ctl.CA2)) {
    case PCR_MODE::INPUT_NEGATIVE:
    case PCR_MODE::INDEPENDENT_NEGATIVE:
        if(old && !v) {
            do_irq(IRQ_FLAG::CA2);
        }
        break;
    case PCR_MODE::INPUT_POSITIVE:
    case PCR_MODE::INDEPENDENT_POSITIVE:
        if(!old && v) {
            do_irq(IRQ_FLAG::CA2);
        }
        break;
    default:
        break;
    }
}

void VIA::cb2_in_push_byte(uint8_t v) {
    for(int i = 0; i < 8; ++i) {
        cb2_in_push(v >> (7 - i) & 1);
    }
}
void VIA::cb2_in_push(bool v) {
    sr = sr << 1 | v;
    if(++sr_c == 8) {
        sr_c = 0;
        do_irq(IRQ_FLAG::SREG);
    }
    bool old = std::exchange(cb2_old, v);
    switch(PCR_MODE(p_ctl.CB2)) {
    case PCR_MODE::INPUT_NEGATIVE:
    case PCR_MODE::INDEPENDENT_NEGATIVE:
        if(old && !v) {
            do_irq(IRQ_FLAG::CB2);
        }
        break;
    case PCR_MODE::INPUT_POSITIVE:
    case PCR_MODE::INDEPENDENT_POSITIVE:
        if(!old && v) {
            do_irq(IRQ_FLAG::CB2);
        }
        break;
    default:
        break;
    }
}

static void little_wait() {
    uint64_t deadline =
        SDL_GetPerformanceCounter() + SDL_GetPerformanceFrequency() / 750000;
    while(SDL_GetPerformanceCounter() <= deadline) {
        __builtin_ia32_pause();
    }
}

uint8_t VIA::read(int n) {
    uint8_t v = 0;
    switch(VIA_REG(n & 0xf)) {
    case VIA_REG::RB:
        v = orB & dirB;
        if(a_ctl.PB_LATCH) {
            v |= irB & ~dirB;
        } else {
            for(int i = 0; i < 8; ++i) {
                if(!(dirB & 1 << i)) {
                    v |= readB(i) << i;
                }
            }
        }
        if(!p_ctl.CB1) {
            irq_flg.reset(int(IRQ_FLAG::CB1));
        }
        if(p_ctl.CB2 == PCR_MODE::INPUT_NEGATIVE ||
           p_ctl.CB2 == PCR_MODE::INPUT_POSITIVE) {
            irq_flg.reset(int(IRQ_FLAG::CB2));
        }
        return v;
    case VIA_REG::DDRB:
        return dirB;
    case VIA_REG::DDRA:
        return dirA;
    case VIA_REG::TIMER1_L:
        irq_flg.reset(int(IRQ_FLAG::TIMER1));
        return timer1->now() & 0xff;
    case VIA_REG::TIMER1_H:
        return timer1->now() >> 8;
    case VIA_REG::TIMER1_L_LATCH:
        return timer1_latch & 0xff;
    case VIA_REG::TIMER1_H_LATCH:
        return timer1_latch >> 8;
    case VIA_REG::TIMER2_L:
        irq_flg.reset(int(IRQ_FLAG::TIMER2));
        if(a_ctl.T2_CTL) {
            return t2_counter & 0xff;
        } else {
            return timer2->now() & 0xff;
        }
    case VIA_REG::TIMER2_H:
        if(a_ctl.T2_CTL) {
            return t2_counter >> 8;
        } else {
            return timer2->now() >> 8;
        }
    case VIA_REG::SR:
        irq_flg.reset(int(IRQ_FLAG::SREG));
        return sr;
    case VIA_REG::ACR:
        return a_ctl.val;
    case VIA_REG::PCR:
        return p_ctl.val;
    case VIA_REG::IFR:
        return irq_flg.to_ulong();
    case VIA_REG::IER:
        little_wait();
        return irq_enable.to_ulong() |
               0x80; /* Apple-VIA's IER always returns high bit */
    case VIA_REG::RA:
        if(a_ctl.PA_LATCH) {
            v = irA;
        } else {
            for(int i = 0; i < 8; ++i) {
                v |= readA(i) << i;
            }
        }
        return v;
    case VIA_REG::RA_H:
        if(a_ctl.PA_LATCH) {
            v = irA;
        } else {
            for(int i = 0; i < 8; ++i) {
                v |= readA(i) << i;
            }
        }
        irq_flg.reset(int(IRQ_FLAG::CA1));
        switch(p_ctl.CA2) {
        case PCR_MODE::INPUT_NEGATIVE:
        case PCR_MODE::INPUT_POSITIVE:
            irq_flg.reset(int(IRQ_FLAG::CA2));
            break;
        case PCR_MODE::HANDSHAKE:
        case PCR_MODE::PULSE:
            // not implemented
            break;
        default:
            break;
        }
        return v;

    default:
        return 0;
    }
}
void VIA::do_writeB(int i, bool v) {
    writeB(i, v);
    if(i == 6 && a_ctl.T2_CTL) {
        --t2_counter;
        if(t2_counter == 0 && t2_running) {
            t2_running = false;
            do_irq(IRQ_FLAG::TIMER2);
        }
    }
}
void VIA::write(int n, uint8_t v) {
    switch(VIA_REG(n & 0xf)) {
    case VIA_REG::RB:
        orB = v;
        for(int i = 0; i < 8; ++i) {
            if(dirB & 1 << i) {
                do_writeB(i, orB & (1 << i));
            }
        }
        if(!p_ctl.CB1) {
            irq_flg.reset(int(IRQ_FLAG::CB1));
        }
        switch(p_ctl.CB2) {
        case PCR_MODE::INPUT_NEGATIVE:
        case PCR_MODE::INPUT_POSITIVE:
            irq_flg.reset(int(IRQ_FLAG::CB2));
            return;
        case PCR_MODE::HANDSHAKE:
        case PCR_MODE::PULSE:
            // not implemented
            return;
        default:
            return;
        }
    case VIA_REG::DDRB:
        for(int i = 0; i < 8; ++i) {
            if(!(dirB & 1 << i) && (v & 1 << i)) {
                do_writeB(i, orB & (1 << i));
            }
        }
        dirB = v;
        return;
    case VIA_REG::DDRA:
        for(int i = 0; i < 8; ++i) {
            if(!(dirA & 1 << i) && (v & 1 << i)) {
                this->writeA(i, orA & (1 << i));
            }
        }
        dirA = v;
        return;

    case VIA_REG::TIMER1_H:
        timer1->stop();
        timer1_latch = (timer1_latch & 0xff) | (v << 8);
        irq_flg.reset(int(IRQ_FLAG::TIMER1));
        if(a_ctl.T1_PB7) {
            writeB(7, false);
        }
        timer1->latch(timer1_latch, a_ctl.T1_FREERUN);
        return;
    case VIA_REG::TIMER1_L:
    case VIA_REG::TIMER1_L_LATCH:
        timer1_latch = (timer1_latch & 0xff00) | v;
        return;
    case VIA_REG::TIMER1_H_LATCH:
        timer1_latch = (timer1_latch & 0xff) | (v << 8);
        return;
    case VIA_REG::TIMER2_L:
        timer2_latch = (timer2_latch & 0xff00) | v;
        return;
    case VIA_REG::TIMER2_H:
        timer2->stop();
        timer2_latch = (timer2_latch & 0xff) | (v << 8);
        set_timer2();
        irq_flg.reset(int(IRQ_FLAG::TIMER2));
        return;
    case VIA_REG::SR:
        irq_flg.reset(int(IRQ_FLAG::SREG));
        sr = v;
        if(a_ctl.SR_OUT) {
            // immidiate shift out everything
            cb2_out(v);
            do_irq(IRQ_FLAG::SREG);
        }
        return;
    case VIA_REG::ACR:
        a_ctl.val = v;
        // stop old timer
        timer1->stop();
        timer2->stop();
        t2_running = false;
        return;
    case VIA_REG::PCR:
        p_ctl.val = v;
        return;
    case VIA_REG::IFR:
        irq_flg = v;
        return;
    case VIA_REG::IER:
        if(v & 0x80) {
            irq_enable |= (v & 0x7f);
        } else {
            irq_enable &= ((~v) & 0x7f);
        }
        return;
    case VIA_REG::RA_H:
    case VIA_REG::RA:
        orA = v;
        for(int i = 0; i < 8; ++i) {
            if(dirA & 1 << i) {
                writeA(i, orA & (1 << i));
            }
        }
        if(!p_ctl.CA1) {
            irq_flg.reset(int(IRQ_FLAG::CA1));
        }
        switch(p_ctl.CA2) {
        case PCR_MODE::INPUT_NEGATIVE:
        case PCR_MODE::INPUT_POSITIVE:
            irq_flg.reset(int(IRQ_FLAG::CA2));
            return;
        case PCR_MODE::HANDSHAKE:
        case PCR_MODE::PULSE:
            // not implemented
            return;
        default:
            return;
        }
    }
}

void VIA::reset() {
    irA = orA = dirA = 0;
    irB = orB = dirB = 0;
    p_ctl.val = a_ctl.val = 0;
    irq_flg = 0;
    irq_enable = 0x7f;
    cb1_old = ca1_old = false;
    cb2_old = ca2_old = false;
}

VIA::~VIA() {}

bool VIA1::readA(int n) {
    switch(n) {
    case 6:
        return false;
    case 4:
        return true;
    case 2:
        return false;
    case 1:
        return false;
    }
    return false;
}
void VIA1::writeA(int n, bool v) {}
bool VIA1::readB(int n) { return false; }
void VIA1::writeB(int n, bool v) {}

VIA1 via1;