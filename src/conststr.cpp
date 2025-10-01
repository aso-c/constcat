//============================================================================
// Name        : conststr.cpp
// Author      : Andrey Solomatov
// Version     :
// Copyright   : Copyright (c) aso by 20.01.25.
// Last Updated:		      14.08.25.
// Description : Development static constexpr string concatenation
//============================================================================

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include <array>
#include <string_view>

#include "constcat.hpp"

class conststr // conststr is a literal type
{
public:
    const char* p;
    std::size_t sz;
    template<std::size_t N>
    constexpr conststr(const char(&a)[N]) : p(a), sz(N - 1) {}

    constexpr operator const char*() const { return p; };

    constexpr char operator[](std::size_t n) const
    {
        return n < sz ? p[n] : throw std::out_of_range("");
    }

    constexpr std::size_t size() const { return sz; }
};

template <typename T>
T testproc(int a);

template<std::size_t sz>
struct conststr2 // conststr is a literal type
{
    const char buf[sz];

//    template<std::size_t N>

    constexpr operator const char*() const { return buf; };

};


constexpr std::size_t count_lower(conststr s)
{
    std::size_t c{};
    for (std::size_t n{}; n != s.size(); ++n)
        if ('a' <= s[n] && s[n] <= 'z')
            ++c;
    return c;
}

// An output function that requires a compile-time constant N, for testing
template<int N>
struct constN
{
    constN() { std::cout << N << '\n'; }
};


//template <conststr str>
template <conststr2 str>
struct prnstr
{
	prnstr() { std::cout << "Constant string is a: \"" << str << "\"" << std::endl; };
};




/// The splitter_a test class w/functor object
template <std::size_t len>
class test_split_a
{
public:

    constexpr test_split_a(const char (&instr)[len]):
	n(splitter(*this, instr))
    {};

    template <typename... Its>
    int constexpr operator()(Its ... its) { ((std::clog << "--template_action_splitter_test-----------" << std::endl) << ... << its) << std::endl;   return 0;};

    int n;
}; /* test_split_a */


/// The splitter_a test class w/named lambda
template <std::size_t len>
class test_split_nm_lmbd
{
public:

    constexpr test_split_nm_lmbd(const char (&instr)[len]):
	n(splitter(got, instr))
    {};

    static constexpr auto got = []<typename... Its>(Its... its) constexpr -> int { ((std::clog << "--[ This is a named lambda call] -----------" << std::endl) << ... << its) << std::endl; return 0;};

    int n;
}; /* test_split_nm_lmbda */



/// The splitter_a test class w/unnamed (anonymous) lambda
template <std::size_t len>
class test_split_anon_lmbd
{
public:

    constexpr test_split_anon_lmbd(const char (&instr)[len]):
	n(splitter([]<typename... Its>(Its... its) constexpr -> int { ((std::clog << "--[ This is a anonymous lambda call] -----------" << std::endl) << ... << its) << std::endl; return 0;},
		instr))
    {};

    int n;
}; /* test_split_nm_lmbda */




int main()
{
#if 0
    std::clog << "Test the splitter class with C-string:" << std::endl
    		  << "===================" << std::endl;
//    test_split2("Split the two C-string with function splitter", /*"Oppa-oppa, Амерка-Европа!!!!!!!"*/ "Oppa-oppa, America-Europpa" " " "Tra-ta-ta splitter test by call the procedure");
    std::clog << "===================" << std::endl;
    test_split_a("Test the splitter of the C-string with template functor action object Yoga-Doga Tra-ta-ta splitter testing by call the callable functor w/operator ()");
    test_split_nm_lmbd("Test the splitter of the C-string with named lambda action Yoga-Doga Tra-la-la splitter_a testing");
    test_split_anon_lmbd("Test the splitter of the C-string with anonymous lambda call Buda-Uda Ra-ra-ra splitter_a testing");
    std::clog << "===================" << std::endl;
    std::clog << "Check the chainsplit, preliminary:" << std::endl;
#endif

	char name[] = u8"Абсолютный";
	const char idt[] = u8"Ultra";
#if 0
    chainsplit([]<typename... Its>(Its ... its) constexpr {
		    (std::clog << "[ chainsplitter action test ] : " << ... << its);
		    return 0;},
			"ABC", name, "cde", idt, "offset sector", "Tracer");
    std::clog << std::endl << "===================" << std::endl;
#endif

	const /*char8_t*/ char reverse[] = "Реверсивный";

#if 0
    std::clog << "===================" << std::endl;
    std::clog << "Check the strsplit:" << std::endl;
    strsplit([]<typename... Its>(Its ... its) constexpr {
		    (std::clog << "[ string splitter action test ] ==> " << ... << its);
		    return 0;},
			"ABC", ": ", name, "; ", "cde", " - ", idt, "; ", "offset sector", " - ", "is ", reverse, " -- ", "Tracer", "!!!");
#endif
    std::clog << std::endl << "===================" << std::endl;
    std::clog << "Check the constcat:" << std::endl;
    std::clog << "to std::array<>:" << std::endl;

    auto result_arr = aso::str::constcat("ABC", ": ", name, "; ", "catenated", " - ", idt, "; ", "array set", " - ", "is ", reverse, " -- ", "Tracer", "!!!");

    std::clog << "[ static compile-time concatenation the const string test to std::array<> ] ==> " << result_arr.data() << std::endl;

    std::clog << "to std::string_view:" << std::endl;
    std::string_view result_str = aso::str::constcat("Its ", "a ", "std::string_view: ", name, " - ", "catenated", " - ", "from ", idt, " - ", "array set", " - ", "is ", "Tracer", "!!!").data();

    std::clog << "[ static compile-time concatenation the const string test to std::string_view ] ==> " << result_str << std::endl;

    return 0;

}; /* main() */
