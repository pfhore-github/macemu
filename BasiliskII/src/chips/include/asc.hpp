#pragma once
#include <atomic>
#include "SDL_thread.h"
#include "SDL_audio.h"
#include "io.hpp"
#include <deque>
extern bool is_quadra;
/** Apple Sound Chip */
namespace ASC_REG {
enum ASC_REG_t {
	/* Plane: 0, EASC: $B0, SONORA: $BC, V8/SPICE/VASP: $E8 */
	VERSION = 0x800,
	/* disable(0), FIFO(1), WAVE(2;Plane/EASC only) */
	MODE = 0x801,
	CONTROL = 0x802,
	FIFO_MODE = 0x803,
	FIFO_IRQ_STATUS = 0x804,
	WAVETABLE_CTL = 0x805,
	VOLUME = 0x806,
	CLOCK = 0x807,
	REC = 0x80A,
	TEST = 0x80F,
	A_WRITE = 0xF00,
	A_READ = 0xF02,
	A_SAMPLE_RATE_INCREMENT = 0xF04,
	A_LEFT_VOLUME = 0xF06,
	A_RIGHT_VOLUME = 0xF07,
	A_FIFO_CTL = 0xF08,
	A_IRQ_CTL = 0xF09,
	A_CDXA_DECOMP = 0xF10,
	B_WRITE = 0xF20,
	B_READ = 0xF22,
	B_SAMPLE_RATE_INCREMENT = 0xF24,
	B_LEFT_VOLUME = 0xF26,
	B_RIGHT_VOLUME = 0xF27,
	B_FIFO_CTL = 0xF28,
	B_IRQ_CTL = 0xF29,
	B_CDXA_DECOMP = 0xF30,
};
}
enum FIFO_FLG {
	FIFO_A_HALF = 1,
	FIFO_A_FULL = 2,
	FIFO_B_HALF = 4,
	FIFO_B_FULL = 8,
	FIFO_HALF = 5,
	FIFO_FULL = 10
};
struct ASC_buf {
	uint16_t w, s;
	ASC_buf();
	int8_t buf[0x400];
	bool full;
	bool push(int8_t v);
	void reset();
};
class ASC : public IO_BASE {
	SDL_Thread* t;
protected:
	SDL_AudioSpec spec;
	SDL_AudioDeviceID speaker;
	uint32_t table_write;
	ASC_buf buf_a, buf_b;
	std::atomic<bool> running = false;
	std::atomic<uint8_t> mode = 0;
	std::atomic<uint8_t> irq_status = 0;
	uint8_t control;
	uint8_t fifo_mode;
	uint8_t wave_ctl;
	uint8_t volume;
	uint8_t clock_rate;
	uint8_t play_rec_a;
	uint8_t unknown[8];
	const uint8_t version;
	ASC(uint8_t version);
	void start(int mode);
	void stop();
public:
	void do_irq();
	void reset_irq();
	uint8_t read(int base) override ;
	void write(int base, uint8_t v) override;
	virtual void do_fifo();
	virtual ~ASC();
	uint32_t delay;
	virtual void watch();
	friend int queue_watch(void*);
	static void sound_run(void* data, uint8_t* stream, int len);
};

std::shared_ptr<ASC> newPlaneASC();
std::shared_ptr<ASC> newEASC(uint8_t version = 0xb0);
std::shared_ptr<ASC> newV8EASC(uint8_t version);
std::shared_ptr<ASC> newSonoraEASC();
