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

#include "concat.hpp"

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
#if 0
    std::cout << "The number of lowercase letters in \"Hello, world!\" is ";
    constN<count_lower("Hello, world!")>(); // the string literal is implicitly
                                            // converted to conststr

    //prnstr<"Check the Test String"> prn;
    conststr jet("Jet Fighter");

    std::cout << "Const str is a: \"" << jet << "\"" << std::endl;
#endif

    const std::string_view tst(generate_str('a', 'b', 'c').data());

    std::cout << "Generated string is: " << '"' << tst.data() << '"' << std::endl;
    std::cout << "Regenerated string is: " << '"' << regen<9>("ABBA test").data() << '"' << std::endl;

    return 0;

}; /* main() */
