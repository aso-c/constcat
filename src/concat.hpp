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
}; /* regen_c() */



//
// Class "splitter" - split array into individual elements and return
//	pointer to 'exec' procedure, that called with all it's items
//
// Parameters:
//	- size - size_t, size of input array
//	- TItem - type of the items of the array
//	- TOut	- return type of the 'exec' procedure
//
template <size_t size, typename TItem, typename TOut>
class splitter
{
public:

	using outgen = TOut (*)();

#if 0
	template <size_t curr, typename ... Its>
	constexpr outgen gen(const TItem (&buf)[size], Its ...its)
	{
		if constexpr (curr > 0)
			return gen<curr-1, TItem, Its...>(buf, buf[curr-1], its...);
		else
//			return {its..., '\0'};
			return exec();
	};
#endif

	template <size_t curr, TOut ... its>
	constexpr outgen gen(const TItem (&buf)[size])
	{
		if constexpr (curr > 0)
			return gen<curr-1, buf[curr-1], its...>(buf);
		else
//			return {its..., '\0'};
			return exec();
	};

	outgen exec;	// pointer to exec() procedure

}; /*splitter  */


//
// Class "splitter_t" - split array into individual elements
// and return object TOut by call gather() procedure
//
// Parameters:
//	- TItem - type of the items of the array
//	- TOut	- return type of the 'gather()' procedure
//
template <typename TItem, typename TOut>
class splitter_t
{
public:

	using outgen = TOut (*)();

#if 0
	template <size_t curr, typename ... Its>
	constexpr outgen gen(const TItem (&buf)[size], Its ...its)
	{
		if constexpr (curr > 0)
			return gen<curr-1, TItem, Its...>(buf, buf[curr-1], its...);
		else
//			return {its..., '\0'};
			return exec();
	};
#endif

	template <size_t size, TItem ... its>
	constexpr TOut yeld(const TItem (&buf)[size])
	{
		if constexpr (size > 0)
			return yeld<size-1, buf[size-1], its...>(buf);
		else
//			return {its..., '\0'};
			return gather<its...>();
	};

	template <TItem ... its>
	TOut gather();	// final procedure, called from the yeld()

}; /*splitter_t  */


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


template <typename item, size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
//	return out;
};



#endif	// __CONCAT_HPP__
