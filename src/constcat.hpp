//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.3
// Copyright   : Copyright (c) aso by 05.08.25.
// Description : Static concatenation ANSI-style string wuth std::strung_view
//============================================================================

#ifndef __CONSTCAT_HPP__
#define __CONSTCAT_HPP__

#if 0
constexpr std::array<char, 4>  generate_str(const char c1, const char c2, const char c3)
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
}; /* regen_c() */
#endif

//
// Template function "splitter" - split array into individual elements
// and return object TOut by calling actor->gather() procedure all splitted items
//
// Template parameters:
//  - TActor - class with the action executor, TActor::gateher()
//  - TItem  - type of the input array items
//	- size - size_t, size of input array
//	- TOut	- type that returned the 'splitter' and the 'gather' procedure
//  - typename ... Its - trailng variadic pack of the individual parameters
//
// Parameters:
// - actor - TActor class with executed method 'TActor::gather()'
// - buf   - reference to const TItem array, with its sizeof is size
	template <class TActor, typename TItem, size_t size, typename TOut = std::array<TItem, size>, typename ... Its>
	static constexpr TOut splitter(TActor* actor, const TItem (&buf)[size], Its ...its)
	{
		std::clog << "Processing item " << size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
		if constexpr (size > 1)
			return splitter<TActor, TItem, size-1, TOut, Its...>(actor, reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
		else
//			return {its..., '\0'};
			return actor->gather(buf[0], its...);
	}; /*splitter */


//
// Template function "splitter" - split array into individual elements
// and return object TOut by calling actor->gather() procedure all splitted items
// Dual version - for pair of input array, split the array, that passed second
//
// Template parameters:
//  - TActor - class with the action executor, TActor::gateher()
//  - TItem  - type of the input array items
//	- size, size1 - size_t, size of input arrays
//	- TOut	- type that returned the 'splitter' and the 'gather' procedure
//  - typename ... Its - trailing variadic pack of the individual parameters
//
// Parameters:
// - actor - TActor class with target executioned method
// - buf, buf1   - reference to const TItem data arrays, with its sizeof is size
template <class TActor, typename TItem, size_t size1, size_t size, typename TOut = std::array<TItem, size1 + size>, typename ... Its>
static constexpr TOut splitter(TActor* actor, const TItem (&buf1)[size1], const TItem (&buf)[size], Its ...its)
{
	std::clog << "Processing item " << size1+size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
	if constexpr (size > 1)
		return splitter<TActor, TItem, size1, size-1, TOut, Its...>(actor, buf1, reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
	else
//			return actor->gather(buf[0], its...);
		return splitter<TActor, TItem, size1, TOut, Its...>(actor, buf1, buf[0], its...);
}; /*splitter */


/// The splitter test class
template <std::size_t len>
class test_split
{
public:
	constexpr test_split(const char (&instr)[len]):
		n(splitter<std::decay_t<decltype(*this)>, char, len, int> (this, instr))
	{};

	template <typename ... Its>
	int gather(Its ... its) { ((std::clog << "--f_splitter_test-----------" << std::endl) << ... << its) << std::endl;   return 0;};

	int n;
}; /* test_split */

/// Dual splitter test class
template <std::size_t len1, std::size_t len>
class test_split2
{
public:
	constexpr test_split2(const char (&instr1)[len1], const char (&instr)[len]):
		n(splitter<std::decay_t<decltype(*this)>, char, len1, len, int> (this, instr1, instr))
	{};

	template <typename ... Its>
	int gather(Its ... its) { ((std::clog << "--f_dual_splitter_test-----------" << std::endl) << ... << its) << std::endl;   return 0;};

	int n;
}; /* test_split2 */




#if 0
template <size_t len>
class regen_clss
{
public:
	template <size_t curr, typename ... Chs>
//	constexpr std::array<char, len+1> operator()(std::string_view str, Chs ...cs)
	constexpr static std::array<char, len> operator()(const char (&str)[len], Chs ...cs)
//	constexpr std::array<char, len> data(const char (&str)[len], Chs ...cs)
	{
		if constexpr (curr > 0)
//			return regen<len, curr-1, char, Chs...>(str, str[curr-1], cs...);
			return operator()<curr-1, char, Chs...>(str, str[curr-1], cs...);
		else
			return {cs.../*, '\0'*/};
	};

#if 0	// exclude regen_c method
//	template <size_t len>
	constexpr std::array<char, len> regen_c(const char (&str)[len])
//	constexpr std::array<char, len> operator()(const char (&str)[len])
	{
		return operator()<len-1>(str);
	}; /* regen_c() */
#endif	// exclude regen_c method

}; /* class regen_clss */
#endif


template <typename item, std::size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
//	return out;
};



#endif	// __CONSTCAT_HPP__
