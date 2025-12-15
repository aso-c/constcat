//============================================================================
// @file        : arrays.hpp
// @author      : Andrey Solomatov (aso)
// Copyright    : Copyright (c) aso by 17.11.25.
// @date Created  07.11.2025
//       Updated  09.12.2025
// @version     : v.0.8.5.5(s)
// @description : Literally merging the ANSI-style strings into a generated std::array.
//		  For various uses, such as initializing std::string_view.
//		  Secuental implementation of the expansion of the array items values.
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
	template <Callable Act, typename TItem, std::size_t size, typename... Its>
	constexpr auto splitter(const Act& action, const TItem (&buf)[size], Its...its);

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


	/// template <> aso::arr::make(): Create std::array std::array from the set of the items
	template <typename... Items>
	constexpr std::array<const std::common_type<Items...>, sizeof...(Items)>
	make(Items... items) {
	    return {items...};
	}; /* template <> aso::arr::make() */



	//! Contains the internal tools for manipulation with arrays
	namespace spec
	{
	    //!
	    // Template function "aso::arr::spec::split()" - recursive implementation functionality of the
	    // aso::arr::splitter() procedure with calculating index instead the buffer type cast in the
	    // split process and returns resulting object by calling the action template procedure
	    // with all splitted items; for using in the aso::arr::split() template procedure
	    //
	    // Template parameters:
	    // @tparam Idx	  - index value in the current recursive call at the splitting sequence
	    // @tparam Act	  - type of the action executor, functor with template <...> operator()
	    // @tparam TItem  - type the item of input array
	    // @tparam size   - std::size_t, size of input array
	    //
	    // @tparam ... Its - trailng variadic pack types of the splitted individual items from input buffer
	    //
	    // Parameters:
	    // @param[in]	actor - type Act parameter with operator() or a lambda, named or anonymous
	    // @param[in]   buf   - reference to const TItem array, with the "size" sizeof
	    template <std::size_t Offs, Callable Act, typename TItem, /*std::size_t size,*/ typename... Its>
	    constexpr auto split(const Act& action, const TItem /*(&*/*buf/*)[size]*/, Its...its)
	    {
		if constexpr (!(Offs > 0))
		    return action(its...);
		else
		    return split<Offs - 1>(action, buf, buf[Offs-1], its...);
	    }; /* template <> aso::arr::spec::split() */


#if 0
	    //!
	    // Template function "aso::arr::expand()" - execute the 'act' parameter with expanded buffer
	    //	    of the C-style array to individual items with std::index_sequence
	    //	    (buffer may be is not a string)
	    //	    Usage:
	    // @code
	    //		expand(action, buffer, make_index_sequence<N>());
	    // @endcode
	    //
	    // Template parameters:
	    // @tparam Act   - type of the action executor, functor with template <...> operator()
	    // @tparam Item  - type of input array items
	    // @tparam Sz    - std::size_t, size of input array
	    // @tparam ...I  - variadic parameter pack std::size index sequence
	    //
	    // Parameters:
	    // @param[in] actor	- type Act parameter with operator() or a lambda, named or anonymous
	    // @param[in] buf	- reference to C-style array with the "size" sizeof,
	    //			  that must be converted to std::array
	    // @param[in] index_sequence<I...> - variadic parameters pack of the splitted individual items
	    //			for adding to generated std::array
	    template <Callable Act_exp, typename Item, std::size_t Sz, size_t... I>
	    constexpr auto expand(Act_exp const &act_exp, Item (&buf)[Sz], std::index_sequence<I...>)
	    {
		return act_exp(buf[I]...);
	    }; /* template <> aso::arr::spec::expand() */
#endif


	    //!
	    // Template function "aso::arr::spec::yeld()" - distribute passed buffer into pack
	    // of individual items & merging it with previous items pack with std::sequence.
	    // Discard dedicated 'expand()' procedure
	    //
	    //	    Usage:
	    // @code
	    //		yeld(action, buffer, make_index_sequence<N>());
	    // @endcode
	    // Template parameters:
	    // @tparam Act   - type of the action executor, functor with template <...> operator()
	    // @tparam Item  - type of input array items
	    // @tparam Sz    - std::size_t, size of input array
	    // @tparam ...I  - variadic parameter pack std::size index sequence
	    //
	    // Parameters:
	    // @param[in] dispose - parameter of the 'Act' callable type with operator() or a lambda,
	    //				named or anonymous;
	    // @param[in] buf	  - reference to C-style array with the "size" sizeof,
	    //				that must be converted to std::array;
	    // @param[in] index_sequence<I...> - variadic parameters pack indexes for the split array buffer
	    //				to the individual items for adding to generated std::array;
	    template <Callable Act, typename Item, std::size_t sz, std::size_t... I>
	    constexpr auto yeld(const Act& deploy, const Item (&buf)[sz], std::index_sequence<I...>)
	    {
		return deploy(buf[I]...);
	    }; /* template <> aso::arr::spec::yeld() */


	    //!
	    // Template function "aso::arr::spec::emit()" - unwinds a set of passed string buffers,
	    // pick up fust buffer & send it to the aso::arr::yeld() procedure in a recursive calls chain.
	    // Initial and intermediate versions with an any number of buffers.
	    //
	    // Template parameters:
	    // @tparam Act    - type of the action executor, functor with template <...> operator()
	    // @tparam Item   - type of the array buffers 'buf' & 'bufs' items
	    // @tparam sz     - size of the first array buffer 'buf'
	    // @tparam sizes  - variadic pack parameters, sizes of the arrays, that passed to procedure
	    // @tparam NxIts  - next items type variadic pack, that will be passed to the outer level lambda
	    //			at the next buffers expanding
	    // @tparam Its    - current buffer expansion type variadic pack, that passed by the inner level lambda
	    //			to the act
	    //
	    // Parameters:
	    // @param[in] act   - type Act action parameter, that called at final string buffers parsing
	    // @param[in] buf   - reference to const array of the any size
	    // @param[in] bufs  - variadic pack of reference to const arrays of the any sizes, that must be processed
	    // @param[in] nxits - variadic parameters pack of the distributed individual items of the current array buff
	    //			  for passing to inner lambda in the yeld() expanding buffer procedure
	    template <Callable Act, typename Item, std::size_t sz, std::size_t... sizes>
	    constexpr auto emit(const Act& act, const Item (&buf)[sz], const Item (&...bufs)[sizes])
	    {
		return emit([act, &buf]<typename... NxIts>(NxIts... nxits) constexpr {
			    return yeld([act, nxits...]<typename... Its>(Its... its) constexpr {
				return act(its..., nxits...);
			    }, buf, std::make_index_sequence<sz>());
			}, bufs...);
	    }; /* template <> aso::arr::spec::emit() */

	    /// Terminal simple version of the template function "aso::arr::emit()"
	    /// All parameters similar as at the full version, except for dropped out.
	    template <Callable Act_em>
	    constexpr auto emit(const Act_em& act_em)
	    {
		return act_em();
	    }; /* template <> aso::arr::spec::emit() */

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
	    return spec::emit([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)> {
				    return { its...};
				},
				    buf1, buf2, bufs...);
	}; /* template <> aso::arr::merge() */


	/// Implementation of the template function "aso::arr::splitter()" - split array into individual elements
	/// and returns resulting object by calling the action template procedure with all splitted items
	template <Callable Act, typename TItem, std::size_t Sz, typename... Its>
	constexpr auto splitter(const Act& action, const TItem (&buf)[Sz], Its...its)
	{
	    return spec::split<Sz>(Act(action), buf, its...);
	}; /* template <> aso::arr::splitter() */

    }; /* namespace aso::arr */


    //! utilities for manipulating with generalized strings
    namespace str
    {

	//! Contains the internal tools for manipulation with strings
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


#if 0
	    //!
	    // Template function "aso::str::spec::yeld()" - expand passed string buffer into pack
	    // of individual chars, drop terminal char & merging it with previous chars pack
	    template <Callable Act_yeld, typename Item, std::size_t sz, typename... Items>
	    constexpr auto yeld(const Act_yeld& act_yeld, const Item (&buf)[sz], const Items ...items)
	    {
		return arr::spec::expand([act_yeld, items...] <typename... Its>(Its... its) constexpr {
		    return act_yeld(its..., items...);
		}, buf, std::make_index_sequence<sz-1>());
	    }; /* template <> aso::str::spec::yeld() */
#endif


	    //!
	    // Template function "aso::str::spec::emit()" - unwinds a set of passed string buffers,
	    // pick up fust buffer & send it to the aso::arr::yeld() procedure in a recursive calls chain.
	    // Initial and intermediate versions with an any number of buffers.
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
	    constexpr auto emit(const Act& act, const Item (&buf)[sz], const Item (&...bufs)[sizes])
	    {
		return emit([act, &buf]<typename... NxIts>(NxIts... nxits) constexpr {
			    return arr::spec::yeld([act, nxits...]<typename... Its>(Its... its) constexpr {
				return act(its..., nxits...);
			    }, buf, std::make_index_sequence<sz-1>());
			}, bufs...);

//		return emit([act_em, &buf]<typename... Its>(Its... its) constexpr {
//			    return yeld(act_em, buf, its...);
//			},
//			    bufs...);
	    }; /* template <> aso::str::spec::emit() */

	    /// Terminal simple version of the template function "aso::arr::emit()"
	    /// All parameters similar as at the full version, except for dropped out.
	    template <Callable Act_em>
	    constexpr auto emit(const Act_em& act_em)
	    {
		return act_em();
	    }; /* template <> aso::str::spec::emit() */

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
	    return spec::/*curry*/emit([]<typename... Its>(Its... its) constexpr -> const std::array<std::common_type_t<Its...>, sizeof...(Its)+1>
				{ return { its..., '\0'};}, bufs...);
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
