//============================================================================
// Name        : conststr.cpp
// Author      : Andrey Solomatov
// Version     :
// Copyright   : Copyright (c) aso by 20.01.25.
// Description : Hello World in C++, Ansi-style
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

int main()
{
    std::clog << "Test the splitter class with C-string:" << std::endl
    		  << "===================" << std::endl;
//    test_split("Split the C-string with function splitter Yoga-Doga Tra-ta-ta splitter test by call the procedure");
//    std::clog << "===================" << std::endl;
    test_split2("Split the two C-string with function splitter", /*"Oppa-oppa, Амерка-Европа!!!!!!!"*/ "Oppa-oppa, America-Europpa" " " "Tra-ta-ta splitter test by call the procedure");
    std::clog << "===================" << std::endl;
//    test_split_tf("Test the splitter of the C-string with template function action Yoga-Doga Tra-ta-ta splitter testing by call template action procedure");
    test_split_a("Test the splitter of the C-string with template functor action object Yoga-Doga Tra-ta-ta splitter testing by call the callable functor w/operator ()");
    test_split_nm_lmbd("Test the splitter of the C-string with named lambda action Yoga-Doga Tra-la-la splitter_a testing");
    test_split_anon_lmbd("Test the splitter of the C-string with anonymous lambda call Buda-Uda Ra-ra-ra splitter_a testing");
    std::clog << "===================" << std::endl;


    return 0;

}; /* main() */
