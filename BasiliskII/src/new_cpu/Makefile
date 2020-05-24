HEADERS=common.hpp inst_cmn.hpp mem.cpp op.h  registers.hpp mmu/mmu.hpp intel.h fpu/fpu.hpp fpu/68881.hpp
OPTIONS=--std=gnu++17 -g -Og -Wall `sdl2-config --cflags --libs`
TEST_OPTIONS=--std=gnu++17 -g -O0 `sdl2-config --cflags --libs`
all: test_cpu test_fpu test_mmu

clean:
	rm obj/*.o obj/*/*.o obj/*/*.gcno obj/libnewcpu.a test_cpu test_fpu

obj/%.o: %.cpp $(HEADERS)
	g++ $(OPTIONS) -c $< -o $@

obj/libnewcpu.a: obj/op_bcd.o obj/mem.o  obj/op_move.o obj/common.o  obj/inst.o   obj/op_arith.o  obj/op_bits.o obj/mmu/mmu.o obj/exceptions.o obj/fpu/core.o obj/fpu/packed_decimal.o obj/fpu/common.o 
	ar rcs obj/libnewcpu.a $^

obj/test/%.o: test_src/%.cpp $(HEADERS)
	g++ $(TEST_OPTIONS) -c $< -o $@

TEST_FILE=obj/test/test_main.o obj/test/test.o obj/test/abcd.o obj/test/add.o obj/test/adda.o obj/test/addi.o obj/test/addq.o obj/test/addx.o obj/test/and.o obj/test/andi.o obj/test/asl.o obj/test/asr.o obj/test/bcc.o obj/test/bchg.o obj/test/bclr.o obj/test/bfchg.o obj/test/bfclr.o obj/test/bfexts.o obj/test/bfextu.o obj/test/bfffo.o obj/test/bfins.o obj/test/bfset.o obj/test/bftst.o obj/test/bkpt.o obj/test/bra.o obj/test/bset.o obj/test/bsr.o obj/test/btst.o obj/test/callm.o obj/test/cas.o obj/test/cas2.o obj/test/chk.o obj/test/chk2.o obj/test/clr.o obj/test/cmp.o obj/test/cmpa.o obj/test/cmpi.o obj/test/cmpm.o obj/test/cmp2.o obj/test/dbcc.o obj/test/divs.o obj/test/divu.o obj/test/eor.o obj/test/eori.o obj/test/exg.o obj/test/ext.o obj/test/illegal.o obj/test/jmp.o obj/test/jsr.o obj/test/lea.o obj/test/link.o obj/test/lsl.o obj/test/lsr.o obj/test/move.o obj/test/movea.o obj/test/move16.o obj/test/movem.o obj/test/movep.o obj/test/moveq.o obj/test/muls.o obj/test/mulu.o obj/test/nbcd.o obj/test/neg.o obj/test/negx.o obj/test/nop.o obj/test/not.o obj/test/or.o obj/test/ori.o obj/test/pack.o obj/test/pea.o obj/test/rol.o obj/test/ror.o obj/test/roxl.o obj/test/roxr.o obj/test/rtd.o obj/test/rtm.o obj/test/rtr.o obj/test/rts.o obj/test/sbcd.o obj/test/scc.o obj/test/sub.o obj/test/suba.o obj/test/subi.o obj/test/subq.o obj/test/subx.o obj/test/swap.o obj/test/tas.o obj/test/trap.o obj/test/trapcc.o obj/test/trapv.o obj/test/tst.o obj/test/unlk.o obj/test/unpk.o

FPU_TEST_FILE=obj/test/test_main.o obj/test/test.o obj/test/fmove_from.o obj/test/fint.o obj/test/fsinh.o obj/test/fintrz.o obj/test/fsqrt.o obj/test/flognp1.o obj/test/fetoxm1.o obj/test/ftanh.o obj/test/fatan.o obj/test/fasin.o obj/test/fatanh.o obj/test/fsin.o obj/test/ftan.o obj/test/fetox.o obj/test/ftwotox.o obj/test/ftentox.o obj/test/flogn.o obj/test/flog10.o obj/test/flog2.o obj/test/fabs.o obj/test/fcosh.o obj/test/fneg.o obj/test/facos.o obj/test/fcos.o obj/test/fgetexp.o obj/test/fgetman.o obj/test/fdiv.o obj/test/fmod.o obj/test/fadd.o obj/test/fmul.o obj/test/frem.o obj/test/fscale.o obj/test/fsub.o obj/test/fsincos.o obj/test/fcmp.o obj/test/ftst.o obj/test/fmovecr.o obj/test/fmove_to.o obj/test/fmovem_sr.o obj/test/fmovem.o obj/test/fscc.o obj/test/fdbcc.o obj/test/ftrapcc.o obj/test/fstate.o
MMU_TEST_FILE=obj/test/test_main.o obj/test/test.o obj/test/mmu1.o obj/test/mmu2.o

test_fpu:  obj/libnewcpu.a $(FPU_TEST_FILE) $(HEADERS)
	g++ $(TEST_OPTIONS) $(FPU_TEST_FILE) obj/libnewcpu.a -o test_fpu -lboost_unit_test_framework

test_cpu:  obj/libnewcpu.a $(TEST_FILE) $(HEADERS)
	g++ $(TEST_OPTIONS) $(TEST_FILE) obj/libnewcpu.a -o test_cpu -lboost_unit_test_framework

test_mmu:  obj/libnewcpu.a $(MMU_TEST_FILE) $(HEADERS)
	g++ $(TEST_OPTIONS) $(MMU_TEST_FILE) obj/libnewcpu.a -o test_mmu -lboost_unit_test_framework




