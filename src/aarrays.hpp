//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.8.0.2-r
// Copyright   : Copyright (c) aso by 17.11.25.
// Description : Static concatenation ANSI-style string and generate std::array for std::string_view
//		    Recursive implementation
//		    Devel stopped (temporarily?)
//============================================================================

#ifndef __AARRAYS_HPP__
#define __AARRAYS_HPP__


namespace aso
{
    namespace arr
    {
	//!
	// Template function "generate()" - split array into individual elements
	// and return object from calling the action template procedure with all splitted items
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
	constexpr auto generate(Act&& action, const TItem (&buf)[size], Its...its)
	{
	    if constexpr (size > 1)
		return generate<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
	    else
		return action(buf[0], its...);
	}; /* template <> generate() */




	//!
	// Template function "unwind()" - operating with any string buffers
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
	constexpr auto unwind(Act act, const Item (&buf)[sz])
	{
  	    return generate(act,buf);
	}; /* template <> unwind() */


	//!
	// Template function "unwind()" - operating with set of any string buffers
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
	constexpr auto unwind(Act&& act, const Item (&buf)[sz], const Item (&...bufs)[sizes])
	{
//	    std::clog << testprn(buf);
	    return unwind([act, &buf]<typename... Its>(Its... its) constexpr {
		return generate(act, buf, its...);}, bufs...);
	}; /* template <> unwind() */



	//!
	// Template function "merge" - merging the std::array object from the passed buffers of any type
	//		(buffer may be is not a string!!!)
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
	    return unwind([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)> {
				    return { its...};},
							bufs...);
	}; /* template <> aso::merge() */

	//!
	// Template function "aso::arr::gen()" - create const std::array object from the passed buffers of single type
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
	    return unwind([]<typename... Its>(Its... its) constexpr -> std::array<Item, sizeof...(its)> {
				    return { its...};},
							buf, bufs...);
	}; /* template <> aso::gen() */

    }; /* namespace aso::arr */


    namespace str
    {

	//!
	// Template function "aso::str::unwind()" - operating with set of any string buffers,
	// and drop trail terminator of the string for all buffers, exclude last one,
	// and invoke splitter for every buffer, that is passed into this procedure
	// Terminal version with one string buffer: pass to a unwind()
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
	constexpr auto unwind(Act act, const Item (&buf)[sz])
	{
	    return arr::generate(act, buf);
	}; /* template <> aso::str::unwind() */


	//!
	// Template function "aso::str::unwind()" - operating with set of any string buffers,
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
	constexpr auto unwind(Act&& act, const Item (&buf)[sz], const  Item (&...bufs)[sizes])
	{
	    /// drop the trailing string terminator of the buf
	    return unwind([act, &buf]<typename... Its>(Its... its) constexpr
				{ return arr::generate(act, reinterpret_cast<const Item (&)[sz-1]>(buf), its...);},
								bufs...);
	}; /* template <> aso::str::generate() */




	//!
	// Template function "merge()" - merging std::array object from the passed string buffers,
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
	    return unwind([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)>
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




template <typename item, std::size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
};


#endif	// __AARRAYS_HPP__
