#ifndef FPU_MPFR_H
#define FPU_MPFR_H
#include <mpfr.h>
#include <stdint.h>
#include <tuple>
#include <utility>
class fpvalue {
    mpfr_t mp;
    uint64_t payload = ~0LLU;

  public:
    mpfr_ptr ptr() const { return const_cast<mpfr_ptr>(&mp[0]); }
    int mpfr_ret = 0;

    fpvalue();
    fpvalue(fpvalue &&old);
    fpvalue(double v);
    fpvalue(int64_t v);
    fpvalue(const fpvalue &v);
    fpvalue(bool sg, uint64_t mantissa, int exp);
    fpvalue(const char *s);

    ~fpvalue();
    void reset();

    fpvalue &operator=(fpvalue &&old);
    fpvalue &operator=(const fpvalue &old);
    fpvalue &operator=(int64_t v);
    fpvalue &operator=(float f);
    fpvalue &operator=(double d);
    fpvalue &set_exp(bool sg, uint64_t mantissa, int exp);
    fpvalue &operator=(const char *s);
    fpvalue &set_nan(uint64_t payload = ~0LLU);
    fpvalue &set_inf(bool sign);
    fpvalue &set_zero(bool sign);

    explicit operator float() const;
    explicit operator double() const;
    explicit operator int64_t() const;
    std::tuple<bool, uint64_t, mpfr_exp_t> get_zexp() const;
    std::pair<fpvalue, mpfr_exp_t> frexp() const;
    uint64_t get_payload() const;
    void set_payload(uint64_t p = ~0LLU);
    std::pair<std::string, mpfr_exp_t> get_str(size_t n);
    fpvalue &operator+=(const fpvalue &a);
    fpvalue &operator-=(const fpvalue &a);
    fpvalue &operator*=(const fpvalue &a);
    fpvalue &operator/=(const fpvalue &a);

    bool signbit() const;

    fpvalue mul_2(long li) const;
    fpvalue sqrt() const;
    fpvalue operator-() const;
    fpvalue abs() const;

    bool operator==(const fpvalue &a) const;
    bool operator!=(const fpvalue &a) const;
    int cmp(const fpvalue &a) const;
    bool is_nan() const;
    bool is_inf() const;
    bool is_zero() const;
    bool is_regular() const;
    bool is_number() const;

    fpvalue log() const;
    fpvalue log2() const;
    fpvalue log10(mpfr_rnd_t rnd = mpfr_get_default_rounding_mode()) const;
    fpvalue log1p() const;
    fpvalue exp() const;
    fpvalue exp2() const;
    fpvalue exp10() const;
    fpvalue expm1() const;

    fpvalue cos() const;
    fpvalue sin() const;
    fpvalue tan() const;
    std::pair<fpvalue, fpvalue> sin_cos() const;

    fpvalue acos() const;
    fpvalue asin() const;
    fpvalue atan() const;

    fpvalue cosh() const;
    fpvalue sinh() const;
    fpvalue tanh() const;

    fpvalue acosh() const;
    fpvalue asinh() const;
    fpvalue atanh() const;

    void set_precision(int n);

    int get_exp() const;

    void subnormalize();

    static fpvalue const_pi();
    static fpvalue const_log2();
    static fpvalue pow10_i(int i);

    fpvalue rint() const;
    fpvalue trunc() const;

    std::pair<fpvalue, long> modquo(const fpvalue &y) const;
    std::pair<fpvalue, long> remquo(const fpvalue &y) const;

    bool fits_int32() const;
    bool fits_int16() const;
    bool fits_int8() const;
};

#endif /* FPU_IEEE_H */
