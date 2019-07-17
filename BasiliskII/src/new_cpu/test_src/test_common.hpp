extern uint8_t *RAMBaseHost;
inline void test_write8(uint32_t addr, uint8_t v) {
	RAMBaseHost[addr] = v;
}
inline void test_write16(uint32_t addr, uint16_t v) {
	RAMBaseHost[addr] = v >> 8;
	RAMBaseHost[addr+1] = v;
}
inline void test_write32(uint32_t addr, uint32_t v) {
	RAMBaseHost[addr] = v >> 24;
	RAMBaseHost[addr+1] = v >> 16;
	RAMBaseHost[addr+2] = v >> 8;
	RAMBaseHost[addr+3] = v;
}

inline uint8_t test_read8(uint32_t addr) {
	return RAMBaseHost[addr];
}

inline uint16_t test_read16(uint32_t addr) {
	return RAMBaseHost[addr] << 8 | RAMBaseHost[addr+1];
}
inline uint32_t test_read32(uint32_t addr) {
	return RAMBaseHost[addr] << 24 |
		RAMBaseHost[addr+1] << 16 |
		RAMBaseHost[addr+2] << 8 |
		RAMBaseHost[addr+3];
}
