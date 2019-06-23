#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP
class exception_t {
protected:
	const int v;
	uint16_t sr;
	exception_t(int v) : v(v*4) {}
	virtual void save_stack(CPU* regs, uint32_t pc) = 0;
	void save_stack0(CPU* regs, uint32_t pc);
	void save_stack1(CPU* regs, uint32_t pc);
	void save_stack2(CPU* regs, uint32_t pc, uint32_t addr);
	void save_stack3(CPU* regs, uint32_t addr);
	void save_stack4(CPU* regs, uint32_t pc, uint32_t ea);
public:
	virtual void run(CPU* regs, uint32_t pc);
};

class Interrupt : public exception_t {	
	int level;
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	Interrupt(int l) :exception_t(l+24), level(l) { }
};

class FormatError : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	FormatError() :exception_t(14) {}
};

class TRAP_N : public exception_t {
	int n;
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	TRAP_N(int n) :exception_t(n+32), n(n) {}
};

class IllegalInstruction : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	IllegalInstruction() :exception_t(4) {}
};

class DivByZero : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	DivByZero() :exception_t(5) {}
};

class AInstruction : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	AInstruction() :exception_t(10) {}
};

class FInstruction : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	FInstruction() :exception_t(11) {}
};

class PrivViolation : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	PrivViolation() :exception_t(8) {}
};

class FpuPreInst : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	FpuPreInst(int e) :exception_t(e) {}
};

class ChkTrap : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	ChkTrap() :exception_t(6) {}
};

class Trapcc : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	Trapcc() :exception_t(7) {}
};

class TraceEx : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	TraceEx() :exception_t(9) {}
};
class UnimplementedFp : public exception_t {
	uint32_t ea;
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	UnimplementedFp(uint32_t ea) :exception_t(11), ea(ea) {}
};

class AddressError : public exception_t {
	uint32_t ea;
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	AddressError(uint32_t ea) :exception_t(3), ea(ea) {}
};

class AccessFault : public exception_t {
protected:
	void save_stack(CPU* regs, uint32_t pc) override;
public:
	uint32_t fa;
	uint32_t ea;
	bool CP, CU, CT, CM, MA, ATC, LK, RW;
	int SIZE, TT, TM;	
	AccessFault() :exception_t(2), fa(0) {}
};
void ILLEGAL(CPU* regs) __attribute__((noreturn));
inline void ILLEGAL(CPU* regs) { throw IllegalInstruction(); }
inline void PRIV_VIOLATION(CPU* regs) { throw PrivViolation(); }
#endif
