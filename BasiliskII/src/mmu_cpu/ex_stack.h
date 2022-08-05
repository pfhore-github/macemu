#ifndef EX_STACK_H__
#define EX_STACK_H__
#include "newcpu.h"
struct ILLEGAL_INST_EX {};

struct BUS_ERROR_EX {
    uint32_t addr = 0;
    bool cm = false;
    bool ma = false;
    bool atc = true;
    bool lk = false;
    bool rw = false;
    SZ size = SZ::LONG;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
};
#endif