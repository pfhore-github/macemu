#pragma once
#include <tuple>
#include <type_traits>
namespace MOCK {
template<int N, typename T>
static std::ostream& put_impl(std::ostream& os, const T& t) {
	if constexpr ( N < std::tuple_size< T >::value ) {
			os << std::get<N>(t) << ",";	
		return put_impl<N+1>(os, t);
		}
	return os;
}

template<typename ... Args>
struct Arg_tuple {
	std::tuple<Args...> x;
	Arg_tuple(Args... i) :x(i...) {}
};
template<typename ... Args>
bool operator==(const Arg_tuple<Args...>& a, const Arg_tuple<Args...>& b) {
	return a.x == b.x;
}
template<typename ... Args>
std::ostream& operator<<(std::ostream& os, const Arg_tuple<Args...>& t) {
	return put_impl<0>(os, t.x);
}
class MockBase {
protected:
	int called = 0;
	const std::string name;
	MockBase(std::string_view n) :name(n) {}
public:	
	void verify(int cnt) {
		BOOST_TEST_CHECK( called == cnt,
						  "function " << name << " is called " << called << " (expected " << cnt << ")");
	}
	virtual void reset() { called = 0; }
	virtual ~MockBase() {}	
};



template<typename E>
class MOCK_Impl {};

template<typename R, typename... Args>
class MOCK_Impl<R ( Args... ) > : public MockBase {
	std::unordered_map<int, std::function<R(Args...)>> expect;
	MOCK_Impl(const MOCK_Impl&) = delete;
	MOCK_Impl& operator=(const MOCK_Impl&) = delete;
protected:
	void reset() override {
		MockBase::reset();
	}
public:
	R invoke(Args... args) {
		called++;
		if( expect[called] ) {
			return expect[called](args...);
		} else if( expect[0] ) {
			return expect[0](args...);
		} else {
			BOOST_ERROR( "unexpected call to " << name );
			return R();
		}
	}
	MOCK_Impl(const std::string& n) :MockBase(n) { }
	MOCK_Impl& nth_is(int n, R v) {
		expect[n] = [v](Args... ) { return v; };	
		return *this;
	}
	MOCK_Impl& nth_is(int n, R v, Args... args) {
		Arg_tuple<Args...> args_tmp ( args... );
		expect[n] = [args_tmp, v, this](Args... arg) {
						BOOST_TEST_CHECKPOINT( called << "-th call " );
						BOOST_CHECK_EQUAL( args_tmp , Arg_tuple<Args...>( arg...) );
						return v;
					};
		return *this;
	}

	MOCK_Impl& always(R v, Args... args) {
		return nth_is(0, v, args... );
	}

	MOCK_Impl& always(R v) {
		return nth_is(0, v);
	}

	template<typename F> MOCK_Impl& nth_do(int n, F f) {
		expect[n] = f;
		return *this;
	}

	template<typename F> MOCK_Impl& always_do(F f) {
		return nth_do(0, f);
	}
};

template<typename R>
class MOCK_Impl<R ( ) > : public MockBase {
	std::unordered_map<int, std::function<R()>> expect;
	MOCK_Impl(const MOCK_Impl&) = delete;
	MOCK_Impl& operator=(const MOCK_Impl&) = delete;
protected:
	void reset() override {
		MockBase::reset();
	}
public:
	R invoke() {
		called++;
		if( expect[called] ) {
			return expect[called]();
		} else if( expect[0] ) {
			return expect[0]();
		} else {
			BOOST_ERROR( "unexpected call to " << name );
			return R();
		}
	}
	MOCK_Impl(const std::string& n) :MockBase(n) { }
	MOCK_Impl& nth_is(int n, R v) {
		expect[n] = [v]() {	return v; };
		return *this;
	}

	MOCK_Impl& always(R v) {
		return nth_is(0, v);
	}

	template<typename F> MOCK_Impl& nth_do(int n, F f) {
		expect[n] = f;
		return *this;
	}

	template<typename F> MOCK_Impl& always_do(F f) {
		return nth_do(0, f);
	}
};


template<typename... Args>
class MOCK_Impl<void ( Args... ) > : public MockBase {
	std::unordered_map<int, std::function<void(Args...)>> expect;
	MOCK_Impl(const MOCK_Impl&) = delete;
	MOCK_Impl& operator=(const MOCK_Impl&) = delete;
protected:
	void reset() override {
		MockBase::reset();
	}
public:
	void invoke(Args... args) {
		called++;
		if( expect[called] ) {
			return expect[called](args...);
		} else if( expect[0] ) {
			return expect[0](args...);
		} else {
			BOOST_ERROR( "unexpected call to " << name );
		}
	}
	MOCK_Impl(const std::string& n) :MockBase(n) { }
	MOCK_Impl& nth_is(int n, Args... args) {
		Arg_tuple<Args...> args_tmp ( args... );
		expect[n] = [args_tmp, this](Args... arg) {
						BOOST_TEST_CHECKPOINT( called << "-th call " );
						BOOST_TEST( args_tmp == Arg_tuple<Args...>(arg...) );
					};
		return *this;
	}

	MOCK_Impl& nth_is_do_nothing(int n) {
		expect[n] = [](Args... ) {};
		return *this;
	}

	MOCK_Impl& always(Args... args) {
		return nth_is(0, args... );
	}

	MOCK_Impl& always_do_nothing() {		
		return nth_is_do_nothing(0);
	}

	template<typename F> MOCK_Impl& nth_do(int n, F f) {
		expect[n] = f;
		return *this;
	}

	template<typename F> MOCK_Impl& always_do(F f) {
		return nth_do(0, f);
	}
};

template<>
class MOCK_Impl<void ( ) > : public MockBase {
	std::unordered_map<int, std::function<void()>> expect;
	MOCK_Impl(const MOCK_Impl&) = delete;
	MOCK_Impl& operator=(const MOCK_Impl&) = delete;
protected:
	void reset() override {
		MockBase::reset();
	}
public:
	void invoke() {
		called++;
		if( expect[called] ) {
			return expect[called]();
		} else if( expect[0] ) {
			return expect[0]();
		} else {
			BOOST_ERROR( "unexpected call to " << name );
			return ;
		}
	}
	MOCK_Impl(const std::string& n) :MockBase(n) { }
	template<typename F> MOCK_Impl& nth_do(int n, F f) {
		expect[n] = f;
		return *this;
	}

	MOCK_Impl& nth_do_nothing(int n) {
		expect[n] = []() {};
		return *this;
	}

	template<typename F> MOCK_Impl& always_do(F f) {
		return nth_do(0, f);
	}

	MOCK_Impl& always_do_nothing() {
		return nth_do_nothing(0);
	}

};

extern std::unordered_map<std::string, std::shared_ptr<MockBase>> objects;
void verify(const std::string& name, int cnt = 1);

template<typename T>
auto get(const std::string& name) {
	auto i = objects.find( name );
	return std::dynamic_pointer_cast<MOCK_Impl<T>> ( i->second );
}


template<typename R, typename... Args>
R invoke(const std::string& name, Args... args) {
	auto p = get<R(Args...)>( name );
	BOOST_REQUIRE( p );
	return p->invoke( args... );
}

template<class T> 
std::shared_ptr<MOCK_Impl<T>> make(const std::string& name) {
	auto ret = std::make_shared<MOCK_Impl<T>>(name);
	objects.emplace( name, ret );
	return ret;
}

void reset_all();

}
#define MAKE_MOCK(name) (*MOCK::make<decltype(name)>(#name))
#define GET_MOCK(name) (*MOCK::get<decltype(name)>(#name))
