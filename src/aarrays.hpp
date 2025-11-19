//============================================================================
// @file        : arrays.hpp
// @author      : Andrey Solomatov (aso)
// Copyright    : Copyright (c) aso by 17.11.25.
// @date Created  07.11.2025
//       Updated  19.11.2025
// @version     : v.0.8.1(s)
// @description : Literally merging the ANSI-style strings into a generated std::array.
//		  For various uses, such as initializing std::string_view.
//		  Secuental implementation of the expansion of the array items values.
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
	    if constexpr (size > 1)
		return splitter<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
	    else
		return action(buf[0], its...);
	}; /* template <> splitter */



	///FixMe Temporaily!!! Direct generation of the std::array from the C-style array (temporarily)
	template <typename Item, std::size_t Sz>
	constexpr auto genx(Item (&buf)[Sz])
	{
	    return gen([]<typename... Its>(Its... its) constexpr -> std::array<Item, sizeof...(its)> {
					return { its...};},
							     buf);
	}; /* template <> aso::genx() */

	//!
	// Template function "aso::arr::gen" - create const std::array object from the passed
	//		C-style array buffer of any size (buffer may be is not a string)
	//
	// Template parameters:
	// @tparam Act	  - type of the action executor, functor with template <...> operator()
	// @tparam Item  -  type of input array items
	// @tparam Sz	  - std::size_t, size of input array
	//
	// @tparam ... Its - trailng variadic pack types of the splitted individual items from input buffer
	//
	//
	// Parameters:
	// @param[in]	actor - type Act parameter with operator() or a lambda, named or anonymous
	// @param[in]   buf   - reference to C-style array with the "size" sizeof,
	//			that must be converted to std::array
	// @param[in]   its   - variadic parameters pack of the splitted individual items
	//			for adding to generated std::array
	template <class Act, typename Item, std::size_t Sz, typename... Its>
	constexpr auto gen(Act&& act, Item (&buf)[Sz], Its...its)
	{
	    if constexpr (Sz > 1)
		return gen<Act, Item, Sz-1, Its...>(std::forward<Act>(act), reinterpret_cast<const Item (&)[Sz-1]>(buf), buf[Sz-1], its...);
	    else
		return act(buf[0], its...);
	}; /* template <> aso::gen() */


	//!
	// Template function "generate" - operating with any string buffers
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
	constexpr auto generate(Act act, const Item (&buf)[sz])
	{
	    return gen(act,buf);
	}; /* template <> generate() */


	//!
	// Template function "generate" - operating with set of any string buffers
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
	constexpr auto generate(Act&& act, const Item (&buf)[sz], const Item (&...bufs)[sizes])
	{
	    return generate([act, &buf]<typename... Its>(Its... its) constexpr {
		return gen(act, buf, its...);}, bufs...);
	}; /* template <> generate() */



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
	template <typename It1, std::size_t Sz1, typename It2, std::size_t Sz2, std::size_t... Szs>
	constexpr auto merge(It1 (&buf1)[Sz1], It2 (&buf2)[Sz2], auto (&...bufs)[Szs])
	{
	    return generate([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)> {
				    return { its...};},
							bufs...);
	}; /* template <> aso::arr::merge() */


#if 0	// Temporarily exclude index sequence using example
	namespace detail
	{
	    // Non-constant value sequencer
	    template<class TIt, std::size_t... idx>
	    constexpr std::array<std::remove_cv_t<TIt>, sizeof...(idx)>
	        sequencer(TIt (&a)[sizeof...(idx)], std::index_sequence<idx...>)
	    {
	        return {{a[idx]...}};
	    }; /* sequencer() */

	    // Constant-value sequencer
	    template<class TIt, std::size_t... idx>
	    constexpr std::array<TIt, sizeof...(idx)>
	        csequencer(TIt (&a)[sizeof...(idx)], std::index_sequence<idx...>)
	    {
	        return {{a[idx]...}};
	    }; /* csequencer() */
	};

	// May be - Sequencer | Sequenced & idxs 4 index_sequence?
	/// (union) array & integral index sequence for it
	template <typename TItem, std::size_t N, std::size_t...idx>
	struct unit
	{
	    TItem (&data)[N];
	    constexpr static std::size_t size = N;
	    constexpr static std::index_sequence/*<idx...>*/ sequence = std::make_index_sequence<N>{};
	}; /* unit */

	// Constant-value sequencer
	template<class TIt, std::size_t... idx>
	constexpr std::array<TIt, sizeof...(idx)>
	obtain(unit<TIt, sizeof...(idx), idx... > arrcmplx/*TIt (&a)[sizeof...(idx)], std::index_sequence<idx...>*/)
	{
	    return {{arrcmplx.data[idx]...}};
	}; /* obtain() */


	template <typename TItem, std::size_t... Ns>
	struct generator
	{
	    constexpr static std::size_t size = (... + Ns);

	    //struct

	    std::array<TItem, size> seq();

	    constexpr std::array<TItem, size> obtain(TItem (&...arrs)[Ns]);

	}; /* template struct seq */


	template<typename TItem, std::size_t N>
	constexpr std::array<TItem, N> gen(TItem (&buf)[N])
	{
	    return detail::csequencer(buf, std::make_index_sequence<N>{});
	    //return obtain({buf});
	}; /* template <> aso::gen() */


	namespace detail
	{
	    template<class T, std::size_t N, std::size_t... I>
	    constexpr std::array<std::remove_cv_t<T>, N>
	        to_array_impl(T (&&a)[N], std::index_sequence<I...>)
	    {
	        return {{std::move(a[I])...}};
	    }
	}

	template<class T, std::size_t N>
	constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N])
	{
	    return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
	}
#endif	// end of temporarily exclude index sequence using example


    }; /* namespace aso::arr */


    namespace str
    {

	//!
	// Template function "aso::str::generate" - operating with set of any string buffers,
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
	constexpr auto generate(Act act, const Item (&buf)[sz])
	{
	    return arr::generate(act, buf);
	}; /* template <> aso::str::generate() */


	//!
	// Template function "aso::str::generate" - operating with set of any string buffers,
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
	constexpr auto generate(Act&& act, const Item (&buf)[sz], const  Item (&...bufs)[sizes])
	{
//	    std::clog << testprn(buf);
	    /// drop the trailing string terminator of the buf
	    return generate([act, &buf]<typename... Its>(Its... its) constexpr
				{ return arr::gen(act, reinterpret_cast<const Item (&)[sz-1]>(buf), its...);},
								bufs...);
	}; /* template <> aso::str::generate() */




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
	    return generate([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)>
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
