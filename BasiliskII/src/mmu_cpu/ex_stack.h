#ifndef EX_STACK_H__
#define EX_STACK_H__
#include <stack>
#include <memory>
#include <setjmp.h>
// jit friendly exception handling
void THROW(int i = 2) __attribute__((noreturn));
struct ILLEGAL_INST_EX {};
struct BUS_ERROR_EX { bool atc = false; };
#endif