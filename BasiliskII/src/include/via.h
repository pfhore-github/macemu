#ifndef VIA_H__
#define VIA_H__ 1
#include <SDL.h>
#include <bitset>
enum class ACR_MODE {
    PA_LATCH,
    PB_LATCH,
    SR_OUT = 4,
    T2_PB6,
    T1_CONTINUOUS,
    T1_PB7
};
enum class IRQ_FLAG {
    CA2 = 0,
    CA1,
    SREG,
    CB2,
    CB1,
    TIMER2,
    TIMER1,
    SET,
};
enum class PCR_MODE : uint32_t {
    INPUT_NEGATIVE = 0,
    INDEPENDENT_NEGATIVE,
    INPUT_POSITIVE,
    INDEPENDENT_POSITIVE,
    HANDSHAKE,
    PULSE,
    MANUAL_LOW,
    MANUAL_HIGH
};
enum class VIA_REG {
    RB = 0,         /* reg B */
    RA_H,           /* reg A with handshake */
    DDRB,           /* reg B direction */
    DDRA,           /* reg A direction */
    TIMER1_L,       /* Timer1 low bit */
    TIMER1_H,       /* Timer1 high bit */
    TIMER1_L_LATCH, /* Timer1 low bit latch */
    TIMER1_H_LATCH, /* Timer1 high bit latch */
    TIMER2_L,       /* Timer2 low bit */
    TIMER2_H,       /* Timer2 high bit */
    SR,             /* shift register */
    ACR,            /* auxiliary control register */
    PCR,            /* peprpheral control register */
    IFR,            /* interrupt flag register */
    IER,            /* interrupt enable register */
    RA,             /* reg A without handshake */
};
// 1tick(783.36kHz) = 1.27655 us
// macintosh CA2 is input only
class via_timer;
/** limited MOS 6522 VIA */
class VIA {
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
    uint16_t timer1_latch, timer2_latch;
    void do_writeB(int i, bool v);

    void set_timer2();
    friend uint32_t timer_exec(uint32_t i, void *p);

  protected:
    std::bitset<8> irq_flg;
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
            PCR_MODE CA2 : 3;
            bool CB1 : 1;
            PCR_MODE CB2 : 3;
        };
    } p_ctl;
    std::bitset<8> irq_enable;

    // peripheral
    virtual bool readA(int n) { return false; }
    virtual void writeA(int n, bool v) {}
    virtual bool readB(int n) { return false; }
    virtual void writeB(int n, bool v) {}
    virtual void cb2_out(uint8_t v) {} // for VIA1; ADB
    virtual void irq_pin();
    friend class ADB_VIA;

  public:
    VIA(uint8_t irq);

    void ca1_in(bool v);
    void cb1_in(bool v);
    void ca2_in_push(bool v);
    void cb2_in_push_byte(uint8_t v); // for ADB
    void cb2_in_push(bool v);
    void do_irq(IRQ_FLAG i);
    void reset_irq(IRQ_FLAG i);
    uint8_t read(int n);
    void write(int n, uint8_t v);
    void reset();
    virtual ~VIA();
    friend void adb_set_state(int);
};

class VIA1 : public VIA {
  public:
    VIA1() : VIA(1) {}
    bool readA(int n) override;
    void writeA(int n, bool v) override;
    bool readB(int n) override;
    void writeB(int n, bool v) override;
};

#endif