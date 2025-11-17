//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.8.0.3-r
// Copyright   : Copyright (c) aso by 17.11.25.
// Description : Literally merging the ANSI-style strings into a generated std::array.
//		 For various uses, such as initializing std::string_view.
//		 Recursive implementation of the expansion of the array items values.
//============================================================================

#ifndef __AARRAYS_HPP__
#define __AARRAYS_HPP__


namespace aso
{
    namespace arr
    {
	//!
	// Template function "splitter" - split array into individual elements
	// and return object from callint the action template procedure with all splitted items
	//
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam TItem  - type the item of input array
	// @tparam size   - std::size_t, size of input array
	//
	// @tparam ... Its - trailng variadic pack types of the splitted individual items from input buffer
	//
	// Parameters:
	// @param[in]	actor - type Act parameter with operator() or a lambda, named or anonymous
	// @param[in]   buf   - reference to const TItem array, with the "size" sizeof
	template <class Act, typename TItem, std::size_t size, typename... Its>
	constexpr auto splitter(Act&& action, const TItem (&buf)[size], Its...its)
	{
//	    std::clog << "Processing item " << size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
	    if constexpr (size > 1)
		return splitter<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
	    else
		return action(buf[0], its...);
	}; /* template <> splitter */



#if 0
/// Envelope for check the sizeof of the passed string buffers
template <std::size_t sz, typename Item>
struct testprn
{
    constexpr testprn(Item (&inbuf)[sz]): buf{inbuf} {};

    constexpr std::ostream& operator ()(std::ostream& out) const {
	return out << " \"" << buf << "\": sizeof: " << sizeof(buf) << " ,";
    };

    Item (&buf)[sz];
};

/// operator <<() for print to ostream object of the class testprn
template <std::size_t sz, typename Item>
std::ostream& operator << (std::ostream& out, const testprn<sz, Item> &tprn) {
    return tprn(out);
}
#endif




	//!
	// Template function "chainsplit" - operating with any string buffers
	// and call splitter every buffer, that is passed into
	// Terminal version with one string buffer for call a splitter
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam Item   - type of the array buffer 'buf' items
	// @tparam sz     - size of the array buffer 'buf'
	//
	// Parameters:
	// @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	// @param[in]   buf   - reference to const array of the any size
	template <class Act, typename Item, std::size_t sz>
	constexpr auto chainsplit(Act act, const Item (&buf)[sz])
	{
  //	    std::clog << testprn(buf);
	    return splitter(act,buf);
	}; /* template <> chainsplit() */


	//!
	// Template function "chainsplit" - operating with set of any string buffers
	// and call splitter every buffer, that is passed into this procedure
	// Initial & intermediate version with with any numbers set of buffers
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam Item   - type of the array buffers 'buf' & 'bufs' items
	// @tparam sz     - size of the first array buffer 'buf'
	// @tparam sizes  - variadic pack parameters, suzes of the arrays, that passed to procedure
	//
	// Parameters:
	// @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	// @param[in]   buf   - reference to const array of the any size
	// @param[in]   bufs  - variadic pack of reference to const arrays of the any sizes, that must be processed
	template <class Act, typename Item, std::size_t sz, std::size_t... sizes>
	constexpr auto chainsplit(Act&& act, const Item (&buf)[sz], const Item (&...bufs)[sizes] /*const Bufs&... bufs*/)
	{
//	    std::clog << testprn(buf);
	    return chainsplit([act, &buf]<typename... Its>(Its... its) constexpr {
		return splitter(act, buf, its...);}, bufs...);
	}; /* template <> chainsplit() */



	//!
	// Template function "constcat" - create std::array object from the passed buffers of any type
	//		(buffer may be not a string)
	//
	// Template parameters:
	// @tparam Bufs	  - variadic template types pack for the passed string buffers
	//
	// Parameters:
	// @param[in]   bufs  - variadic parameters pack of reference to set of the const C-style string buffers,
	//		that must be concatenated
	template <typename... Bufs>
	constexpr auto merge(const Bufs&... bufs)
	{
	    return chainsplit([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)> {
				    return { its...};},
							bufs...);
	}; /* template <> aso::arr::merge() */

