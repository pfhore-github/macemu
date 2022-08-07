cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
enable_testing()
project(Basillisk2 CXX)
if ( CMAKE_COMPILER_IS_GNUCXX )
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-coverage")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  -Wno-unused-parameter -Wall -Wextra --coverage")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()


include_directories(./Unix ./mmu_cpu include . ./test)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

set(CPU_FILE mmu_cpu/memory.cpp
mmu_cpu/newcpu.cpp mmu_cpu/exception.cpp  mmu_cpu/ea.cpp 
mmu_cpu/mbus.cpp
mmu_cpu/fpu/fpu_mpfr.cpp 
mmu_cpu/mmu/mmu_68040.cpp
mmu_cpu/compiler/compiler.cpp
mmu_cpu/compiler/bits.cpp
mmu_cpu/compiler/arithmetic.cpp
mmu_cpu/compiler/move.cpp
mmu_cpu/compiler/bcd.cpp
mmu_cpu/compiler/branch.cpp
mmu_cpu/compiler/bitfield.cpp
)

#add_executable(Basillisk2 main.cpp prefs.cpp prefs_items.cpp Unix/sys_unix.cpp rom_patches.cpp slot_rom.cpp rsrc_patches.cpp emul_op.cpp macos_util.cpp xpram.cpp 
#Unix/xpram_unix.cpp timer.cpp Unix/timer_unix.cpp adb.cpp serial.cpp ether.cpp sony.cpp disk.cpp cdrom.cpp scsi.cpp video.cpp audio.cpp extfs.cpp 
#Unix/disk_sparsebundle.cpp Unix/tinyxml2.cpp user_strings.cpp Unix/user_strings_unix.cpp Unix/sshpty.c Unix/strlcpy.c  
#CrossPlatform/vm_alloc.cpp CrossPlatform/video_blit.cpp
#slirp/bootp.c slirp/ip_output.c slirp/tcp_input.c slirp/cksum.c slirp/mbuf.c slirp/tcp_output.c slirp/debug.c 
##slirp/misc.c slirp/tcp_subr.c slirp/if.c slirp/sbuf.c slirp/tcp_timer.c slirp/ip_icmp.c slirp/slirp.c
#slirp/tftp.c slirp/ip_input.c slirp/socket.c slirp/udp.c

#SDL/video_sdl2.cpp SDL/common.cpp Unix/extfs_unix.cpp Unix/serial_unix.cpp Unix/ether_unix.cpp 
#Unix/Linux/scsi_linux.cpp SDL/audio_sdl.cpp 
# Unix/prefs_editor_gtk.cpp 
#  Unix/main_unix.cpp Unix/prefs_unix.cpp 
#dummy/clip_dummy.cpp

#${CPU_FILE}
#mmu_cpu/basilisk_glue.cpp
#
#mmu_cpu/compiler/compemu_support.cpp mmu_cpu/compiler/compemu_fpp.cpp compstbl.o cpustbl_nf.o

#)

add_executable(Yame 
yame.cpp
${CPU_FILE}
mmu_cpu/basilisk_glue.cpp
mmu_cpu/disasm.cpp
chips/io.cpp
chips/via.cpp
chips/mcu.cpp
chips/iop.cpp
chips/rtc.cpp
#chips/scc.cpp
rom/boot.cpp
rom/boot2.cpp
rom/romdata.cpp
)
set( LIBRARY ${SDL2_LIBRARIES} vdeplug Threads::Threads fmt mpfr gmp)
#target_link_libraries(Basillisk2 PRIVATE ${SDL2_LIBRARIES} vdeplug Threads::Threads mpfr gmp)
target_link_libraries(Yame PRIVATE ${LIBRARY} )
#target_include_directories(Basillisk2 PRIVATE ${GTK_INCLUDE_DIRS})
#target_link_directories(Basillisk2 PRIVATE ${GTK_LIBRARY_DIRS})
IF (WIN32)
  # set stuff for windows
ELSE()
  # set stuff for other systems
ENDIF()
set(TEST_COMMON ${CPU_FILE}  test/test_common.cpp )
set(TEST_LIB fmt boost_unit_test_framework ${SDL2_LIBRARIES} ${LIBRARY} yaml-cpp)
add_executable(ut ${TEST_COMMON}
test/cpu.cpp
test/exception.cpp 
#[[

test/fpu/fint.cpp 
test/fpu/fintrz.cpp
test/fpu/flog10.cpp
test/fpu/flog2.cpp
test/fpu/flogn.cpp
test/fpu/flognp1.cpp
test/fpu/fmod.cpp
test/fpu/fmul.cpp
test/fpu/fneg.cpp
test/fpu/fnop.cpp
test/fpu/frem.cpp
test/fpu/fscale.cpp
test/fpu/fsgldiv.cpp
test/fpu/fsglmul.cpp
test/fpu/fsin.cpp
test/fpu/fsincos.cpp
test/fpu/fsinh.cpp
test/fpu/fsqrt.cpp
test/fpu/fsub.cpp
test/fpu/ftan.cpp
test/fpu/ftanh.cpp
test/fpu/ftentox.cpp
test/fpu/ftst.cpp
test/fpu/ftwotox.cpp


test/fpu/fmove.cpp
test/fpu/fmovecr.cpp
test/fpu/fmovem.cpp

test/fpu/fbcc.cpp
test/fpu/fdbcc.cpp
test/fpu/fscc.cpp

test/fpu/nan.cpp

test/mmu/disabled.cpp
test/mmu/ttr.cpp
test/mmu/atc.cpp
test/mmu/lookup.cpp
test/mmu/pflush.cpp
test/mmu/ptest.cpp

chips/via.cpp
chips/rtc.cpp
chips/easc.cpp

peripheral/adb.cpp
test/via.cpp
]]
)


target_link_libraries(ut ${TEST_LIB})
add_test(NAME ut COMMAND ./ut)



