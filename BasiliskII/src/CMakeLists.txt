cmake_minimum_required(VERSION 3.23)
enable_testing()
project(Basillisk2 CXX)
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
mmu_cpu/op_math.cpp
mmu_cpu/op_bits.cpp
mmu_cpu/op_branch.cpp
mmu_cpu/op_bcd.cpp
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
#mmu_cpu/fpu/fpu_ieee.cpp 
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
set(TEST_COMMON ${CPU_FILE} mmu_cpu/test/_test_common2.cpp mmu_cpu/test/test_common.cpp )
set(TEST_LIB fmt boost_unit_test_framework ${SDL2_LIBRARIES} Threads::Threads )
add_executable(test_cpu ${TEST_COMMON}
#mmu_cpu/fpu/fpu_ieee.cpp 
#mmu_cpu/compiler/compemu_support.cpp mmu_cpu/compiler/compemu_fpp.cpp
mmu_cpu/test/cpu1.cpp  )
target_link_libraries(test_cpu ${TEST_LIB})
file(GLOB TEST_FILE mmu_cpu/test/[!_]*.cpp)
add_test(NAME test_cpu COMMAND ./test_cpu)
add_custom_command(
  OUTPUT ../mmu_cpu/test/_test_common2.cpp
  WORKING_DIRECTORY ../mmu_cpu/test
  COMMAND perl asm.pl
  DEPENDS ./mmu_cpu/test/asm.pl ${TEST_FILE}
  )
function( cpu_test name)
  add_executable(test_cpu_${name} ${TEST_COMMON} mmu_cpu/test/${name}.cpp )
  target_link_libraries(test_cpu_${name} ${TEST_LIB})
  add_test(NAME test_cpu_${name} COMMAND ./test_cpu_${name})
endfunction(cpu_test)
cpu_test(math)
cpu_test(and)
cpu_test(cmp2)
cpu_test(chk2)
cpu_test(btst)
cpu_test(bchg)
cpu_test(bclr)
cpu_test(bset)
cpu_test(cas)
cpu_test(cas2)
cpu_test(movep)
cpu_test(move)
cpu_test(negx)
cpu_test(clr)
cpu_test(neg)
cpu_test(not)
cpu_test(nbcd)
cpu_test(link)
cpu_test(swap)
cpu_test(pea)
cpu_test(ext)
cpu_test(movem)
cpu_test(tst)
cpu_test(mul)
cpu_test(div)
cpu_test(unlk)
cpu_test(rtx)
cpu_test(jmp)
cpu_test(chk)
cpu_test(lea)
cpu_test(scc)
cpu_test(dbcc)
cpu_test(or)
cpu_test(sbcd)
cpu_test(pack)
cpu_test(unpk)
cpu_test(sub)
cpu_test(cmp)
cpu_test(eor)
cpu_test(exg)
cpu_test(add)
cpu_test(shift)