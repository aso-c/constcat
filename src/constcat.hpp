//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.7.1
// Copyright   : Copyright (c) aso by 01.10.25.
// Description : Static concatenation ANSI-style string and generate std::array for std::string_view
//============================================================================

#ifndef __CONSTCAT_HPP__
#define __CONSTCAT_HPP__


namespace aso
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
//	std::clog << "Processing item " << size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
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
  //	std::clog << testprn(buf);
	return splitter(act,buf);
    }; /* template <> chainsplit() */


    //!
    // Template function "chainsplit" - operating with set of any string buffers
    // and call splitter every buffer, that is passed into this procedure
    // Initial & intermediate version with with any numbers set of buffers
    //
    // Template parameters:
    // @tparam Act	  - type of the action executor, functor with template <...> operator()
    // @tparam Item   - type of the first array buffer 'buf' items
    // @tparam sz     - size of the first array buffer 'buf'
    // @tparam Bufs   - variadic pack of type parameters, that passed to procedure
    //
    // Parameters:
    // @param[in]	act   - type Act action parameter, that called at final string buffers parsing
    // @param[in]   buf   - reference to const array of the any size
    // @param[in]   bufs  - variadic pack of reference to const arrays of the any sizes, that must be processed
    template <class Act, typename Item, std::size_t sz, typename... Bufs>
    constexpr auto chainsplit(Act&& act,  const Item (&buf)[sz], const Bufs&... bufs)
    {
//	std::clog << testprn(buf);
	return chainsplit([act, &buf]<typename... Its>(Its... its) constexpr {
	    return splitter(act, buf, its...);}, bufs...);
    }; /* template <> chainsplit() */


#
    //!
    // Template function "constcat" - create std::array object from the passed buffers of any type
    //		(buffer must be is not a string!!!)
    //
    // Template parameters:
    // @tparam Bufs	  - variadic template types pack for the passed string buffers
    //
    // Parameters:
    // @param[in]   bufs  - variadic parameters pack of reference to set of the const C-style string buffers,
    //		that must be concatenated
    template <typename... Bufs>
    constexpr auto constcat(const Bufs&... bufs)
    {
	return chainsplit([]<typename... Its>(Its... its) constexpr -> std::array<std::common_type_t<Its...>, sizeof...(Its)> {
				return { its...};},
						bufs...);
    }; /* template <> aso::constcat() */



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
	    return chainsplit(act,buf);
	}; /* template <> aso::str::split() */


	//!
	// Template function "aso::str::split" - operating with set of any string buffers,
	// and drop trail terminator of the string for all buffers, exclude last one,
	// and invoke splitter for every buffer, that is passed into this procedure
	// Initial & intermediate version with set of some buffers
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam Item   - type of the first array buffer 'buf' items
	// @tparam sz     - size of the first array buffer 'buf'
	// @tparam Bufs   - variadic pack of type parameters, that passed to procedure
	//
	// Parameters:
	// @param[in]	act   - type Act action parameter, that called at final string buffers parsing
	// @param[in]   buf   - reference to const array of the any size
	// @param[in]   bufs  - variadic pack of reference to const arrays of the any sizes, that must be processed
	template <class Act, typename Item, std::size_t sz, typename... Bufs>
	constexpr auto split(Act&& act, const Item (&buf)[sz], const Bufs&... bufs)
	{
//	    std::clog << testprn(buf);
	    /// drop the trailing string terminator of the buf
	    return split([act, &buf]<typename... Its>(Its... its) constexpr
				{ return splitter(act, reinterpret_cast<const Item (&)[sz-1]>(buf), its...);},
								bufs...);
	}; /* template <> aso::str::split() */




	//!
	// Template function "constcat" - create std::array object from the passed string buffers,
	//
	// Template parameters:
	// @tparam Bufs	  - variadic template types pack for the passed string buffers
	//
	// Parameters:
	// @param[in]   bufs  - variadic parameters pack of reference to set of the const C-style string buffers,
	//		that must be concatenated
	template <typename... Bufs>
	constexpr auto constcat(const Bufs&... bufs)
	{
	    return split([]<typename... Its>(Its... its) constexpr -> std::array<std::common_type_t<Its...>, sizeof...(Its)>
				{ return { its...};}, bufs...);
	}; /* template <> aso::str::constcat() */

    }; /* namespace aso::str */

}; /* namespace aso */


// template <std::size_t size>
// std::array(const char[size]) -> std::array<const char, size>;


//template </*typename Item,*/ std::size_t sz1, std::size_t sz2>
//constexpr std::array<const Item, sz1 + sz2 - 1> operator +(const Item (&str1)[sz1], const Item (&str2)[sz2])
//constexpr std::array<const char, sz1 + sz2 - 1> operator +(/*const char (&*/std::string_view str1/*)[sz1]*/, /*const char (&*/std::string_view str2/*)[sz2]*/)
constexpr std::array<const char, 2> operator +(std::string_view str1, std::string_view str2)
{
    return {str1[0], str2[0]}/*aso::str::constcat(str1, str2)*/;
}; /* template <Item, sz1, sz2> std::array<Item, sz1 + sz2 - 1> operator +(&str1, &str2)
 */




template <typename item, std::size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
};


#endif	// __CONSTCAT_HPP__
