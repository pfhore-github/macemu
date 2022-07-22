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
#include "chips/via.h"

void adb_send_reset() {
    // xxxx-0000
    via1.cb2_in_byte(0);
}

void adb_flush() {}
void sendKbdEvent(const SDL_KeyboardEvent& e) {
//    via1.cb2_in();
}

void sendMouseMotionEvent(const SDL_MouseMotionEvent& e) {
    
}

void sendMouseButtonEvent(const SDL_MouseButtonEvent& e) {
    
}

void sendMouseWheelEvent(const SDL_MouseWheelEvent& e) {
    
}