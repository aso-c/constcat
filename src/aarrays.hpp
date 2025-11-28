//============================================================================
// @file        : arrays.hpp
// @author      : Andrey Solomatov (aso)
// Copyright    : Copyright (c) aso by 17.11.25.
// @date Created  07.11.2025
//       Updated  27.11.2025
// @version     : v.0.8.4(r)
// @description : Literally merging the ANSI-style strings into a generated std::array.
//		  For various uses, such as initializing std::string_view.
//		  Recursive implementation of the expansion of the array items values.
//============================================================================

#ifndef __AARRAYS_HPP__
#define __AARRAYS_HPP__


namespace aso
{

    // The Callable concept - determines a callable object, like a function
    template<class F, typename ...Args>
    concept Callable = std::is_function<F(Args...)>::value;
//    concept Callable = std::is_function_v<F(Args...)>;


    //! utilities for manipulating with std::array objects & other kind of buffers
    namespace arr
    {
	//!
	// Template function "aso::arr::splitter()" - split array into individual elements
	// and returns resulting object by calling the action template procedure with all splitted items
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
	    if constexpr (size > 1)
		return splitter<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
	    else
		return action(buf[0], its...);
	}; /* template <> aso::arr::splitter() */



	/// Direct generation of the std::array from the C-style array - single array only version
	template <typename Item, std::size_t Sz>
	constexpr auto gen(Item (&buf)[Sz])
	{
	    return splitter([]<typename... Its>(Its... its) constexpr -> std::array<Item, sizeof...(its)> {
				return { its...};
			    },
				buf);
	}; /* template <> aso::arr::gen() */
	/// Direct generation of the std::array from the C-style array: additional some items version
	template <typename Item, std::size_t Sz, typename... Items>
	constexpr auto gen(Item (&buf)[Sz], Items... items) {
	    return gen(buf+1, items..., buf[0]);
	}; /* template <> aso::arr::gen() */
	/// Direct generation of the std::array from the C-style array: only some items version
	template <typename... Items>
	constexpr std::array<const std::common_type<Items...>, sizeof...(Items)> gen(Items... items) {
	    return {items...};
	}; /* template <> aso::arr::gen() */


	//! Encloses the internal utilities for service purposes
	namespace spec
	{
	    //!
	    // Template function "aso::arr::generate()" - create const std::array object from the passed
	    //		C-style array buffer of any size (buffer may be is not a string)
	    //
	    // Template parameters:
	    // @tparam Act	  - type of the action executor, functor with template <...> operator()
	    // @tparam Item  -  type of input array items
	    // @tparam Sz	  - std::size_t, size of input array
	    //
	    // @tparam ... Its - trailng variadic pack types of the splitted individual items from input buffer
	    //
	    // Parameters:
	    // @param[in]	actor - type Act parameter with operator() or a lambda, named or anonymous
	    // @param[in]   buf   - reference to C-style array with the "size" sizeof,
	    //			that must be converted to std::array
	    // @param[in]   its   - variadic parameters pack of the splitted individual items
	    //			for adding to generated std::array
	    template <class Act, typename Item, std::size_t Sz, typename... Its>
	    constexpr std::array<const std::common_type_t<Item, Its...>, Sz+sizeof...(Its)> generate(Act&& act, Item (&buf)[Sz], Its...its)
	    {
		if constexpr (Sz > 1)
			return generate<Act, Item, Sz-1, Its...>(std::forward<Act>(act), reinterpret_cast<const Item (&)[Sz-1]>(buf), buf[Sz-1], its...);
		else
		    return std::array<const std::common_type_t<Item, Its...>, sizeof...(Its)+1>{buf[0], its...};
	    }; /* template <> aso::arr::generate() */


