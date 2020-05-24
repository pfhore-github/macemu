#include "io.hpp"
#include "via.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <atomic>
#include <vector>
#include <cmath>
#include "asc.hpp"
#include "machine.hpp"
#include "SDL_mutex.h"
// internal audio device
constexpr int MAX_BUF = 0x400;
/* original ASC */
class PlaneASC : public ASC {
	int8_t buf[0x800];
	int8_t buf_internal[0x800];
	union {
		struct {
			uint32_t phase;
			uint32_t increment;
		};
		struct {
			uint8_t raw[8];
		};
	} wavetable[4];
public:
	void do_wave(int16_t* dst, int len);
	uint8_t read(int base) override;
	void write(int base, uint8_t v) override;
	PlaneASC();
};
/* EASC */
class EASC : public ASC {
protected:
	uint16_t sample_rate_incr_a = 0;
	uint16_t sample_rate_incr_b = 0;
	uint8_t a_left_vol = 0;
	uint8_t a_right_vol = 0;
	bool a_fifo_ctl = false;
	bool a_irq_ctl = false;
	uint8_t a_cdxa[8];
	uint8_t b_left_vol = 0;
	uint8_t b_right_vol = 0;
	bool b_fifo_ctl = false;
	bool b_irq_ctl = false;
	uint8_t b_cdxa[8];
public:
	uint8_t read(int base) override;
	void write(int base, uint8_t v) override;

	EASC( uint8_t version );
	static void sound_run(void* data, uint8_t* stream, int len);
	void watch() override;
	~EASC() {}
};
/* one buffer ASC */
class V8EASC : public EASC {
public:
	V8EASC(uint8_t version) :EASC(version) {}
	uint8_t read(int base) override;
	void write(int base, uint8_t v) override;
	void do_fifo() override;
	void watch() override;
};
/* full EASC */
class FullEASC : public EASC {
public:
	FullEASC(uint8_t version) :EASC(version) {}
	void do_fifo() override;
	void watch() override;
};

void ASC::watch() {
	if( mode != 1 ) {
		return;
	}
	while(SDL_GetQueuedAudioSize(speaker) ) {
		irq_status |= FIFO_HALF;
		do_irq();
		SDL_Delay(10);
	}
	irq_status |= FIFO_FULL ;
	do_irq();
}

void EASC::watch() {
	if( ! mode )
		return;
	while(SDL_GetQueuedAudioSize(speaker) ) {
		if( a_irq_ctl ) {
			irq_status |= FIFO_A_HALF;
		}
		if( b_irq_ctl ) {
			irq_status |= FIFO_B_HALF;
		}
		if( irq_status ) {
			do_irq();
		}
		SDL_Delay(10);
	}
	irq_status |= FIFO_FULL ;
	do_irq();
}

void V8EASC::watch() {
	if( ! mode )
		return;
	while(SDL_GetQueuedAudioSize(speaker) ) {
		if( a_irq_ctl ) {
			irq_status |= FIFO_A_HALF;
		}
		if( irq_status ) {
			do_irq();
		}
		SDL_Delay(10);
	}
	irq_status |= FIFO_A_FULL | FIFO_A_HALF;
	do_irq();
}

void FullEASC::watch() {
	if( ! mode )
		return;
	while(SDL_GetQueuedAudioSize(speaker) ) {
		if( a_irq_ctl ) {
			irq_status |= FIFO_A_HALF;
		}
		if( b_irq_ctl ) {
			irq_status |= FIFO_B_HALF;
		}
		if( irq_status ) {
			do_irq();
		}
		SDL_Delay(10);
	}
	irq_status |= FIFO_FULL | FIFO_HALF;
	do_irq();
}
int queue_watch(void* p) {
	ASC* asc = static_cast<ASC*>(p);
	while( asc->running ) {
		asc->watch();
		SDL_Delay(10);
	}
	return 0;
}
void ASC::do_irq() {
	if( is_quadra ) {
		cpu.irq(5);
	} else {
		if( auto via2 = std::dynamic_pointer_cast<VIA>(machine->via2) ) {
			via2->do_irq( IRQ_FLAG::CB1 );
		}
	}
}

void ASC::reset_irq() {
	if( auto via2 = std::dynamic_pointer_cast<VIA>(machine->via2) ) {
		via2->reset_irq( IRQ_FLAG::CB1 );
	}
}

