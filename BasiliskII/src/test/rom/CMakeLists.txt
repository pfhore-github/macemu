cmake_minimum_required(VERSION 3.0)
enable_testing()
find_package(SDL2  REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "-Og -g3 -Wall -Wextra -fsanitize=undefined  -Wno-unused-parameter")
find_package(Boost REQUIRED unit_test_framework)
set(ROOT_DIR ../..)
set(ROM_DIR ../../rom)
set(cmn_include ${ROOT_DIR}/include  ${ROOT_DIR}/Unix ${ROOT_DIR}/new_cpu ${ROOT_DIR}/chips/include ${ROOT_DIR}/rom)
include_directories(${SDL2_INCLUDE_DIRS} ${cmn_include} ../include)
link_directories(../..)
link_libraries(new_cpu mac_rom mac_hw ${SDL2_LIBRARIES} ${target_lib} boost_unit_test_framework fmt test_common)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(test_addon  common.cpp)
function(UT name)
  ADD_EXECUTABLE(${name} ${name}.cpp ${test_addon} )
  target_precompile_headers(${name} REUSE_FROM boot)
  add_test(
	NAME ${name}
	COMMAND ./bin/${name}
	)
  add_test(
	NAME ${name}_ROM
	COMMAND ./bin/${name} -- --rom
	)
endfunction()
function(UT2 name)
  ADD_EXECUTABLE(${name} ${name}.cpp common.cpp  ${ARGN})
  target_precompile_headers(${name} REUSE_FROM boot)
  add_test(
	NAME ${name}
	COMMAND ./bin/${name}
	)
  add_test(
	NAME ${name}_ROM
	COMMAND ./bin/${name} -- --rom
	)
endfunction()

ADD_EXECUTABLE(boot boot.cpp ${test_addon})
target_precompile_headers(boot PRIVATE ${ROOT_DIR}/include/include.hpp)
add_test( NAME boot COMMAND ./bin/boot )
add_test( NAME boot_ROM	COMMAND ./bin/boot -- --rom	)

UT(init_hw )
UT(reset_via)
UT2(get_hw_info)
UT2(check_mb)
UT(init_mmu)
UT(has_fpu)
UT(test_iifx_exp1)
UT(has_via)
UT(has_rbv)
UT(test_scsi_io)
UT(test_scsi_cmd)
UT(test_iop)
UT(has_ether)
UT(has_lowmem1)
UT(read_id_from_via)
# UT(init_hw2)
UT(init_hw3)
UT(init_powerbook)
UT(test_scsi )
UT(check_flag )
# remap_rom
UT(test_model_id )
UT(get_model_id)
UT(play_asc2 )
UT(play_asc3 )
UT(play_asc5 )
UT(play_asc)
UT(play_chime_1)
UT(play_chime_4)
UT(play_easc  )
ADD_EXECUTABLE(play_easc_run play_easc_run.cpp ${test_addon}  ) 
target_precompile_headers(play_easc_run REUSE_FROM boot)
UT(wait_until_easc_fifo_half )
UT(easc_beep )
UT(get_hw_info2 )
UT(send_to_pb_key )
UT(send_to_pb_dword )
UT(send_to_pb_byte)
UT(recv_from_pb)
UT(init_scc_iop_fw)
UT(init_pb_duo )
UT(check_easc_fifo_empty)
UT(check_easc_fifo_half)
UT(check_easc_fifo_done)
UT(easc_is_sonora)
UT(modem_port_send1 )
UT(get_scc_addr  )
UT(init_modem_port)
UT(read_modem_port)
UT(write_modem_port_byte)
UT(write_modem_port_cstr )
UT(from_ascii)
UT(read_modem_cmd_dword)
UT(to_ascii)
UT(write_modem_integer )
UT(read_modem_cmd1 )
UT(comm_modem_port)
UT(ram_check)
UT(check_simm)
UT(check_simm_size)
UT(check_simm_slots)

UT(pb_handler52)
UT(init_adb)
