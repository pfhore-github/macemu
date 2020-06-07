cmake_minimum_required(VERSION 3.0)
project(BasiliskII C CXX)
find_package(SDL2  REQUIRED)
find_package(GTK2  REQUIRED)
set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS "-Og -g3 -Wall")
set(CMAKE_CXX_FLAGS "-Og -g3 -Wall -Wextra -fsanitize=undefined  -Wno-unused-parameter")
set(cmn_include ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/Unix ${PROJECT_SOURCE_DIR}/new_cpu ${PROJECT_SOURCE_DIR}/chips/include)
include_directories(${SDL2_INCLUDE_DIRS} ${cmn_include})

add_subdirectory(test)

# CPU emuration library
add_library(new_cpu STATIC  new_cpu/common.cpp new_cpu/exceptions.cpp new_cpu/inst.cpp new_cpu/mem.cpp   new_cpu/op_arith.cpp new_cpu/op_bcd.cpp new_cpu/op_bits.cpp new_cpu/op_move.cpp new_cpu/fpu/common.cpp new_cpu/fpu/core.cpp new_cpu/fpu/packed_decimal.cpp new_cpu/mmu/mmu.cpp )
# Hardware Emuration library
add_library(mac_hw  SHARED  machine.cpp io.cpp adb.cpp rtc.cpp chips/asc.cpp chips/glu.cpp chips/iop.cpp chips/jaws.cpp  chips/msc.cpp chips/lc.cpp chips/mcu.cpp chips/memcjr.cpp chips/ncr5380.cpp chips/oss.cpp chips/rbv.cpp chips/scc.cpp chips/sonic.cpp chips/via.cpp chips/via1.cpp chips/via2.cpp chips/swim.cpp chips/z8530.cpp)

# ROM emuration library
add_library(mac_rom SHARED rom/data.cpp rom/wrapper.cpp rom/boot.cpp rom/init_hw.cpp rom/via.cpp rom/get_hw_info.cpp   rom/init_mmu.cpp rom/test_hw.cpp  rom/init_hw2.cpp rom/init_hw3.cpp  rom/remap_rom.cpp rom/check_flag.cpp rom/test_model_id.cpp rom/get_model_id.cpp rom/asc.cpp rom/easc.cpp rom/get_hw_info2.cpp rom/powerbook.cpp  rom/init_scc_iop_fw.cpp rom/sad_mac.cpp rom/serial_modem.cpp rom/get_scc_addr.cpp rom/simm_test.cpp)
target_precompile_headers(mac_rom PRIVATE ${PROJECT_SOURCE_DIR}/include/include.hpp)

if(WIN32)
endif(WIN32)
if(UNIX)
  set(target_src Unix/extfs_unix.cpp Unix/serial_unix.cpp Unix/ether_unix.cpp Unix/Linux/scsi_linux.cpp  Unix/prefs_editor_gtk.cpp Unix/main_unix.cpp Unix/prefs_unix.cpp dummy/clip_dummy.cpp Unix/sys_unix.cpp Unix/xpram_unix.cpp Unix/timer_unix.cpp Unix/disk_sparsebundle.cpp Unix/tinyxml2.cpp Unix/user_strings_unix.cpp Unix/rpc_unix.cpp Unix/sshpty.c Unix/strlcpy.c  prefs.cpp)
  set(target_lib pthread m rt)
  add_compile_definitions(DATADIR="/usr/local/share")
endif(UNIX)

add_executable(BasiliskII main.cpp prefs.cpp prefs_items.cpp rom_patches.cpp slot_rom.cpp rsrc_patches.cpp emul_op.cpp macos_util.cpp xpram.cpp  timer.cpp  adb.cpp serial.cpp ether.cpp sony.cpp disk.cpp cdrom.cpp scsi.cpp video.cpp audio.cpp extfs.cpp 	 user_strings.cpp   SDL/video_sdl2.cpp SDL/audio_sdl.cpp ${target_src} new_cpu/basilisk_glue.cpp  io.cpp  rtc.cpp  machine.cpp CrossPlatform/vm_alloc.cpp  CrossPlatform/video_blit.cpp slirp/bootp.c slirp/ip_output.c  slirp/tcp_input.c      slirp/cksum.c     slirp/mbuf.c       slirp/tcp_output.c     slirp/debug.c     slirp/misc.c       slirp/tcp_subr.c       slirp/if.c        slirp/sbuf.c       slirp/tcp_timer.c      slirp/ip_icmp.c   slirp/slirp.c      slirp/tftp.c           slirp/ip_input.c  slirp/socket.c     slirp/udp.c )
target_link_libraries(BasiliskII new_cpu machw macrom ${SDL2_LIBRARIES} ${GTK2_LIBRARIES}
  ${target_lib})
target_include_directories(BasiliskII
  PUBLIC ${PROJECT_SOURCE_DIR}/Unix ${PROJECT_SOURCE_DIR}/uae_cpu
  ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/CrossPlatform
  ${PROJECT_SOURCE_DIR}/slirp ${GTK2_INCLUDE_DIRS}
  )