void PlaneASC::do_wave(int16_t* bf, int len) {
	for(int i = 0; i < len; ++i ) {
		int32_t mix = 0;
		for(int ch = 0; ch < 4; ++ch ) {
			if( wavetable[ch].increment == 0 ) {
				continue;
			}
			unsigned int phase = (wavetable[ch].phase += wavetable[ch].increment);
			int8_t smpl = buf_internal[ch<<9 | ((phase>>15)&0x1ff) ] ^ 0x80;
			mix += smpl;
		}
		bf[2*i] = mix * volume / 4;
		bf[2*i+1] = mix * volume / 4;
	}
}


void ASC::sound_run(void* data, uint8_t* stream, int len) {
	auto a = dynamic_cast<PlaneASC*>((ASC*)data);
	if( a ) {
		int16_t* bf = reinterpret_cast<int16_t*>(stream);
		a->do_wave(bf, len/4);
	}
}
ASC::ASC(uint8_t version)
	:version(version)
{
	table_write = 0;
	speaker = 0;
	running = true;
	t = SDL_CreateThread(queue_watch, "watcher", this);
}
ASC::~ASC() {
	running = false;
	int q;
	SDL_WaitThread(t, &q);
	stop();
}
void ASC_buf::reset() {
	w = 0;
	s = 0;
	full = false;
	memset(buf, 0, 0x400);
}	
ASC_buf::ASC_buf() { reset(); }
PlaneASC::PlaneASC() :ASC(0) {
	memset(buf, 0, 0x800);
	memset(buf_internal, 0, 0x800);
	SDL_zero(spec);
	spec.freq = 22257;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 0x80;	
	for(int i = 0; i < 4; ++i ) {
		wavetable[i].phase = 0;
		wavetable[i].increment = 0;
	}
}
void ASC::start(int m) {
	if( speaker ) {
		stop();
	}
	mode = m;
	switch( mode ) {
	case 0 : return;		
	case 1 : // FIFO
		spec.userdata = nullptr;
		spec.callback = nullptr;
		speaker = SDL_OpenAudioDevice(nullptr, false, &spec, nullptr, 0);
		SDL_PauseAudioDevice(speaker, true);
		break;
	case 2 : // WAVETABLE
		spec.userdata = this;
		spec.callback = &ASC::sound_run;
		speaker = SDL_OpenAudioDevice(nullptr, false, &spec, nullptr, 0);
		SDL_PauseAudioDevice(speaker, false);
		break;
	}
}
void ASC::stop() {
	if( mode == 1 ) {
		// wait until queue is empty
		while( SDL_GetQueuedAudioSize(speaker) > 0 ) {
			SDL_Delay(10);
		}
		buf_a.reset();
		buf_b.reset();
		SDL_PauseAudioDevice(speaker, true);
		SDL_Delay(10);
	}
	SDL_CloseAudioDevice(speaker);
	speaker = 0;
}
uint8_t ASC::read(int base) {
	switch( base ) {
	case ASC_REG::VERSION: return version;
	case ASC_REG::MODE: return mode;
	case ASC_REG::CONTROL: return control;
	case ASC_REG::FIFO_MODE : return fifo_mode;
	case ASC_REG::FIFO_IRQ_STATUS :
		return irq_status.exchange( 0 );
	case ASC_REG::VOLUME: return volume;
	case ASC_REG::CLOCK: return clock_rate;
	case ASC_REG::REC: return play_rec_a;
	default:
		if( base < 0x838 ) {
			return unknown[base-0x830];
		} 
	}
	return 0;
}
uint8_t PlaneASC::read(int base) {
	if( base < 0x800 ) {
		return buf[base];
	}
	switch( base ) {
	case ASC_REG::WAVETABLE_CTL : return wave_ctl;
	case 0x810:
	case 0x818:
	case 0x820:
	case 0x828:
		return 0;
	case 0x811:
	case 0x819:
	case 0x821:
	case 0x829:
		return wavetable[(base-0x810)>>3].raw[2];
	case 0x812:
	case 0x81A:
	case 0x822:
	case 0x82A:
		return wavetable[(base-0x810)>>3].raw[1];
	case 0x813:
	case 0x81B:
	case 0x823:
	case 0x82B:
		return wavetable[(base-0x810)>>3].raw[0];
	case 0x815:
	case 0x81D:
	case 0x825:
	case 0x82D:
		return wavetable[(base-0x810)>>3].raw[6];
	case 0x816:
	case 0x81E:
	case 0x826:
	case 0x82E:
		return wavetable[(base-0x810)>>3].raw[5];
	case 0x817:
	case 0x81F:
	case 0x827:
	case 0x82F:
		return wavetable[(base-0x810)>>3].raw[4];		
	default: return ASC::read(base);
	}
}
bool ASC_buf::push(int8_t v) {
	buf[ w ] = v;
	if( w < MAX_BUF ) {
		w++;
		s++;
	} 
	full = s == MAX_BUF;
	return full;
}




