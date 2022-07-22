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


include_directories(./Unix ./mmu_cpu include .)

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
mmu_cpu/compiler/jit_ea.cpp
mmu_cpu/compiler/jit_bits.cpp
mmu_cpu/compiler/jit_arithmetic.cpp
mmu_cpu/compiler/jit_move.cpp
mmu_cpu/compiler/jit_bcd.cpp
mmu_cpu/compiler/jit_branch.cpp
mmu_cpu/compiler/jit_bf.cpp
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
set(TEST_COMMON ${CPU_FILE}  test/test_common.cpp )
set(TEST_LIB fmt boost_unit_test_framework ${SDL2_LIBRARIES} ${LIBRARY})
add_executable(ut ${TEST_COMMON}
test/exception.cpp 

test/bits/bchg.cpp
test/bits/bset.cpp
test/bits/bclr.cpp
test/bits/btst.cpp
test/bits/andi.cpp
test/bits/and.cpp
test/bits/ori.cpp
test/bits/or.cpp
test/bits/eori.cpp
test/bits/eor.cpp
test/bits/clr.cpp
test/bits/not.cpp
test/bits/swap.cpp
test/bits/ext.cpp
test/bits/extb.cpp
test/bits/tst.cpp
test/bits/tas.cpp
test/bits/asr.cpp
test/bits/lsr.cpp
test/bits/roxr.cpp
test/bits/ror.cpp
test/bits/asl.cpp
test/bits/lsl.cpp
test/bits/roxl.cpp
test/bits/rol.cpp

test/bits/bfchg.cpp
test/bits/bfclr.cpp
test/bits/bfexts.cpp
test/bits/bfextu.cpp
test/bits/bfffo.cpp
test/bits/bfins.cpp
test/bits/bfset.cpp
test/bits/bftst.cpp

test/arithmetic/add.cpp
test/arithmetic/addi.cpp
test/arithmetic/addq.cpp
test/arithmetic/addx.cpp
test/arithmetic/adda.cpp
test/arithmetic/sub.cpp
test/arithmetic/subi.cpp
test/arithmetic/subq.cpp
test/arithmetic/suba.cpp
test/arithmetic/subx.cpp
test/arithmetic/cmp.cpp
test/arithmetic/cmpi.cpp
test/arithmetic/cmpa.cpp
test/arithmetic/cmpm.cpp
test/arithmetic/neg.cpp
test/arithmetic/negx.cpp
test/arithmetic/mulu.cpp
test/arithmetic/muls.cpp
test/arithmetic/divu.cpp
test/arithmetic/divs.cpp
test/arithmetic/cmp2.cpp
test/arithmetic/cas.cpp
test/arithmetic/cas2.cpp
test/arithmetic/chk.cpp
test/arithmetic/chk2.cpp

test/bcd/nbcd.cpp
test/bcd/sbcd.cpp
test/bcd/abcd.cpp
test/bcd/pack.cpp
test/bcd/unpk.cpp

test/move/exg.cpp
test/move/lea.cpp
test/move/move.cpp
test/move/movea.cpp
test/move/move16.cpp
test/move/movem.cpp
test/move/movep.cpp
test/move/moveq.cpp
test/move/pea.cpp
test/move/scc.cpp
test/move/moves.cpp
test/move/movec.cpp

test/branch/bcc.cpp
test/branch/bkpt.cpp
test/branch/bra.cpp
test/branch/bsr.cpp
test/branch/dbcc.cpp
test/branch/illegal.cpp
test/branch/link.cpp
test/branch/nop.cpp
test/branch/unlk.cpp
test/branch/bkpt.cpp
test/branch/trap.cpp
test/branch/trapcc.cpp
test/branch/reset.cpp
test/branch/rte.cpp
test/branch/rtd.cpp
test/branch/rts.cpp
test/branch/trapv.cpp
test/branch/rtr.cpp
test/branch/jsr.cpp
test/branch/jmp.cpp

test/fpu/fabs.cpp
test/fpu/facos.cpp
test/fpu/fadd.cpp
test/fpu/fasin.cpp
test/fpu/fatan.cpp
test/fpu/fatanh.cpp
test/fpu/fcmp.cpp
test/fpu/fcos.cpp
test/fpu/fcosh.cpp
test/fpu/fdiv.cpp
test/fpu/fetox.cpp
test/fpu/fetoxm1.cpp
test/fpu/fgetexp.cpp
test/fpu/fgetman.cpp
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

)

