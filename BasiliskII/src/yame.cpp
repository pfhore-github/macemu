#include "cpu_emulation.h"
#include "memory.h"
#include "newcpu.h"
#include <SDL.h>
extern bool debug;
std::vector<std::byte> RAM;
void m68k_execute();
void reset_via();
void EmulOp(uint16_t opcode, M68kRegisters *r) {}
void reset_all() {
    reset_via();

}
void idle_resume() {}
bool Init680x0();
std::atomic<uint32_t> InterruptFlags;
extern bool rom_overlay;
void quit() {
    SDL_free(ROMBaseHost);
    SDL_Quit();    
}
int main() {
    RAM.resize(64 * 1024 * 1024);
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "fatal error",
                                 SDL_GetError(), nullptr);
        exit(1);
    }
    SDL_StopTextInput();
    atexit(quit);

    // load ROM
    ROMBaseHost = static_cast<uint8_t *>(
        SDL_LoadFile_RW(SDL_RWFromFile("quadra950.ROM", "r"), &ROMSize, 1));

    Init680x0();
    regs.pc = 0x4080002a;
    debug = true;
    m68k_execute();
    SDL_Event e;
    for(;;) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                exit(0);
            }
        }
    }
    return 0;
}