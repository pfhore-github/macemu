#ifndef NEWCPU_MEMORY_H
#define NEWCPU_MEMORY_H
void* to_ra(uint32_t va, int sz, bool rw);
inline void write_b(uint32_t va, uint8_t b) {
	*(uint8_t*)to_ra(va, 1, true) = b;
}
inline void write_w(uint32_t va, uint16_t w) {
	*(uint16_t*)to_ra(va, 2, true) = SDL_SwapBE16(w);
}
inline void write_l(uint32_t va, uint32_t lw) {
	*(uint32_t*)to_ra(va, 4, true) = SDL_SwapBE16(lw);
}
inline uint8_t read_b(uint32_t va) {
	return *(uint8_t*)to_ra(va, 1, false);
}
inline uint16_t read_w(uint32_t va) {
	return *(uint16_t*)to_ra(va, 2, false);
}
inline uint32_t read_l(uint32_t va) {
	return *(uint32_t*)to_ra(va, 4, false);
}
inline uint16_t fetch_w(uint32_t& va) {
	uint16_t v = read_w(va);
	va += 2;
	return v;
}
inline uint32_t fetch_l(uint32_t& va) {
	uint32_t v = read_l(va);
	va += 4;
	return v;
}

#endif