void ASC::write(int base, uint8_t v) {
	switch( base ) {
	case ASC_REG::FIFO_MODE:
		if( v & 0x80 ) {
			start(mode);
		}
		fifo_mode = v & 0x7f;
		return;
	case ASC_REG::FIFO_IRQ_STATUS :
		return;
	case ASC_REG::VOLUME:
		volume = v;
		return;
	case ASC_REG::CONTROL:
		control = v;
		return;
	case ASC_REG::CLOCK:
		v &= 3;
		switch( v ) {
		case 0 : spec.freq = 22257; break;
		case 2 : spec.freq = 22050; break;
		case 3 : spec.freq = 44100; break;
		default: break;
		}
		start(mode);
		clock_rate = v;
		return;
			
	default:
		if( base < 0x838 ) {
			// TODO:$830-837
			unknown[base-0x830] = v;
		}
	} 		
}
void PlaneASC::write(int base, uint8_t v) {
	if( base < 0x800 ) {
		switch( mode ) {
		case 1 :
			if( base < 0x400 ) {
				buf_a.push( v );
			} else {
				if( buf_b.push( v ) ) {
					irq_status |= FIFO_FULL;
					do_fifo();
				}
			}
			return;
		case 2 :
			buf[base] = v;
			if( ++table_write == 0x800 ) {
				SDL_PauseAudioDevice(speaker, true);
				memcpy(buf_internal, buf, 0x800); 
				table_write = 0;
				SDL_PauseAudioDevice(speaker, false);
			}
			return;
		}
	}
	switch( base ) {
	case ASC_REG::MODE:
		start( v & 3);
		break;
	case ASC_REG::WAVETABLE_CTL :
		/* TODO */
		wave_ctl = v;
		return;
	case 0x810:
	case 0x818:
	case 0x820:
	case 0x828:
	case 0x814:
	case 0x81C:
	case 0x824:
	case 0x82C:
		break;
	case 0x811:
	case 0x819:
	case 0x821:
	case 0x829:
		wavetable[(base-0x810)>>3].raw[2] = v;
		break;
	case 0x812:
	case 0x81A:
	case 0x822:
	case 0x82A:
		wavetable[(base-0x810)>>3].raw[1] = v;
		break;
	case 0x813:
	case 0x81B:
	case 0x823:
	case 0x82B:
		wavetable[(base-0x810)>>3].raw[0] = v;
		break;
	case 0x815:
	case 0x81D:
	case 0x825:
	case 0x82D:
		wavetable[(base-0x810)>>3].raw[6] = v;
		break;
	case 0x816:
	case 0x81E:
	case 0x826:
	case 0x82E:
		wavetable[(base-0x810)>>3].raw[5] = v;
		break;
	case 0x817:
	case 0x81F:
	case 0x827:
	case 0x82F:
		wavetable[(base-0x810)>>3].raw[4] = v;
		break;
	default :
		ASC::write(base, v);
	}
}
static SDL_AudioDeviceID mic;
void open_mic() {
	SDL_AudioSpec mic_spec;
	SDL_zero(mic_spec);
	mic_spec.freq = 22050;
	mic_spec.format = AUDIO_S16;
	mic_spec.channels = 1;
	mic_spec.samples = 4096;
	mic_spec.userdata = nullptr;
	mic_spec.callback = nullptr;
	mic = SDL_OpenAudioDevice(nullptr, 1, &mic_spec, nullptr, 0);
	SDL_PauseAudioDevice(mic, true);
}

