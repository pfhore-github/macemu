#ifndef NEWCPU_COMMON
#define NEWCPU_COMMON


template<class T, class U> T bit_cast(const U& v) {
	static_assert(sizeof(T)==sizeof(U));
	T r;
	memcpy(&r, &v, sizeof(T));
	return r;
}
#endif
