//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.1
// Copyright   : Copyright (c) aso by 21.07.25.
// Description : Static concatenation ANSI-style string wuth std::strung_view
//============================================================================

#ifndef __CONCAT_HPP__
#define __CONCAT_HPP__


constexpr /*consteval*/ std::array<char, 4>  generate_str(const char c1, const char c2, const char c3)
{
	const char res[] = {c1, c2, c3, '\0'};

	std::cout << "c1 is: '" << c1 << "'" << std::endl;
	std::cout << "c2 is: '" << c2 << "'" << std::endl;
	std::cout << "c3 is: '" << c3 << "'" << std::endl;

	std::cout << "Inner generated result string is: '" << res << "'" << std::endl;


	//return res;
	return {{c1, c2, c3}};
	// return /*char*(*/{c1, c2, c3}/*)*/;
}; /*generate_str */


template <std::size_t len, std::size_t curr, typename ... Chs>
constexpr std::array<char, len+1> regen(std::string_view str, Chs ...cs)
{
	if constexpr (curr > 0)
		return regen<len, curr-1, char, Chs...>(str, str[curr-1], cs...);
	else
		return {cs..., '\0'};
}; /* regen() */

template <size_t len>
constexpr std::array<char, len+1> regen(const std::string_view str)
{
	return regen<len, len>(str);
}; /* regen() */

template <size_t len>
constexpr std::array<char, len> regen_c(const char (&str)[len])
{
	return regen<len-1, len-1>(str);
}; /* regen() */



#endif	// __CONCAT_HPP__
