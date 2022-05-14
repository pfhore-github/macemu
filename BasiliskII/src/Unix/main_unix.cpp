/*
 *  main_unix.cpp - Startup code for Unix
 *
 *  Basilisk II (C) Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "sysdeps.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sstream>

#ifdef USE_SDL
# include <SDL.h>
# include <SDL_main.h>
#endif


#ifdef HAVE_PTHREADS
# include <pthread.h>
#endif

#if REAL_ADDRESSING || DIRECT_ADDRESSING
# include <sys/mman.h>
#endif

#if __MACOSX__
# include "utils_macosx.h"
#endif

#if !EMULATED_68K && defined(__NetBSD__)
# include <m68k/sync_icache.h> 
# include <m68k/frame.h>
# include <sys/param.h>
# include <sys/sysctl.h>
struct sigstate {
	int ss_flags;
	struct frame ss_frame;
	struct fpframe ss_fpstate;
};
# define SS_FPSTATE  0x02
# define SS_USERREGS 0x04
#endif



#include <string>
using std::string;

#include "cpu_emulation.h"
#include "sys.h"
#include "rom_patches.h"
#include "xpram.h"
#include "timer.h"
#include "video.h"
#include "emul_op.h"
#include "prefs.h"
#include "prefs_editor.h"
#include "macos_util.h"
#include "user_strings.h"
#include "version.h"
#include "main.h"
#include <mutex>

#if USE_JIT
#ifdef UPDATE_UAE
extern void (*flush_icache)(void); // from compemu_support.cpp
extern bool UseJIT;
#else
extern void flush_icache_range(uint8 *start, uint32 size); // from compemu_support.cpp
#endif
#endif



// Constants
const char ROM_FILE_NAME[] = "ROM";
const int SCRATCH_MEM_SIZE = 0x10000;	// Size of scratch memory area


// RAM and ROM pointers
std::vector<uint8_t> ROM;
std::vector<std::byte> RAM;

// CPU and FPU type, addressing mode
int CPUType;


// Global variables
#ifndef USE_SDL_VIDEO
extern char *x_display_name;						// X11 display name
extern Display *x_display;							// X11 display handle
#ifdef X11_LOCK_TYPE
X11_LOCK_TYPE x_display_lock = X11_LOCK_INIT;		// X11 display lock
#endif
#endif

static uint8 last_xpram[XPRAM_SIZE];				// Buffer for monitoring XPRAM changes
#include "SDL.h"
#include <thread>
#include <optional>
static std::thread emul_thread;						// Handle of MacOS emulation thread (main thread)

SDL_TimerID one_m, one_s, one_t;
static std::mutex intflag_lock;	// Mutex to protect InterruptFlags
#define LOCK_INTFLAGS intflag_lock.lock()
#define UNLOCK_INTFLAGS intflag_lock.unlock()

// Prototypes
static Uint32 one_minute(Uint32 interval, void *param);
static Uint32 one_second(Uint32 interval, void *param);
static Uint32 one_tick(Uint32 interval, void *param);
#if !EMULATED_68K
static void sigirq_handler(int sig, int code, struct sigcontext *scp);
static void sigill_handler(int sig, int code, struct sigcontext *scp);
extern "C" void EmulOpTrampoline(void);
#endif

// vde switch variable
char* vde_sock;

/*
 *  Ersatz functions
 */



/*
 *  Helpers to map memory that can be accessed from the Mac side
 */

// NOTE: VM_MAP_32BIT is only used when compiling a 64-bit JIT on specific platforms
void *vm_acquire_mac(size_t size)
{
	return malloc(size);
}




/*
 *  Update virtual clock and trigger interrupts if necessary
 */

#ifdef USE_CPU_EMUL_SERVICES
static uint64 n_check_ticks = 0;
static uint64 emulated_ticks_start = 0;
static uint64 emulated_ticks_count = 0;
static int64 emulated_ticks_current = 0;
static int32 emulated_ticks_quantum = 1000;
int32 emulated_ticks = emulated_ticks_quantum;