EASC::EASC(uint8_t version) :ASC(version) {
	SDL_zero(spec);
	spec.freq = 22255;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 0x200;

	a_cdxa[0] = b_cdxa[0] = 0;
	a_cdxa[1] = b_cdxa[1] = 0;
	a_cdxa[2] = b_cdxa[2] = 0;
	a_cdxa[3] = b_cdxa[3] = 0x3c;
	a_cdxa[4] = b_cdxa[4] = 0xcc;
	a_cdxa[5] = b_cdxa[5] = 0x73;
	a_cdxa[6] = b_cdxa[6] = 0xc9;
	a_cdxa[7] = b_cdxa[7] = 0x62;
	
}
uint8_t EASC::read(int base) {
	switch( base ) {
	case ASC_REG::WAVETABLE_CTL : return 0;
	case ASC_REG::CLOCK: return 3;
	case ASC_REG::REC: return play_rec_a;
	case ASC_REG::A_WRITE: return buf_a.w >> 8;
	case ASC_REG::A_WRITE+1: return buf_a.w & 0xff;
	case ASC_REG::A_READ: return 0;
	case ASC_REG::A_READ+1: return 0;
	case ASC_REG::A_SAMPLE_RATE_INCREMENT: return sample_rate_incr_a >> 8;
	case ASC_REG::A_SAMPLE_RATE_INCREMENT+1: return sample_rate_incr_a & 0xff;
	case ASC_REG::A_LEFT_VOLUME : return a_left_vol;
	case ASC_REG::A_RIGHT_VOLUME : return a_right_vol;
	case ASC_REG::A_FIFO_CTL : return a_fifo_ctl << 7;
	case ASC_REG::A_IRQ_CTL : return a_irq_ctl;
	case ASC_REG::A_CDXA_DECOMP : return a_cdxa[0];
	case ASC_REG::A_CDXA_DECOMP+1 : return a_cdxa[1];
	case ASC_REG::A_CDXA_DECOMP+2 : return a_cdxa[2];
	case ASC_REG::A_CDXA_DECOMP+3 : return a_cdxa[3];
	case ASC_REG::A_CDXA_DECOMP+4 : return a_cdxa[4];
	case ASC_REG::A_CDXA_DECOMP+5 : return a_cdxa[5];
	case ASC_REG::A_CDXA_DECOMP+6 : return a_cdxa[6];
	case ASC_REG::A_CDXA_DECOMP+7 : return a_cdxa[7];
	case ASC_REG::B_WRITE: return buf_b.w >> 8;
	case ASC_REG::B_WRITE+1: return buf_b.w & 0xff;
	case ASC_REG::B_READ: return 0;
	case ASC_REG::B_READ+1: return 0;
	case ASC_REG::B_SAMPLE_RATE_INCREMENT: return sample_rate_incr_b >> 8;
	case ASC_REG::B_SAMPLE_RATE_INCREMENT+1: return sample_rate_incr_b & 0xff;
	case ASC_REG::B_LEFT_VOLUME : return b_left_vol;
	case ASC_REG::B_RIGHT_VOLUME : return b_right_vol;
	case ASC_REG::B_FIFO_CTL : return b_fifo_ctl << 7;
	case ASC_REG::B_IRQ_CTL : return b_irq_ctl;
	case ASC_REG::B_CDXA_DECOMP : return b_cdxa[0];
	case ASC_REG::B_CDXA_DECOMP+1 : return b_cdxa[1];
	case ASC_REG::B_CDXA_DECOMP+2 : return b_cdxa[2];
	case ASC_REG::B_CDXA_DECOMP+3 : return b_cdxa[3];
	case ASC_REG::B_CDXA_DECOMP+4 : return b_cdxa[4];
	case ASC_REG::B_CDXA_DECOMP+5 : return b_cdxa[5];
	case ASC_REG::B_CDXA_DECOMP+6 : return b_cdxa[6];
	case ASC_REG::B_CDXA_DECOMP+7 : return b_cdxa[7];
	default: return ASC::read(base);
	}
}
void EASC::write(int base, uint8_t v) {
	if( base < 0x400 ) {
		buf_a.push( v );
		return;
	} else if( base < 0x800 ) {
		if( buf_b.push( v ) ) {
			irq_status |= FIFO_FULL;
			do_fifo();
		}
		return;
	}
	switch( base ) {
	case ASC_REG::MODE:
		start( v & 1 );
		break;
	case ASC_REG::REC:
		play_rec_a = v;
		return;
	case ASC_REG::A_SAMPLE_RATE_INCREMENT:
		sample_rate_incr_a = (sample_rate_incr_a & 0xff) | v << 8;
		return;
	case ASC_REG::A_SAMPLE_RATE_INCREMENT+1:
		sample_rate_incr_a = (sample_rate_incr_a & 0xff00) | v;
		return;
	case ASC_REG::A_LEFT_VOLUME : a_left_vol = v; return;
	case ASC_REG::A_RIGHT_VOLUME : a_right_vol = v; return;
	case ASC_REG::A_FIFO_CTL : a_fifo_ctl = v >> 7; return;
	case ASC_REG::A_IRQ_CTL : a_irq_ctl = v & 1; return;
	case ASC_REG::A_CDXA_DECOMP : a_cdxa[0] = v; return;
	case ASC_REG::A_CDXA_DECOMP+1 : a_cdxa[1] = v; return;
	case ASC_REG::A_CDXA_DECOMP+2 : a_cdxa[2] = v; return;
	case ASC_REG::A_CDXA_DECOMP+3 : a_cdxa[3] = v; return;
	case ASC_REG::A_CDXA_DECOMP+4 : a_cdxa[4] = v; return;
	case ASC_REG::A_CDXA_DECOMP+5 : a_cdxa[5] = v; return;
	case ASC_REG::A_CDXA_DECOMP+6 : a_cdxa[6] = v; return;
	case ASC_REG::A_CDXA_DECOMP+7 : a_cdxa[7] = v; return;
	case ASC_REG::B_SAMPLE_RATE_INCREMENT:
		sample_rate_incr_b = (sample_rate_incr_b & 0xff) | v << 8;
		return;
	case ASC_REG::B_SAMPLE_RATE_INCREMENT+1:
		sample_rate_incr_b = (sample_rate_incr_b & 0xff00) | v;
		return;
	case ASC_REG::B_LEFT_VOLUME : b_left_vol = v; return;
	case ASC_REG::B_RIGHT_VOLUME : b_right_vol = v; return;
	case ASC_REG::B_FIFO_CTL : b_fifo_ctl = v >> 7; return;
	case ASC_REG::B_IRQ_CTL : b_irq_ctl = v & 1; return;
	case ASC_REG::B_CDXA_DECOMP : b_cdxa[0] = v; return;
	case ASC_REG::B_CDXA_DECOMP+1 : b_cdxa[1] = v; return;
	case ASC_REG::B_CDXA_DECOMP+2 : b_cdxa[2] = v; return;
	case ASC_REG::B_CDXA_DECOMP+3 : b_cdxa[3] = v; return;
	case ASC_REG::B_CDXA_DECOMP+4 : b_cdxa[4] = v; return;
	case ASC_REG::B_CDXA_DECOMP+5 : b_cdxa[5] = v; return;
	case ASC_REG::B_CDXA_DECOMP+6 : b_cdxa[6] = v; return;
	case ASC_REG::B_CDXA_DECOMP+7 : b_cdxa[7] = v; return;
	default: ASC::write( base, v );
	} 		
}

