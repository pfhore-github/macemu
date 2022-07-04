
fpvalue::fpvalue() : payload(~0LLU), mpfr_ret(0) { mpfr_init(mp); }
fpvalue::~fpvalue() { mpfr_clear(mp); }
fpvalue::fpvalue(fpvalue &&old) {
    mpfr_swap(mp, old.mp);
    payload = old.payload;

    
    mpfr_set_nan(old.mp);
    old.payload = 0xffffffffffffffffLLU;
}
fpvalue::fpvalue(double v) {
    mpfr_init_set_d(mp, v, mpfr_get_default_rounding_mode());
}

fpvalue::fpvalue(int64_t v) {
    mpfr_ret = mpfr_init_set_si(mp, v, mpfr_get_default_rounding_mode());
}
fpvalue::fpvalue(const fpvalue &v) : payload(v.payload) {
    mpfr_ret = mpfr_init_set(mp, v.mp, mpfr_get_default_rounding_mode());
}
fpvalue::fpvalue(bool sg, uint64_t mantissa, int exp) : fpvalue() {
    mpfr_ret = mpfr_set_uj_2exp(mp, mantissa, exp - 63,
                                mpfr_get_default_rounding_mode());
    mpfr_setsign(mp, mp, sg, mpfr_get_default_rounding_mode());
}
fpvalue::fpvalue(const char *s) {
    mpfr_init_set_str(mp, s, 10, mpfr_get_default_rounding_mode());
}

fpvalue &fpvalue::operator=(fpvalue &&old) {
    mpfr_swap(mp, old.mp);
    payload = old.payload;
    return *this;
}

fpvalue &fpvalue::operator=(const fpvalue &old) {
    mpfr_ret = mpfr_set(mp, old.mp, mpfr_get_default_rounding_mode());
    payload = old.payload;
    return *this;
}