void cpu_do_check_ticks(void)
{
#if DEBUG
	n_check_ticks++;
#endif

	uint64 now;
	static uint64 next = 0;
	if (next == 0)
		next = emulated_ticks_start = GetTicks_usec();

	// Update total instructions count
	if (emulated_ticks <= 0) {
		emulated_ticks_current += (emulated_ticks_quantum - emulated_ticks);
		// XXX: can you really have a machine fast enough to overflow
		// a 63-bit m68k instruction counter within 16 ms?
		if (emulated_ticks_current < 0) {
			printf("WARNING: Overflowed 63-bit m68k instruction counter in less than 16 ms!\n");
			goto recalibrate_quantum;
		}
	}

	// Check for interrupt opportunity
	now = GetTicks_usec();
	if (next < now) {
		one_tick();
		do {
			next += 16625;
		} while (next < now);
		emulated_ticks_count++;

		// Recalibrate 1000 Hz quantum every 10 ticks
		static uint64 last = 0;
		if (last == 0)
			last = now;
		else if (now - last > 166250) {
		  recalibrate_quantum:
			emulated_ticks_quantum = ((uint64)emulated_ticks_current * 1000) / (now - last);
			emulated_ticks_current = 0;
			last = now;
		}
	}

	// Update countdown
	if (emulated_ticks <= 0)
		emulated_ticks += emulated_ticks_quantum;
}
#else
uint16 emulated_ticks;
void cpu_do_check_ticks(void)
{
	static int delay = -1;
	if (delay < 0)
		delay = PrefsFindInt32("delay");
	if (delay)
		usleep(delay);
}
#endif


/*
 *  Main program
 */

static void usage(const char *prg_name)
{
	printf(
		"Usage: %s [OPTION...]\n"
		"\nUnix options:\n"
		"  --config FILE\n    read/write configuration from/to FILE\n"
		"  --display STRING\n    X display to use\n"
		"  --break ADDRESS\n    set ROM breakpoint in hexadecimal\n"
		"  --loadbreak FILE\n    load breakpoint from FILE\n"
		"  --rominfo\n    dump ROM information\n"
		"  --switch SWITCH_PATH\n    vde_switch address\n", prg_name
	);
	LoadPrefs(NULL); // read the prefs file so PrefsPrintUsage() will print the correct default values
	PrefsPrintUsage();
	exit(0);
}

int main(int argc, char **argv)
{
	const char *vmdir = NULL;
	char str[256];

	// Initialize variables
	srand(time(NULL));
	tzset();

	// Print some info
	printf(GetString(STR_ABOUT_TEXT1), VERSION_MAJOR, VERSION_MINOR);
	printf(" %s\n", GetString(STR_ABOUT_TEXT2));

	// Parse command line arguments
	for (int i=1; i<argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			usage(argv[0]);
		} else if (strcmp(argv[i], "--break") == 0) {
			argv[i++] = NULL;
			if (i < argc) {
				std::stringstream ss;
				ss << std::hex << argv[i];
				ss >> ROMBreakpoint;
				argv[i] = NULL;
			}
		} else if (strcmp(argv[i], "--config") == 0) {
			argv[i++] = NULL;
			if (i < argc) {
				extern string UserPrefsPath; // from prefs_unix.cpp
				UserPrefsPath = argv[i];
				argv[i] = NULL;
			}
		} else if (strcmp(argv[i], "--rominfo") == 0) {
			argv[i] = NULL;
			PrintROMInfo = true;
		} else if (strcmp(argv[i], "--switch") == 0) {
			argv[i] = NULL;
			if (argv[++i] == NULL) {
				printf("switch address not defined\n");
				usage(argv[0]);
			}
			vde_sock = argv[i];
			argv[i] = NULL;
		}
		
#if defined(__APPLE__) && defined(__MACH__)
		// Mac OS X likes to pass in various options of its own, when launching an app.
		// Attempt to ignore these.
		if (argv[i]) {
			const char * mac_psn_prefix = "-psn_";
			if (strcmp(argv[i], "-NSDocumentRevisionsDebugMode") == 0) {
				argv[i] = NULL;
			} else if (strncmp(mac_psn_prefix, argv[i], strlen(mac_psn_prefix)) == 0) {
				argv[i] = NULL;
			}
		}
#endif
	}

	// Remove processed arguments
	for (int i=1; i<argc; i++) {
		int k;
		for (k=i; k<argc; k++)
			if (argv[k] != NULL)
				break;
		if (k > i) {
			k -= i;
			for (int j=i+k; j<argc; j++)
				argv[j-k] = argv[j];
			argc -= k;
		}
	}

	// Read preferences
	PrefsInit(vmdir, argc, argv);

	// Any command line arguments left?
	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
			fprintf(stderr, "Unrecognized option '%s'\n", argv[i]);
			usage(argv[0]);
		}
	}

	// Initialize SDL system
	int sdl_flags = 0;
	sdl_flags |= SDL_INIT_VIDEO;
	sdl_flags |= SDL_INIT_AUDIO;
	assert(sdl_flags != 0);
	if (SDL_Init(sdl_flags) == -1) {
		char str[256];
		sprintf(str, "Could not initialize SDL: %s.\n", SDL_GetError());
		ErrorAlert(str);
		QuitEmulator();
	}
	atexit(SDL_Quit);