add_executable(jit ${TEST_COMMON}


test/compiler/move/jit_lea.cpp
test/compiler/move/jit_pea.cpp
test/compiler/move/jit_moves.cpp
test/compiler/move/jit_move.cpp
test/compiler/move/jit_movec.cpp
test/compiler/move/jit_movem.cpp

test/compiler/bits/jit_ori.cpp
test/compiler/bits/jit_ext.cpp
test/compiler/bits/jit_extb.cpp
test/compiler/bits/jit_andi.cpp
test/compiler/bits/jit_eori.cpp
test/compiler/bits/jit_clr.cpp
test/compiler/bits/jit_not.cpp
test/compiler/bits/jit_tst.cpp
test/compiler/bits/jit_tas.cpp
test/compiler/bits/jit_swap.cpp
test/compiler/bits/jit_bclr.cpp
test/compiler/bits/jit_btst.cpp
test/compiler/bits/jit_bchg.cpp
test/compiler/bits/jit_bset.cpp

test/compiler/bits/jit_asr.cpp
test/compiler/bits/jit_lsr.cpp
test/compiler/bits/jit_roxr.cpp
test/compiler/bits/jit_ror.cpp
test/compiler/bits/jit_asl.cpp
test/compiler/bits/jit_lsl.cpp
test/compiler/bits/jit_roxl.cpp
test/compiler/bits/jit_rol.cpp

test/compiler/bits/jit_bftst.cpp
test/compiler/bits/jit_bfchg.cpp
test/compiler/bits/jit_bfclr.cpp
test/compiler/bits/jit_bfset.cpp
test/compiler/bits/jit_bfextu.cpp
test/compiler/bits/jit_bfexts.cpp
test/compiler/bits/jit_bfffo.cpp
test/compiler/bits/jit_bfins.cpp


test/compiler/arithmetic/jit_addi.cpp
test/compiler/arithmetic/jit_subi.cpp
test/compiler/arithmetic/jit_cmpi.cpp
test/compiler/arithmetic/jit_neg.cpp
test/compiler/arithmetic/jit_negx.cpp
test/compiler/arithmetic/jit_cas.cpp
test/compiler/arithmetic/jit_cmp2.cpp
test/compiler/arithmetic/jit_chk2.cpp
test/compiler/arithmetic/jit_mulu.cpp
test/compiler/arithmetic/jit_muls.cpp
test/compiler/arithmetic/jit_divs.cpp
test/compiler/bcd/jit_nbcd.cpp
test/compiler/branch/jit_bkpt.cpp
test/compiler/branch/jit_link.cpp
test/compiler/branch/jit_illegal.cpp
test/compiler/branch/jit_trap.cpp
test/compiler/branch/jit_trapv.cpp
test/compiler/branch/jit_unlk.cpp
test/compiler/branch/jit_nop.cpp
test/compiler/branch/jit_reset.cpp
test/compiler/branch/jit_rte.cpp
test/compiler/branch/jit_rts.cpp
test/compiler/branch/jit_rtr.cpp
test/compiler/branch/jit_rtd.cpp
test/compiler/branch/jit_jsr.cpp
test/compiler/branch/jit_jmp.cpp

)
target_link_libraries(ut ${TEST_LIB})
target_link_libraries(jit ${TEST_LIB})
add_test(NAME ut COMMAND ./ut)
add_test(NAME jit COMMAND ./jit)