fpvalue &fpvalue::operator=(int64_t v) {
    mpfr_ret = mpfr_set_si(mp, v, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue &fpvalue::operator=(float f) {
    mpfr_ret = mpfr_set_flt(mp, f, mpfr_get_default_rounding_mode());
    return *this;
}
fpvalue &fpvalue::operator=(double d) {
    mpfr_ret = mpfr_set_d(mp, d, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue &fpvalue::set_exp(bool sg, uint64_t mantissa, int exp) {
    mpfr_ret = mpfr_set_uj_2exp(mp, mantissa, exp - 63,
                                mpfr_get_default_rounding_mode());
    mpfr_setsign(mp, mp, sg, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue &fpvalue::operator=(const char *s) {
    mpfr_prec_round(mp, 64, mpfr_get_default_rounding_mode());
    mpfr_ret =
        mpfr_strtofr(mp, s, nullptr, 10, mpfr_get_default_rounding_mode());
    
    return *this;
}

fpvalue &fpvalue::set_nan(uint64_t payload) {
    mpfr_set_nan(mp);
    this->payload = payload;
    return *this;
}

fpvalue &fpvalue::set_inf(bool sign) {
    mpfr_set_inf(mp, sign ? -1 : 1);
    return *this;
}

fpvalue &fpvalue::set_zero(bool sign) {
    mpfr_set_zero(mp, sign ? -1 : 1);
    return *this;
}

fpvalue::operator float() const {
    return mpfr_get_flt(mp, mpfr_get_default_rounding_mode());
}
fpvalue::operator double() const {
    return mpfr_get_d(mp, mpfr_get_default_rounding_mode());
}
int64_t fpvalue::to_int () const {
    return mpfr_get_sj(mp, mpfr_get_default_rounding_mode());
}

std::tuple<bool, uint64_t, mpfr_exp_t> fpvalue::get_zexp() const {
    bool s = mpfr_signbit(mp);
    if(mpfr_inf_p(mp)) {
        return {s, 0, 0x7fff};
    } else if(mpfr_zero_p(mp)) {
        return {s, 0, 0};
    } else if(mpfr_nan_p(mp)) {
        return {false, payload, 0x7fff};
    } else {
        mpz_t rop;
        mpz_init(rop);
        mpfr_exp_t e = mpfr_get_z_2exp(rop, mp) + 63;
        uint64_t mant;
        memcpy(&mant, rop->_mp_d, sizeof(uint64_t));
        mpz_clear(rop);
        if(e < -16383) {
            // denormalized
            mant >>= (-16383 - e);
            e = -16383;
        }
        return {s, mant, (e + 0x3fff)};
    }
}

std::pair<fpvalue, mpfr_exp_t> fpvalue::frexp() const {
    mpfr_exp_t e;
    fpvalue ret;
    mpfr_frexp(&e, ret.mp, mp, mpfr_get_default_rounding_mode());
    return {ret, e};
}
void fpvalue::reset() {
    mpfr_set_prec(mp, 64);
    payload = ~0LLU;
    mpfr_ret = 0;
}

uint64_t fpvalue::get_payload() const { return payload; }
void fpvalue::set_payload(uint64_t p) { payload = p; }
std::pair<std::string, mpfr_exp_t> fpvalue::get_str(size_t n) {
    char buf[64];
    mpfr_exp_t e;
    mpfr_get_str(buf, &e, 10, n, mp, mpfr_get_default_rounding_mode());
    return {std::string(buf), e};
}

fpvalue &fpvalue::operator+=(const fpvalue &op2) {
    mpfr_ret = mpfr_add(mp, mp, op2.mp, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue &fpvalue::operator-=(const fpvalue &op2) {
    mpfr_ret = mpfr_sub(mp, mp, op2.mp, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue &fpvalue::operator*=(const fpvalue &op2) {
    mpfr_ret = mpfr_mul(mp, mp, op2.mp, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue &fpvalue::operator/=(const fpvalue &op2) {
    mpfr_ret = mpfr_div(mp, mp, op2.mp, mpfr_get_default_rounding_mode());
    return *this;
}

fpvalue operator/(fpvalue a, const fpvalue &b) {
    return a/= b;
}
bool fpvalue::signbit() const { return mpfr_signbit(mp); }

fpvalue fpvalue::sqrt() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_sqrt(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::operator-() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_neg(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::abs() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_abs(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

int fpvalue::cmp(const fpvalue &op2) const { return mpfr_cmp(mp, op2.mp); }

bool fpvalue::operator==(const fpvalue &op2) const {
    return mpfr_equal_p(mp, op2.mp);
}
bool fpvalue::operator!=(const fpvalue &op2) const {
    return !mpfr_equal_p(mp, op2.mp);
}

bool fpvalue::is_nan() const { return mpfr_nan_p(mp); }
bool fpvalue::is_inf() const { return mpfr_inf_p(mp); }
bool fpvalue::is_zero() const { return mpfr_zero_p(mp); }
bool fpvalue::is_regular() const { return mpfr_regular_p(mp); }
bool fpvalue::is_number() const { return mpfr_number_p(mp); }

fpvalue fpvalue::log() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_log(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::log2() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_log2(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::log10(mpfr_rnd_t rnd) const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_log10(ret.mp, mp, rnd);
    return ret;
}

fpvalue fpvalue::log1p() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_log1p(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::exp() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_exp(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}
fpvalue fpvalue::exp2() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_exp2(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}
fpvalue fpvalue::exp10() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_exp10(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::expm1() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_expm1(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::cos() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_cos(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::sin() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_sin(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::tan() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_tan(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

std::pair<fpvalue, fpvalue> fpvalue::sin_cos() const {
    fpvalue ret1, ret2;
    ret1.mpfr_ret =
        mpfr_sin_cos(ret1.mp, ret2.mp, mp, mpfr_get_default_rounding_mode());
    return {ret1, ret2};
}

fpvalue fpvalue::acos() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_acos(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::asin() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_asin(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::atan() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_atan(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::cosh() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_cosh(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::sinh() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_sinh(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::tanh() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_tanh(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::acosh() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_acosh(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::asinh() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_asinh(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::atanh() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_atanh(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::rint() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_rint(ret.mp, mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::trunc() const {
    fpvalue ret;
    ret.mpfr_ret = mpfr_trunc(ret.mp, mp);
    return ret;
}

fpvalue fpvalue::const_pi() {
    fpvalue ret;
    ret.mpfr_ret = mpfr_const_pi(ret.mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::const_log2() {
    fpvalue ret;
    ret.mpfr_ret = mpfr_const_log2(ret.mp, mpfr_get_default_rounding_mode());
    return ret;
}

fpvalue fpvalue::pow10_i(int i) {
    fpvalue ret;
    ret.mpfr_ret =
        mpfr_ui_pow_ui(ret.mp, 10, i, mpfr_get_default_rounding_mode());
    return ret;
}

int fpvalue::get_exp() const { return mpfr_get_exp(mp); }

std::pair<fpvalue, long> fpvalue::modquo(const fpvalue &y) const {
    fpvalue ret;
    long q;
    ret.mpfr_ret =
        mpfr_fmodquo(ret.mp, &q, mp, y.mp, mpfr_get_default_rounding_mode());
    return {ret, q};
}

std::pair<fpvalue, long> fpvalue::remquo(const fpvalue &y) const {
    fpvalue ret;
    long q;
    ret.mpfr_ret =
        mpfr_remquo(ret.mp, &q, mp, y.mp, mpfr_get_default_rounding_mode());
    return {ret, q};
}

void fpvalue::subnormalize() {
    mpfr_ret =
        mpfr_subnormalize(mp, mpfr_ret, mpfr_get_default_rounding_mode());
}

void fpvalue::set_precision(int n) {
    mpfr_prec_round(mp, n, mpfr_get_default_rounding_mode());
}

fpvalue fpvalue::mul_2(long li) const {
    fpvalue ret;
    ret.mpfr_ret =
        mpfr_mul_2si(ret.mp, mp, li, mpfr_get_default_rounding_mode());
    return ret;
}
bool fpvalue::fits_int32() const {
    return mpfr_fits_sint_p(mp, mpfr_get_default_rounding_mode());
}
bool fpvalue::fits_int16() const {
    return mpfr_fits_sshort_p(mp, mpfr_get_default_rounding_mode());
}
bool fpvalue::fits_int8() const {
    if(mpfr_fits_sint_p(mp, mpfr_get_default_rounding_mode())) {
        long u = mpfr_get_si(mp, mpfr_get_default_rounding_mode());
        return u < 128 && u > -129;
    }
    return false;
}