#include "sysdeps.h"
#include <SDL.h>
#include <bit>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include <time.h>
#include <sys/time.h>
#include "via.h"
#include "SDL_timer.h"
static SDL_TimerID vblank, onesec;
static Uint32 vbalnk_irq(Uint32 interval, void *param)
{
    static uint32_t microsec = 0;
    microsec += 630;
    int next = 16;
    if( microsec > 1000) {
        microsec -= 1000;
        next++;
    }
    via1.ca1_in();
    return next;
}
static Uint32 onesec_irq(Uint32 interval, void *param)
{
    via1.ca2_in();
    return 1000;
}
void quit_timer() {
    SDL_RemoveTimer(vblank);
    SDL_RemoveTimer(onesec);
}

void run_timer() {
    vblank = SDL_AddTimer(16, vbalnk_irq, nullptr);
    onesec = SDL_AddTimer(1000, onesec_irq, nullptr);
    atexit(run_timer);
}

uint8_t pram[256];

bool rtcDataIn() { return false; }
void rTCEnb(bool v) {}
void rtcCLK(bool v) {}
void rtcDataOut(bool v) {}