void V8EASC::write(int base, uint8_t v) {
	if( base < 0x400 ) {
		if( buf_a.push( v ) ) {
			irq_status |= FIFO_A_FULL;
			do_fifo();
		}
	} else if( base >= 0x800 ) {
		EASC::write( base, v );		
	} 
}

uint8_t V8EASC::read(int base) {
	switch( base ) {
	case ASC_REG::MODE:
	case ASC_REG::CONTROL:
		return 1;
	default:
		return EASC::read(base);
	}
	return 0;
}

void ASC::do_fifo() {
	int16_t b[0x800];
	for(int i = 0; i < 0x400; ++i ) {
		b[2*i] = int8_t(buf_a.buf[i]^0x80) * volume ;		
		b[2*i+1] = int8_t(buf_b.buf[i]^0x80) * volume;
	}
	buf_a.reset();
	buf_b.reset();
	SDL_PauseAudioDevice(speaker, false);
	SDL_QueueAudio(speaker, b, 0x1000 );
}

// V8 is one buffer 2 output
void V8EASC::do_fifo() {
	int16_t b[0x800];
	for(int i = 0; i < 0x400; ++i ) {		
		int8_t v = buf_a.buf[lround(i)]^0x80;
		b[2*i] = v * volume;
		b[2*i+1] = v * volume;
	}
	buf_a.reset();
	SDL_PauseAudioDevice(speaker, false);
	SDL_QueueAudio(speaker, b, 0x1000 );
}

// TODO left buffer is unused...
void FullEASC::do_fifo() {
	std::vector<int16_t> bf;
	for(double a = 0.0, b = 0.0; a < 0x400 && b < 0x400 ;
		a += sample_rate_incr_a / 32768.0, b += sample_rate_incr_b / 32768.0 ) {
		int8_t v1 = buf_a.buf[lround(a)]^0x80;
		int8_t v2 = buf_b.buf[lround(b)]^0x80;
		bf.push_back( (v1 * a_left_vol + v2 * b_left_vol) * volume / 128 );
		bf.push_back( (v1 * a_right_vol + v2 * b_right_vol) * volume / 128 );
	}
	buf_a.reset();  
	buf_b.reset(); 
	SDL_PauseAudioDevice(speaker, false);
	SDL_QueueAudio(speaker, bf.data(), bf.size() );
}
std::shared_ptr<ASC> newPlaneASC() { return std::make_shared<PlaneASC>(); }
std::shared_ptr<ASC> newEASC(uint8_t version) { return std::make_shared<EASC>(version); }
std::shared_ptr<ASC> newV8EASC(uint8_t version) { return std::make_shared<V8EASC>(version); }
std::shared_ptr<ASC> newSonoraEASC() { return std::make_shared<FullEASC>(0xbc); }
