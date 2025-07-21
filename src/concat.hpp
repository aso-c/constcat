//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.1
// Copyright   : Copyright (c) aso by 21.07.25.
// Description : Static concatenation ANSI-style string wuth std::strung_view
//============================================================================

#ifndef __CONCAT_HPP__
#define __CONCAT_HPP__


constexpr std::string_view generate_str(const char c1, const char c2, const char c3)
{
	char res[] = {c1, c2, c3};

	std::cout << "c1 is: '" << c1 << "'" << std::endl;
	std::cout << "c2 is: '" << c2 << "'" << std::endl;
	std::cout << "c3 is: '" << c3 << "'" << std::endl;

	std::cout << "Inner generated result string is: '" << res << "'" << std::endl;


	return res;
}; /*generate_str */


#endif	// __CONCAT_HPP__