#if __MACOSX__ && SDL_VERSION_ATLEAST(2,0,0)
	// On Mac OS X hosts, SDL2 will create its own menu bar.  This is mostly OK,
	// except that it will also install keyboard shortcuts, such as Command + Q,
	// which can interfere with keyboard shortcuts in the guest OS.
	//
	// HACK: disable these shortcuts, while leaving all other pieces of SDL2's
	// menu bar in-place.
	disable_SDL2_macosx_menu_bar_keyboard_shortcuts();
#endif
	

	// Init system routines
	SysInit();


	

	// Read RAM size
	RAMSize = PrefsFindInt32("ramsize");
	if (RAMSize <= 1000) {
		RAMSize *= 1024 * 1024;
	}
	RAMSize &= 0xfff00000;	// Round down to 1MB boundary
	if (RAMSize < 1024*1024) {
		WarningAlert(GetString(STR_SMALL_RAM_WARN));
		RAMSize = 1024*1024;
	}
	if (RAMSize > 1023*1024*1024)						// Cap to 1023MB (APD crashes at 1GB)
		RAMSize = 1023*1024*1024;

	RAMSize = RAMSize & -getpagesize();					// Round down to page boundary
	RAM.resize( RAMSize );
	// Initialize VM system




#if __MACOSX__
	extern void set_current_directory();
	set_current_directory();
#endif

	// Get rom file path from preferences
	const char *rom_path = PrefsFindString("rom");
	uint32_t ROMSize;
	// Load Mac ROM
	int rom_fd = open(rom_path ? rom_path : ROM_FILE_NAME, O_RDONLY);
	if (rom_fd < 0) {
		ErrorAlert(STR_NO_ROM_FILE_ERR);
		QuitEmulator();
	}
	printf("%s", GetString(STR_READING_ROM_FILE));
	ROMSize = lseek(rom_fd, 0, SEEK_END);
	if (ROMSize != 64*1024 && ROMSize != 128*1024 && ROMSize != 256*1024 && ROMSize != 512*1024 && ROMSize != 1024*1024) {
		ErrorAlert(STR_ROM_SIZE_ERR);
		close(rom_fd);
		QuitEmulator();
	}
	lseek(rom_fd, 0, SEEK_SET);
	if (read(rom_fd, ROMBaseHost, ROMSize) != (ssize_t)ROMSize) {
		ErrorAlert(STR_ROM_FILE_READ_ERR);
		close(rom_fd);
		QuitEmulator();
	}


	// Initialize everything
	if (!InitAll(vmdir))
		QuitEmulator();



	// POSIX.4 timers and real-time signals available, start 60Hz timer
	one_m = SDL_AddTimer(60000, one_minute, nullptr);
	one_s = SDL_AddTimer(1000, one_second, nullptr);
	one_t = SDL_AddTimer(1000/60, one_tick, nullptr);

	// Start 68k and jump to ROM boot routine
	Start680x0();

	QuitEmulator();
	return 0;
}


/*
 *  Quit emulator
 */

void QuitEmulator(void)
{

	// Exit 680x0 emulation
	Exit680x0();

	SDL_RemoveTimer(one_m);
	SDL_RemoveTimer(one_s);
	SDL_RemoveTimer(one_t);

	// Deinitialize everything
	ExitAll();

	// Free ROM/RAM areas

	// Exit system routines
	SysExit();

	// Exit preferences
	PrefsExit();


	exit(0);
}