	    //!
	    // Template function "aso::arr::spec::curry()" - unwinds a set of passed string buffers
	    // into separate parameters for calling the splitter() function in a recursive calls chain.
	    // Initial and intermediate versions with an aany number of buffers.
	    //
	    // Template parameters:
	    // @tparam Act	  - type of the action executor, functor with template <...> operator()
	    // @tparam Item   - type of the array buffers 'buf' & 'bufs' items
	    // @tparam sz     - size of the first array buffer 'buf'
	    // @tparam sizes  - variadic pack parameters, sizes of the arrays, that passed to procedure
	    //
	    // Parameters:
	    // @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	    // @param[in]   buf   - reference to const array of the any size
	    // @param[in]   bufs  - variadic pack of reference to const arrays of the any sizes, that must be processed
	    template <Callable Act, typename Item, std::size_t sz, std::size_t... sizes>
	    constexpr auto curry(const Act& act, const Item (&buf)[sz], const Item (&...bufs)[sizes])
	    {
		return curry([act, &buf]<typename... Its>(Its... its) constexpr {
			    return splitter(act, buf, its...);
			},
			    bufs...);
	    }; /* template <> aso::arr::spec::curry() */

	    //!
	    // Terminal simple version of the template function "aso::arr::curry()" without
	    // string buffer(s), only call the 'act' parameter.
	    //
	    // Template parameters:
	    // @tparam Act	  - type of the action executor, functor with template <...> operator()
	    //
	    // Parameters:
	    // @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	    template <Callable Act>
	    constexpr auto curry(Act act)
	    {
		return act();
	    }; /* template <> aso::arr::spec::curry() */

	}; /* namespace aso::arr::spec */




	//!
	// Template function "aso::arr::merge()" - create std::array object from the passed buffers of any type
	//		(buffer may be not a string)
	//
	// Template parameters:
	// @tparam Bufs	  - variadic template types pack for the passed string buffers
	//
	// Parameters:
	// @param[in]   bufs  - variadic parameters pack of reference to set of the const C-style string buffers,
	//		that must be concatenated
	template <typename It1, std::size_t Sz1, typename It2, std::size_t Sz2, std::size_t... Szs>
	constexpr auto merge(It1 (&buf1)[Sz1], It2 (&buf2)[Sz2], auto (&...bufs)[Szs])
	{
	    return spec::curry([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)> {
				    return { its...};
				},
				    buf1, buf2, bufs...);
	}; /* template <> aso::arr::merge() */

    }; /* namespace aso::arr */


    //! utilities for manipulating with generalized strings
    namespace str
    {

	//! Encloses the internal utilities for service purposes
	namespace spec
	{

	    //!
	    // Template function "aso::str::spec::curry()" - unwinds a set of passed string buffers
	    // into separate parameters for calling the splitter() function in a recursive calls chain.
	    // Initial and intermediate versions with an aany number of buffers.
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
	    template <Callable Act, typename Item, std::size_t sz, std::size_t... sizes >
	    constexpr auto curry(Act&& act, const Item (&buf)[sz], const  Item (&...bufs)[sizes])
	    {
//		std::clog << testprn(buf);
		/// drop the trailing string terminator of the buf
		return curry([act, &buf]<typename... Its>(Its... its) constexpr {
				return arr::splitter(act, reinterpret_cast<const Item (&)[sz-1]>(buf), its...);
			    },
				bufs...);
	    }; /* template <> aso::str::spec::curry() */

	    //!
	    // Terminal simple version of the template function "aso::str::spec::curry()" without
	    // string buffer(s), only call the 'act' parameter.
	    //
	    // Template parameters:
	    // @tparam Act	  - type of the action executor, functor with template <...> operator()
	    //
	    // Parameters:
	    // @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	    template <Callable Act>
	    constexpr auto curry(Act act)
	    {
		return act();
	    }; /* template <> aso::str::spec::curry(act) */

	}; /* namespace aso::str::spec */


	//!
	// Template function "aso::str::merge()" - merging passed string buffers into one const std::array object,
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
	    return spec::curry([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)>
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
