inline void TEST_NZ8(int8_t v) {
	regs.n = v < 0;
	regs.z = v == 0;
}
inline void TEST_NZ16(int16_t v) {
	regs.n = v < 0;
	regs.z = v == 0;
}
inline void TEST_NZ32(int32_t v) {
	regs.n = v < 0;
	regs.z = v == 0;
}
inline uint8_t DO_OR_B(uint8_t a, uint8_t b) {
    uint8_t v = a | b;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_OR_W(uint16_t a, uint16_t b) {
    uint16_t v = a | b;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_OR_L(uint32_t a, uint32_t b) {
    uint32_t v = a | b;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint8_t DO_AND_B(uint8_t a, uint8_t b) {
    uint8_t v = a & b;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_AND_W(uint16_t a, uint16_t b) {
    uint16_t v = a & b;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_AND_L(uint32_t a, uint32_t b) {
    uint32_t v = a & b;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint8_t DO_EOR_B(uint8_t a, uint8_t b) {
    uint8_t v = a ^ b;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_EOR_W(uint16_t a, uint16_t b) {
    uint16_t v = a ^ b;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_EOR_L(uint32_t a, uint32_t b) {
    uint32_t v = a ^ b;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint8_t DO_ADD_B(uint8_t a, uint8_t b) {
    uint8_t v = a + b;
    regs.v = ( a & b & ~v | ~a & ~b & v) >> 7 & 1;
    regs.x = regs.c = ( a & b | ~v & a | ~v & b) >> 7 & 1;
    TEST_NZ8(v);
    return v;
}

inline uint16_t DO_ADD_W(uint16_t a, uint16_t b) {
    uint16_t v = a + b;
    regs.v = ( a & b & ~v | ~a & ~b & v) >> 15 & 1;
    regs.c = ( a & b | ~v & a | ~v & b) >> 15 & 1;
    TEST_NZ16(v);
    return v;
}

inline uint32_t DO_ADD_L(uint32_t a, uint32_t b) {
    uint32_t v = a + b;
    regs.v = ( a & b & ~v | ~a & ~b & v) >> 31 & 1;
    regs.x = regs.c = ( a & b | ~v & a | ~v & b) >> 31 & 1;
    TEST_NZ32(v);
    return v;
}

inline uint8_t DO_SUB_B(uint8_t a, uint8_t b) {
    uint8_t v = a - b;
    regs.v = ( ~b & a & ~v | b & ~a & v) >> 7 & 1;
    regs.x = regs.c = ( b & ~a | ~v & ~a | v & b) >> 7 & 1;
    TEST_NZ8(v);
    return v;
}

inline uint16_t DO_SUB_W(uint16_t a, uint16_t b) {
    uint16_t v = a - b;
    regs.v = ( ~b & a & ~v | b & ~a & v) >> 15 & 1;
    regs.x = regs.c = ( b & ~a | ~v & ~a | v & b) >> 15 & 1;
    TEST_NZ16(v);
    return v;
}

inline uint32_t DO_SUB_L(uint32_t a, uint32_t b) {
    uint32_t v = a - b;
    regs.v = ( ~b & a & ~v | b & ~a & v) >> 31 & 1;
    regs.x = regs.c = ( b & ~a | ~v & ~a | v & b) >> 31 & 1;
    TEST_NZ32(v);
    return v;
}

inline void DO_CMP_B(uint8_t a, uint8_t b) {
    uint8_t v = a - b;
    regs.v = ( ~b & a & ~v | b & ~a & v) >> 7 & 1;
    regs.c = ( b & ~a | ~v & ~a | v & b) >> 7 & 1;
    TEST_NZ8(v);
}

inline void DO_CMP_W(uint16_t a, uint16_t b) {
    uint16_t v = a - b;
    regs.v = ( ~b & a & ~v | b & ~a & v) >> 15 & 1;
    regs.c = ( b & ~a | ~v & ~a | v & b) >> 15 & 1;
    TEST_NZ16(v);
}

inline void DO_CMP_L(uint32_t a, uint32_t b) {
    uint32_t v = a - b;
    regs.v = ( ~b & a & ~v | b & ~a & v) >> 31 & 1;
    regs.c = ( b & ~a | ~v & ~a | v & b) >> 31 & 1;
    TEST_NZ32(v);
}