/*
 *  Code was patched, flush caches if neccessary (i.e. when using a real 680x0
 *  or a dynamically recompiling emulator)
 */

void FlushCodeCache(void *start, uint32 size)
{
#if USE_JIT
    if (UseJIT)
#ifdef UPDATE_UAE
		flush_icache();
#else
		flush_icache_range((uint8 *)start, size);
#endif
#endif
#if !EMULATED_68K && defined(__NetBSD__)
	m68k_sync_icache(start, size);
#endif
}


/*
 *  SIGINT handler, enters mon
 */

#ifdef ENABLE_MON
static void sigint_handler(...)
{
#if EMULATED_68K
	uaecptr nextpc;
#ifdef UPDATE_UAE
	extern void m68k_dumpstate(FILE *, uaecptr *nextpc);
	m68k_dumpstate(stderr, &nextpc);
#else
	extern void m68k_dumpstate(uaecptr *nextpc);
	m68k_dumpstate(&nextpc);
#endif
#endif
	VideoQuitFullScreen();
	const char *arg[4] = {"mon", "-m", "-r", NULL};
	mon(3, arg);
	QuitEmulator();
}
#endif


#ifdef HAVE_PTHREADS
/*
 *  Pthread configuration
 */

void Set_pthread_attr(pthread_attr_t *attr, int priority)
{
	pthread_attr_init(attr);
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
	// Some of these only work for superuser
	if (geteuid() == 0) {
		pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(attr, SCHED_FIFO);
		struct sched_param fifo_param;
		fifo_param.sched_priority = ((sched_get_priority_min(SCHED_FIFO) + 
					      sched_get_priority_max(SCHED_FIFO)) / 2 +
					     priority);
		pthread_attr_setschedparam(attr, &fifo_param);
	}
	if (pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM) != 0) {
#ifdef PTHREAD_SCOPE_BOUND_NP
	    // If system scope is not available (eg. we're not running
	    // with CAP_SCHED_MGT capability on an SGI box), try bound
	    // scope.  It exposes pthread scheduling to the kernel,
	    // without setting realtime priority.
	    pthread_attr_setscope(attr, PTHREAD_SCOPE_BOUND_NP);
#endif
	}
#endif
}
#endif // HAVE_PTHREADS


/*
 *  Mutexes
 */



/*
 *  Interrupt flags (must be handled atomically!)
 */

std::atomic<uint32_t> InterruptFlags {0};

#if EMULATED_68K
void SetInterruptFlag(uint32 flag)
{
	LOCK_INTFLAGS;
	InterruptFlags |= flag;
	UNLOCK_INTFLAGS;
}

void ClearInterruptFlag(uint32 flag)
{
	LOCK_INTFLAGS;
	InterruptFlags &= ~flag;
	UNLOCK_INTFLAGS;
}
#endif

#if !EMULATED_68K
void TriggerInterrupt(void)
{
#if defined(HAVE_PTHREADS)
	pthread_kill(emul_thread, SIG_IRQ);
#else
	raise(SIG_IRQ);
#endif
}

void TriggerNMI(void)
{
	// not yet supported
}
#endif


/*
 *  XPRAM watchdog thread (saves XPRAM every minute)
 */

static void xpram_watchdog(void)
{
	if (memcmp(last_xpram, XPRAM, XPRAM_SIZE)) {
		memcpy(last_xpram, XPRAM, XPRAM_SIZE);
		SaveXPRAM();
	}
}


static Uint32 one_minute(Uint32 interval, void *param)
{
	xpram_watchdog();
	return interval;
}
/*
 *  60Hz thread (really 60.15Hz)
 */

static Uint32 one_second(Uint32 interval, void *param)
{
	// Pseudo Mac 1Hz interrupt, update local time
	WriteMacInt32(0x20c, TimerDateTime());

	SetInterruptFlag(INTFLAG_1HZ);
	TriggerInterrupt();
	return interval;
}

static Uint32 one_tick(Uint32 interval, void *param)
{
	VideoRefresh();
	SetInterruptFlag(INTFLAG_ETHER);

	// Trigger 60Hz interrupt
	if (ROMVersion != ROM_VERSION_CLASSIC || HasMacStarted()) {
		SetInterruptFlag(INTFLAG_60HZ);
		TriggerInterrupt();
	}
	return interval;
}






