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
    struct a_ctl_t {
        bool PA_LATCH;
        bool PB_LATCH;
        unsigned int SR_MODE;
        bool SR_OUT;
        bool T2_CTL;
        bool T1_FREERUN;
        bool T1_PB7;
    } a_ctl;

    struct p_ctl_t {
        bool CA1;
        PCR_MODE CA2;
        bool CB1;
        PCR_MODE CB2;
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

    uint8_t read_reg_b();           // $0000
    uint8_t read_reg_a_handshake(); // $0200
    uint8_t read_dir_b();           // $0400
    uint8_t read_dir_a();           // $0600
    uint8_t read_timer1_l();        // $0800
    uint8_t read_timer1_h();        // $0A00
    uint8_t read_timer1_latch_l();  // $0C00
    uint8_t read_timer1_latch_h();  // $0E00
    uint8_t read_timer2_l();        // $1000
    uint8_t read_timer2_h();        // $1200
    uint8_t read_shift_reg();       // $1400
    uint8_t read_auxilary_cr();     // $1600
    uint8_t read_peripheral_cr();   // $1800
    uint8_t read_irq();             // $1A00
    uint8_t read_irq_enable();      // $1C00
    uint8_t read_reg_a();           // $1E00

    void write_reg_b(uint8_t v);           // $0000
    void write_reg_a_handshake(uint8_t v); // $0200
    void write_dir_b(uint8_t v);           // $0400
    void write_dir_a(uint8_t v);           // $0600
    void write_timer1_l(uint8_t v);        // $0800
    void write_timer1_h(uint8_t v);        // $0A00
    void write_timer1_latch_l(uint8_t v);  // $0C00
    void write_timer1_latch_h(uint8_t v);  // $0E00
    void write_timer2_l(uint8_t v);        // $1000
    void write_timer2_h(uint8_t v);        // $1200
    void write_shift_reg(uint8_t v);       // $1400
    void write_auxilary_cr(uint8_t v);     // $1600
    void write_peripheral_cr(uint8_t v);   // $1800
    void write_irq(uint8_t v);             // $1A00
    void write_irq_enable(uint8_t v);      // $1C00
    void write_reg_a(uint8_t v);           // $1E00
};


extern VIA* via1;
extern VIA* via2;

#endif