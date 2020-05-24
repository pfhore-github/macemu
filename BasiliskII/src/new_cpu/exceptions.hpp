#pragma once
namespace Exception {
class Base {
protected:
	Base(int v, int p) :vec(v), priority(p) {}
public:
	const int vec;
	const int priority;
	void exec();
	virtual void save_stack(uint16_t sr) = 0;
};
class Format0 : public Base {
	uint32_t nextpc;
protected:
	Format0(int v, uint32_t next, int priority = 3)
		:Base(v, priority), nextpc(next) {}
	void save_stack(uint16_t sr) override;
};

class Format1 : public Base {
	uint32_t nextpc;
protected:
	Format1(int v, uint32_t next, int priority = 3)
		:Base(v, priority), nextpc(next) {}
	void save_stack(uint16_t sr) override;
};

class Format2 : public Base {
	uint32_t nextpc;
	uint32_t error_address;
protected:
	Format2(int v, uint32_t error_addr, uint32_t next, int priority = 3)
		:Base(v, priority), nextpc(next), error_address(error_addr) {}
	void save_stack(uint16_t sr) override;
};

class Format3 : public Base {
protected:
	Format3(int v, int priority = 4)
		:Base(v, priority) {}
	void save_stack(uint16_t sr) override;
};
struct AccessFault : public Base {
	AccessFault(uint32_t addr, uint16_t ssw, bool is_inst);
	uint32_t addr;
	uint16_t ssw;	
	void save_stack(uint16_t sr) override;
};

struct AddressError : public Format2 {
	AddressError(uint32_t addr);
};
	

struct DivBy0 : public Format2 {
	DivBy0();
};

struct ChkError : public Format2 {
	ChkError();
};
struct TrapCC : public Format2 {
	TrapCC();
};

struct IllegalInst : public Format0 {
	IllegalInst();
};

struct PrivilegeViolation : public Format0 {
	PrivilegeViolation();
};

struct Trace : public Format2 {
	Trace();
};

struct FormatError : public Format0 {
	FormatError();
};

struct ALine : public Format0 {
	ALine();
};

struct FLine : public Format0 {
	FLine();
};


struct Trap : public Format0 {
	Trap(int i);
};

struct FP_unimplemented_inst : public Format2 {
	FP_unimplemented_inst();
};

struct FP_BSUN : public Format0 {
	FP_BSUN();
};

struct FP_INEXACT : public Format0 {
	FP_INEXACT();
};

struct FP_DIV_BY_0 : public Format0 {
	FP_DIV_BY_0();
};

struct FP_UNDERFLOW : public Format0 {
	FP_UNDERFLOW();
};
struct FP_OPERR : public Format0 {
	FP_OPERR();
};

struct FP_OVERFLOW : public Format0 {
	FP_OVERFLOW();
};
struct FP_SNAN : public Format0 {
	FP_SNAN();
};

struct FP_unimplemented_type_pre : public Format0 {
	FP_unimplemented_type_pre();
};


struct FP_unimplemented_type_post : public Format3 {
	FP_unimplemented_type_post();
};
}
constexpr uint16_t SSW_CP = 1 << 15; // FPU Store Exception pending
constexpr uint16_t SSW_CU = 1 << 14; // FPU UNimplement Exception Pending
constexpr uint16_t SSW_CT = 1 << 13; // Trace Pending
constexpr uint16_t SSW_CM = 1 << 12; // MOVEM pending
