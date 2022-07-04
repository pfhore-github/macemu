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


include_directories(./Unix ./mmu_cpu include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

set(CPU_FILE mmu_cpu/memory.cpp
mmu_cpu/newcpu.cpp mmu_cpu/exception.cpp  mmu_cpu/ea.cpp 
mmu_cpu/mbus.cpp
mmu_cpu/op_move.cpp
mmu_cpu/op_arithmetic.cpp
mmu_cpu/op_bits.cpp
mmu_cpu/op_branch.cpp
mmu_cpu/op_bcd.cpp
mmu_cpu/op_bf.cpp
mmu_cpu/fpu/fpu_mpfr.cpp 
mmu_cpu/mmu/mmu_68040.cpp
mmu_cpu/compiler/jit.cpp
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
#chips/scc.cpp
rom/boot.cpp
rom/boot2.cpp
rom/romdata.cpp
)
set( LIBRARY ${SDL2_LIBRARIES} vdeplug Threads::Threads fmt mpfr gmp asmjit)
#target_link_libraries(Basillisk2 PRIVATE ${SDL2_LIBRARIES} vdeplug Threads::Threads mpfr gmp)
target_link_libraries(Yame PRIVATE ${LIBRARY} )
#target_include_directories(Basillisk2 PRIVATE ${GTK_INCLUDE_DIRS})
#target_link_directories(Basillisk2 PRIVATE ${GTK_LIBRARY_DIRS})
IF (WIN32)
  # set stuff for windows
ELSE()
  # set stuff for other systems
ENDIF()
set(TEST_COMMON ${CPU_FILE}  mmu_cpu/test/test_common.cpp )
set(TEST_LIB fmt boost_unit_test_framework ${SDL2_LIBRARIES} ${LIBRARY})
add_executable(ut ${TEST_COMMON}
mmu_cpu/test/exception.cpp 

mmu_cpu/test/bits/bchg.cpp
mmu_cpu/test/bits/bset.cpp
mmu_cpu/test/bits/bclr.cpp
mmu_cpu/test/bits/btst.cpp
mmu_cpu/test/bits/andi.cpp
mmu_cpu/test/bits/and.cpp
mmu_cpu/test/bits/ori.cpp
mmu_cpu/test/bits/or.cpp
mmu_cpu/test/bits/eori.cpp
mmu_cpu/test/bits/eor.cpp
mmu_cpu/test/bits/clr.cpp
mmu_cpu/test/bits/not.cpp
mmu_cpu/test/bits/swap.cpp
mmu_cpu/test/bits/ext.cpp
mmu_cpu/test/bits/extb.cpp
mmu_cpu/test/bits/tst.cpp
mmu_cpu/test/bits/tas.cpp
mmu_cpu/test/bits/asr.cpp
mmu_cpu/test/bits/lsr.cpp
mmu_cpu/test/bits/roxr.cpp
mmu_cpu/test/bits/ror.cpp
mmu_cpu/test/bits/asl.cpp
mmu_cpu/test/bits/lsl.cpp
mmu_cpu/test/bits/roxl.cpp
mmu_cpu/test/bits/rol.cpp

mmu_cpu/test/bits/bfchg.cpp
mmu_cpu/test/bits/bfclr.cpp
mmu_cpu/test/bits/bfexts.cpp
mmu_cpu/test/bits/bfextu.cpp
mmu_cpu/test/bits/bfffo.cpp
mmu_cpu/test/bits/bfins.cpp
mmu_cpu/test/bits/bfset.cpp
mmu_cpu/test/bits/bftst.cpp

mmu_cpu/test/arithmetic/add.cpp
mmu_cpu/test/arithmetic/addi.cpp
mmu_cpu/test/arithmetic/addq.cpp
mmu_cpu/test/arithmetic/addx.cpp
mmu_cpu/test/arithmetic/adda.cpp
mmu_cpu/test/arithmetic/sub.cpp
mmu_cpu/test/arithmetic/subi.cpp
mmu_cpu/test/arithmetic/subq.cpp
mmu_cpu/test/arithmetic/suba.cpp
mmu_cpu/test/arithmetic/subx.cpp
mmu_cpu/test/arithmetic/cmp.cpp
mmu_cpu/test/arithmetic/cmpi.cpp
mmu_cpu/test/arithmetic/cmpa.cpp
mmu_cpu/test/arithmetic/cmpm.cpp
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

mmu_cpu/test/move/exg.cpp
mmu_cpu/test/move/lea.cpp
mmu_cpu/test/move/move.cpp
mmu_cpu/test/move/movea.cpp
mmu_cpu/test/move/move16.cpp
mmu_cpu/test/move/movem.cpp
mmu_cpu/test/move/movep.cpp
mmu_cpu/test/move/moveq.cpp
mmu_cpu/test/move/pea.cpp
mmu_cpu/test/move/scc.cpp
mmu_cpu/test/move/moves.cpp
mmu_cpu/test/move/movec.cpp

mmu_cpu/test/branch/bcc.cpp
mmu_cpu/test/branch/bkpt.cpp
mmu_cpu/test/branch/bra.cpp
mmu_cpu/test/branch/bsr.cpp
mmu_cpu/test/branch/dbcc.cpp
mmu_cpu/test/branch/illegal.cpp
mmu_cpu/test/branch/link.cpp
mmu_cpu/test/branch/nop.cpp
mmu_cpu/test/branch/unlk.cpp
mmu_cpu/test/branch/bkpt.cpp
mmu_cpu/test/branch/trap.cpp
mmu_cpu/test/branch/trapcc.cpp
mmu_cpu/test/branch/reset.cpp
mmu_cpu/test/branch/rte.cpp
mmu_cpu/test/branch/rtd.cpp
mmu_cpu/test/branch/rts.cpp
mmu_cpu/test/branch/trapv.cpp
mmu_cpu/test/branch/rtr.cpp
mmu_cpu/test/branch/jsr.cpp
mmu_cpu/test/branch/jmp.cpp

mmu_cpu/test/fpu/fabs.cpp
mmu_cpu/test/fpu/facos.cpp
mmu_cpu/test/fpu/fadd.cpp
mmu_cpu/test/fpu/fasin.cpp
mmu_cpu/test/fpu/fatan.cpp
mmu_cpu/test/fpu/fatanh.cpp
mmu_cpu/test/fpu/fcmp.cpp
mmu_cpu/test/fpu/fcos.cpp
mmu_cpu/test/fpu/fcosh.cpp
mmu_cpu/test/fpu/fdiv.cpp
mmu_cpu/test/fpu/fetox.cpp
mmu_cpu/test/fpu/fetoxm1.cpp
mmu_cpu/test/fpu/fgetexp.cpp
mmu_cpu/test/fpu/fgetman.cpp
mmu_cpu/test/fpu/fint.cpp 
mmu_cpu/test/fpu/fintrz.cpp
mmu_cpu/test/fpu/flog10.cpp
mmu_cpu/test/fpu/flog2.cpp
mmu_cpu/test/fpu/flogn.cpp
mmu_cpu/test/fpu/flognp1.cpp
mmu_cpu/test/fpu/fmod.cpp
mmu_cpu/test/fpu/fmul.cpp
mmu_cpu/test/fpu/fneg.cpp
mmu_cpu/test/fpu/fnop.cpp
mmu_cpu/test/fpu/frem.cpp
mmu_cpu/test/fpu/fscale.cpp
mmu_cpu/test/fpu/fsgldiv.cpp
mmu_cpu/test/fpu/fsglmul.cpp
mmu_cpu/test/fpu/fsin.cpp
mmu_cpu/test/fpu/fsincos.cpp
mmu_cpu/test/fpu/fsinh.cpp
mmu_cpu/test/fpu/fsqrt.cpp
mmu_cpu/test/fpu/fsub.cpp
mmu_cpu/test/fpu/ftan.cpp
mmu_cpu/test/fpu/ftanh.cpp
mmu_cpu/test/fpu/ftentox.cpp
mmu_cpu/test/fpu/ftst.cpp
mmu_cpu/test/fpu/ftwotox.cpp


mmu_cpu/test/fpu/fmove.cpp
mmu_cpu/test/fpu/fmovecr.cpp
mmu_cpu/test/fpu/fmovem.cpp

mmu_cpu/test/fpu/fbcc.cpp
mmu_cpu/test/fpu/fdbcc.cpp
mmu_cpu/test/fpu/fscc.cpp

mmu_cpu/test/fpu/nan.cpp

mmu_cpu/test/mmu/disabled.cpp
mmu_cpu/test/mmu/ttr.cpp
mmu_cpu/test/mmu/atc.cpp
mmu_cpu/test/mmu/lookup.cpp
mmu_cpu/test/mmu/pflush.cpp
mmu_cpu/test/mmu/ptest.cpp
 )
target_link_libraries(ut ${TEST_LIB})
add_test(NAME ut COMMAND ./ut)



