cmake_minimum_required(VERSION 3.23)
enable_testing()
project(Basillisk2 CXX)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()


include_directories(./Unix ./mmu_cpu include ./SDL ./slirp ./CrossPlatform)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

set(CPU_FILE mmu_cpu/memory.cpp 
mmu_cpu/newcpu.cpp mmu_cpu/exception.cpp  mmu_cpu/ea.cpp 
mmu_cpu/op_move.cpp
mmu_cpu/op_arithmetic.cpp
mmu_cpu/op_bits.cpp
mmu_cpu/op_branch.cpp
mmu_cpu/op_bcd.cpp
mmu_cpu/op_bf.cpp
mmu_cpu/fpu/fpu_ieee.cpp 
)

add_executable(Basillisk2 main.cpp prefs.cpp prefs_items.cpp Unix/sys_unix.cpp rom_patches.cpp slot_rom.cpp rsrc_patches.cpp emul_op.cpp macos_util.cpp xpram.cpp 
Unix/xpram_unix.cpp timer.cpp Unix/timer_unix.cpp adb.cpp serial.cpp ether.cpp sony.cpp disk.cpp cdrom.cpp scsi.cpp video.cpp audio.cpp extfs.cpp 
Unix/disk_sparsebundle.cpp Unix/tinyxml2.cpp user_strings.cpp Unix/user_strings_unix.cpp Unix/sshpty.c Unix/strlcpy.c  
CrossPlatform/vm_alloc.cpp CrossPlatform/video_blit.cpp
slirp/bootp.c slirp/ip_output.c slirp/tcp_input.c slirp/cksum.c slirp/mbuf.c slirp/tcp_output.c slirp/debug.c 
slirp/misc.c slirp/tcp_subr.c slirp/if.c slirp/sbuf.c slirp/tcp_timer.c slirp/ip_icmp.c slirp/slirp.c
slirp/tftp.c slirp/ip_input.c slirp/socket.c slirp/udp.c

SDL/video_sdl2.cpp SDL/common.cpp Unix/extfs_unix.cpp Unix/serial_unix.cpp Unix/ether_unix.cpp 
Unix/Linux/scsi_linux.cpp SDL/audio_sdl.cpp 
# Unix/prefs_editor_gtk.cpp 
  Unix/main_unix.cpp Unix/prefs_unix.cpp 
dummy/clip_dummy.cpp

${CPU_FILE}
mmu_cpu/basilisk_glue.cpp
#
#mmu_cpu/compiler/compemu_support.cpp mmu_cpu/compiler/compemu_fpp.cpp compstbl.o cpustbl_nf.o

)

target_link_libraries(Basillisk2 PRIVATE ${SDL2_LIBRARIES} vdeplug Threads::Threads)
#target_include_directories(Basillisk2 PRIVATE ${GTK_INCLUDE_DIRS})
#target_link_directories(Basillisk2 PRIVATE ${GTK_LIBRARY_DIRS})
IF (WIN32)
  # set stuff for windows
ELSE()
  # set stuff for other systems
