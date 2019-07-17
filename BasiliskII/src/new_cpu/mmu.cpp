#include "registers.hpp"
void NonMMU::movec_to(int v, uint32_t value) {
	switch(v) {
	case 0 : cpu->SFC = value; return;
	case 1 : cpu->DFC = value; return;
	case 0x800 : cpu->USP = value; return;
	case 0x801 : cpu->VBR = value; return;
	}
}
uint32_t NonMMU::movec_from(int v) {
	switch(v) {
	case 0 : return cpu->SFC;
	case 1 : return cpu->DFC;
	case 0x800 : return cpu->USP;
	case 0x801 : return cpu->VBR;
	}
	return 0;
}
