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
bool enabled;
bool overrun;
SDL_AudioDeviceID spkrDev = 0;
SDL_AudioDeviceID micDev;
uint8_t fifo_a[0x400];
int fifo_a_ptr;
uint8_t fifo_b[0x400];
int fifo_b_ptr;
uint8_t last_snd_a, last_snd_b;
uint8_t left_a_vol, right_a_vol, left_b_vol, right_b_vol;
uint8_t m_vol = 7;
std::pair<int16_t, int16_t> mix(int8_t a, int8_t b) {
    return {(a * left_a_vol + b * left_b_vol) * m_vol / 7,
            (a * right_a_vol + b * right_b_vol) * m_vol / 7};
}
uint8_t read_easc(int addr) {
    if(addr < 0x400) {
        // TODO: FIFO A READ
        return 0;
    } else if(addr < 0x800) {
        // TODO: FIFO B READ
        return 0;
    }
    switch(addr) {
    case 0x800:
        return 0xB0;
    case 0x801:
        return enabled;
    case 0x802:
        return overrun << 7;
    case 0x806:
        return m_vol << 5;
    case 0x807:
        return 3;
    }
    return 0;
}

static void audioCallback(void *, Uint8 *stream, int len) {
    auto p = reinterpret_cast<int16_t *>(stream);
    for(int i = 0; i < len; i += 2) {
        std::tie(p[i], p[i + 1]) = mix(last_snd_a, last_snd_b);
    }
}
static SDL_AudioSpec spkrSpec{
    .freq = 44100,
    .format = AUDIO_S16,
    .channels = 2,
    .silence = 0,
    .samples = 0x400,
    .padding = 0,
    .size = 0,
    .callback = nullptr,
    .userdata = nullptr,
};
void enable_audio() {
    spkrSpec.callback = nullptr;
    SDL_CloseAudioDevice(spkrDev);
    spkrDev = SDL_OpenAudioDevice(nullptr, false, &spkrSpec, nullptr, 0);
}
void disable_audio() {
    spkrSpec.callback = audioCallback;
    SDL_CloseAudioDevice(spkrDev);
    spkrDev = SDL_OpenAudioDevice(nullptr, false, &spkrSpec, nullptr, 0);
}

void write_easc(int addr, uint8_t v) {
    if(addr < 0x400) {
        fifo_a[fifo_a_ptr++] = last_snd_a = v;
        if(fifo_a_ptr == 0x400) {
            fifo_a_ptr = 0;
        }
    } else if(addr < 0x800) {
        fifo_b[fifo_b_ptr++] = last_snd_b = v;
        if(fifo_b_ptr == 0x400) {
            fifo_b_ptr = 0;
            // transfer
            static int16_t bx[0x800];
            for(int i = 0; i < 0x400; ++i) {
                std::tie(bx[2 * i], bx[2 * i + 1]) = mix(fifo_a[i], fifo_b[i]);
            }
            SDL_QueueAudio(spkrDev, bx, 0x800 * sizeof(int16_t));
        }
    } else {
        switch(addr) {
        case 0x801:
            if(v == 0) {
                disable_audio();
            } else if(v == 1) {
                enable_audio();
            }
            break;
        case 0x806:
            m_vol = v >> 5;
            break;
        }
    }
}