ENDIF()
set(TEST_COMMON ${CPU_FILE}  mmu_cpu/test/test_common.cpp )
set(TEST_LIB fmt boost_unit_test_framework ${SDL2_LIBRARIES} Threads::Threads )
add_executable(test_cpu ${TEST_COMMON}
mmu_cpu/test/cpu_impl.cpp
mmu_cpu/test/exception.cpp 

mmu_cpu/test/bits/bchg.cpp
mmu_cpu/test/bits/bset.cpp
mmu_cpu/test/bits/bclr.cpp
mmu_cpu/test/bits/btst.cpp
mmu_cpu/test/bits/andi.cpp
mmu_cpu/test/bits/and.cpp
mmu_cpu/test/bits/ori.cpp
mmu_cpu/test/bits/or.cpp
mmu_cpu/test/bits/eor.cpp
mmu_cpu/test/bits/clr.cpp
mmu_cpu/test/bits/not.cpp
mmu_cpu/test/bits/ext.cpp
mmu_cpu/test/bits/extb.cpp
mmu_cpu/test/bits/tst.cpp
mmu_cpu/test/bits/asr.cpp
mmu_cpu/test/bits/lsr.cpp
mmu_cpu/test/bits/roxr.cpp
mmu_cpu/test/bits/ror.cpp
mmu_cpu/test/bits/asl.cpp
mmu_cpu/test/bits/lsl.cpp
mmu_cpu/test/bits/roxl.cpp
mmu_cpu/test/bits/rol.cpp

mmu_cpu/test/bitfields.cpp

mmu_cpu/test/arithmetic/add.cpp
mmu_cpu/test/arithmetic/addi.cpp
mmu_cpu/test/arithmetic/addq.cpp
mmu_cpu/test/arithmetic/adda.cpp
mmu_cpu/test/arithmetic/sub.cpp
mmu_cpu/test/arithmetic/subi.cpp
mmu_cpu/test/arithmetic/subq.cpp
mmu_cpu/test/arithmetic/suba.cpp
mmu_cpu/test/arithmetic/cmp.cpp
mmu_cpu/test/arithmetic/cmpi.cpp
mmu_cpu/test/arithmetic/cmpa.cpp
mmu_cpu/test/arithmetic/neg.cpp
mmu_cpu/test/arithmetic/negx.cpp
mmu_cpu/test/arithmetic/mulu.cpp
mmu_cpu/test/arithmetic/muls.cpp
mmu_cpu/test/arithmetic/divu.cpp
mmu_cpu/test/arithmetic/divs.cpp
mmu_cpu/test/arithmetic/cmp2.cpp
mmu_cpu/test/arithmetic/cas.cpp
mmu_cpu/test/arithmetic/cas2.cpp
mmu_cpu/test/arithmetic/chk.cpp
mmu_cpu/test/arithmetic/chk2.cpp

mmu_cpu/test/bcd/nbcd.cpp
mmu_cpu/test/bcd/sbcd.cpp
mmu_cpu/test/bcd/abcd.cpp
mmu_cpu/test/bcd/pack.cpp
mmu_cpu/test/bcd/unpk.cpp
 )
target_link_libraries(test_cpu ${TEST_LIB})
add_test(NAME test_cpu COMMAND ./test_cpu)
function( cpu_test name)
  add_executable(test_cpu_${name} ${TEST_COMMON} mmu_cpu/test/${name}.cpp )
  target_link_libraries(test_cpu_${name} ${TEST_LIB})
  add_test(NAME test_cpu_${name} COMMAND ./test_cpu_${name})
endfunction(cpu_test)

cpu_test(movep)
cpu_test(move)
cpu_test(link)
cpu_test(swap)
cpu_test(pea)
cpu_test(movem)
cpu_test(unlk)
cpu_test(rtx)
cpu_test(jmp)
cpu_test(lea)
cpu_test(scc)
cpu_test(dbcc)
cpu_test(exg)

add_executable(test_fpu ${TEST_COMMON}
mmu_cpu/test/fpu/fmove.cpp
mmu_cpu/test/fpu/fint.cpp 
mmu_cpu/test/fpu/fsinh.cpp
mmu_cpu/test/fpu/fintrz.cpp
mmu_cpu/test/fpu/fsqrt.cpp
mmu_cpu/test/fpu/flognp1.cpp
mmu_cpu/test/fpu/fexpm1.cpp
mmu_cpu/test/fpu/ftanh.cpp
mmu_cpu/test/fpu/fatan.cpp
mmu_cpu/test/fpu/fasin.cpp
mmu_cpu/test/fpu/fatanh.cpp
mmu_cpu/test/fpu/fsin.cpp
mmu_cpu/test/fpu/ftan.cpp
mmu_cpu/test/fpu/fetox.cpp
 )
target_link_libraries(test_fpu ${TEST_LIB})
add_test(NAME test_fpu COMMAND ./test_fpu)
