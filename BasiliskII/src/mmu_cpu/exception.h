#ifndef EXCEPTION_H__
#define EXCEPTION_H__
#include <stdint.h>
#include <stdlib.h>
void BUSERROR(uint32_t addr, uint16_t ssw, uint32_t ea) __attribute__((noreturn));
void ILLEGAL_INST() __attribute__((noreturn));
void PRIV_ERROR() __attribute__((noreturn));
void CHK2_ERROR() __attribute__((noreturn));
extern jmp_buf ex_jmp;
#endif