#ifndef INST_CMN_HPP
#define INST_CMN_HPP

inline void test_trace_branch() {
	if( cpu.T == 1 ) {
		throw Exception::Trace();
	}
}
inline uint8_t get_cr() {
	return cpu.X << 4 |
		cpu.N << 3 |
		cpu.Z << 2 |
		cpu.V << 1 |
		cpu.C;
}

inline void set_cr(uint8_t v) {
	cpu.X = (v >> 4) & 1;
	cpu.N = (v >> 3) & 1;
	cpu.Z = (v >> 2) & 1;
	cpu.V = (v >> 1) & 1;
	cpu.C = v & 1;
}

inline uint16_t get_sr() {
	uint16_t low = get_cr();
	low |=
		cpu.intmask << 8 |
		cpu.M << 12 |
		cpu.S << 13 |
		cpu.T << 14;
	return low;
}
inline void save_sp() {
	if( ! cpu.S ) {
		cpu.ISP = cpu.R[15];
	} else if( cpu.M ) {
		cpu.MSP = cpu.R[15];
	} else {
		cpu.ISP = cpu.R[15];
	}
}
inline void load_sp() {
	if( ! cpu.S ) {
		cpu.R[15] = cpu.ISP;
	} else if( cpu.M ) {
		cpu.R[15] = cpu.MSP;
	} else {
		cpu.R[15] = cpu.ISP;
	}
}
inline void set_sr(uint16_t v) {
	save_sp();
	set_cr(v);
	cpu.intmask =(v >> 8) & 7;
	cpu.M =(v >> 12) & 1;
	cpu.S =(v >> 13) & 1;
	cpu.T =(v >> 14) & 3;
	load_sp();
}
inline void set_nz(int8_t v) {
	cpu.N = v < 0;
	cpu.Z = !v;
}
inline void set_nz(int16_t v) {
	cpu.N = v < 0;
	cpu.Z = !v;
}
inline void set_nz(int32_t v) {
	cpu.N = v < 0;
	cpu.Z = !v;
}






#endif