	//!
	// Template function "aso::arr::gen" - create const std::array object from the passed buffers of single type
	//		and any sizes (buffer must be is not a string!!!)
	//
	// Template parameters:
	// @tparam Bufs	  - variadic template types pack for the passed string buffers
	//
	// Parameters:
	// @param[in]   bufs  - variadic parameters pack of reference to set of the const C-style string buffers,
	//		that must be concatenated
	template <typename Item, std::size_t sz, std::size_t... szs>
	constexpr auto gen(Item (&buf)[sz], Item (&...bufs)[szs])
	{
	    return chainsplit([]<typename... Its>(Its... its) constexpr -> std::array<Item, sizeof...(its)> {
				    return { its...};},
							buf, bufs...);
	}; /* template <> aso::gen() */

    }; /* namespace aso::arr */


    namespace str
    {

	//!
	// Template function "aso::str::split" - operating with set of any string buffers,
	// and drop trail terminator of the string for all buffers, exclude last one,
	// and invoke splitter for every buffer, that is passed into this procedure
	// Terminal version with one string buffer: pass to a chainsplitter()
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam Item   - type of the array buffer 'buf' items
	// @tparam sz     - size of the array buffer 'buf'
	//
	// Parameters:
	// @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	// @param[in]   buf   - reference to const array of the any size
	template <class Act, typename Item, std::size_t sz>
	constexpr auto split(Act act, const Item (&buf)[sz])
	{
	    return arr::chainsplit(act, buf);
	}; /* template <> aso::str::split() */


	//!
	// Template function "aso::str::split" - operating with set of any string buffers,
	// and drop trail terminator of the string for all buffers, exclude last one,
	// and invoke splitter for every buffer, that is passed into this procedure
	// Initial & intermediate version with set of some buffers
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam Item   - type of the array buffers - the 'buf' & the 'bufs' items
	// @tparam sz     - size of the first array buffer 'buf'
	// @tparam sizes  - variadic template pack sizes of the buffers, that passed to procedure
	//
	// Parameters:
	// @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	// @param[in]   buf   - reference to const array of the any size
	// @param[in]   bufs  - variadic pack of reference to const arrays of the any sizes, that must be processed
	template <class Act, typename Item, std::size_t sz, std::size_t... sizes >
	constexpr auto split(Act&& act, const Item (&buf)[sz], const  Item (&...bufs)[sizes])
	{
//	    std::clog << testprn(buf);
	    /// drop the trailing string terminator of the buf
	    return split([act, &buf]<typename... Its>(Its... its) constexpr
				{ return arr::splitter(act, reinterpret_cast<const Item (&)[sz-1]>(buf), its...);},
								bufs...);
	}; /* template <> aso::str::split() */




	//!
	// Template function "merge" - merging passed string buffers into one const std::array object,
	//
	// Template parameters:
	// @tparam Item	  - type of the passed string buffers (variant of the char's type)
	// @tparam sizes  - variadic template pack sizes of the passed string buffers
	//
	// Parameters:
	// @param[in]   bufs  - variadic parameters pack of reference to set of the const C-style string buffers,
	//		that must be merged
	template <typename Item, std::size_t... sizes>
	constexpr auto merge(const Item (&...bufs)[sizes])
	{
	    return split([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)>
				{ return { its...};}, bufs...);
	}; /* template <> aso::str::merge() */


    }; /* namespace aso::str */

}; /* namespace aso */


template </*typename Item,*/ std::size_t sz1, std::size_t sz2>
//constexpr std::array<const Item, sz1 + sz2 - 1> operator +(const Item (&str1)[sz1], const Item (&str2)[sz2])
//constexpr std::array<const char, sz1 + sz2 - 1> operator +(/*const char (&*/std::string_view str1/*)[sz1]*/, /*const char (&*/std::string_view str2/*)[sz2]*/)
constexpr std::array<const char, sz1 + sz2 - 1> operator +(const std::array<const char, sz1> arr1, const char(&buf2)[sz2])
{
    return aso::str::merge<const char(&)[sz1], const char(&)[sz2]>(arr1.data(), buf2);
};

#if 0
constexpr std::array<const char, 2> operator +(std::string_view str1, std::string_view str2)
{
    return {str1[0], str2[0]}/*aso::str::constcat(str1, str2)*/;
}; /* template <Item, sz1, sz2> std::array<Item, sz1 + sz2 - 1> operator +(&str1, &str2) */
#endif




template <typename item, std::size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
};


#endif	// __AARRAYS_HPP__
