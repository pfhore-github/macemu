HEADERS=common.hpp exceptions.hpp inst_cmn.hpp mem.cpp op.h  registers.hpp mmu.hpp intel.h
OPTIONS=-coverage --std=gnu++17 -g -O0 `sdl2-config --cflags --libs` 
all: test
clean:
	rm obj/* obj/test/* test

obj/%.o: %.cpp $(HEADERS)
	g++ $(OPTIONS) -c $< -o $@

obj/libnewcpu.a: obj/op_bcd.o obj/mem.o  obj/op_move.o obj/common.o  obj/inst.o   obj/op_arith.o  obj/op_bits.o obj/mmu.o obj/exceptions.o
	ar rcs obj/libnewcpu.a $^

obj/test/%.o: test_src/%.cpp
	g++ $(OPTIONS) -c $< -o $@

TEST_FILE=obj/test/test_main.o obj/test/test.o obj/test/abcd.o obj/test/add.o obj/test/adda.o obj/test/addi.o obj/test/addq.o obj/test/addx.o obj/test/and.o obj/test/andi.o obj/test/asl.o obj/test/asr.o obj/test/bcc.o obj/test/bchg.o obj/test/bclr.o obj/test/bfchg.o obj/test/bfclr.o obj/test/bfexts.o obj/test/bfextu.o obj/test/bfffo.o obj/test/bfins.o obj/test/bfset.o obj/test/bftst.o obj/test/bkpt.o obj/test/bra.o obj/test/bset.o obj/test/bsr.o obj/test/btst.o obj/test/callm.o obj/test/cas.o obj/test/cas2.o obj/test/chk.o obj/test/chk2.o
test:  obj/libnewcpu.a $(TEST_FILE) $(HEADERS)
	g++ $(OPTIONS) $(TEST_FILE) obj/libnewcpu.a -o test -lboost_unit_test_framework



