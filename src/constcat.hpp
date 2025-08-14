//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.5
// Copyright   : Copyright (c) aso by 14.08.25.
// Description : Static concatenation ANSI-style string with std::array/std::string_view
//============================================================================

#ifndef __CONSTCAT_HPP__
#define __CONSTCAT_HPP__



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
    std::clog << "Processing item " << size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
    if constexpr (size > 1)
	return splitter<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
    else
	return action(buf[0], its...);
}; /*splitter */




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

/// operator <<() for print to ostream oject of the class testprn
template <std::size_t sz, typename Item>
std::ostream& operator << (std::ostream& out, const testprn<sz, Item> &tprn) {
    return tprn(out);
}




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
    std::clog << testprn(buf);
    return splitter(act,buf);
}; /* template <> chainsplit() */


//!
// Template function "chainsplit" - operating with set of any string buffers
// and call splitter every buffer, that is passed into this procedure
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
constexpr auto chainsplit(Act&& act,  const Item (&buf)[sz], const Bufs&... bufs)
{
    std::clog << testprn(buf);
    return chainsplit([act, &buf]<typename... Its>(Its... its) constexpr {
	return splitter(act, buf, its...);}, bufs...);
}; /* template <> chainsplit() */



//!
// Template function "stringsplit" - operating with set of any string buffers,
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
constexpr auto strsplit(Act act, const Item (&buf)[sz])
{
    return chainsplit(act,buf);
}; /* template <> stringsplit() */


//!
// Template function "stringsplit" - operating with set of any string buffers,
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
constexpr auto strsplit(Act&& act, const Item (&buf)[sz], const Bufs&... bufs)
{
    std::clog << testprn(buf);
    return strsplit([act, &buf]<typename... Its>(Its... its) constexpr {
	return splitter(act, reinterpret_cast<const Item (&)[sz-1]>(buf), its...);}, bufs...);	// drop the trailing string terminator of the buf
}; /* template <> strsplit() */



//!
// Template function "constcat" - create std::array object from the pased string buffers,
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
    return strsplit([]<typename... Its>(Its... its) constexpr -> std::array<std::common_type_t<Its...>, sizeof...(Its)> {
			return { its...};},
					bufs...);
}; /* template <> constcat() */





template <typename item, std::size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
//	return out;
};



/// The splitter_a test class w/functor object
template <std::size_t len>
class test_split_a
{
public:

    constexpr test_split_a(const char (&instr)[len]):
	n(splitter(*this, instr))
    {};

    template <typename... Its>
    int constexpr operator()(Its ... its) { ((std::clog << "--template_action_splitter_test-----------" << std::endl) << ... << its) << std::endl;   return 0;};

    int n;
}; /* test_split_a */


/// The splitter_a test class w/named lambda
template <std::size_t len>
class test_split_nm_lmbd
{
public:

    constexpr test_split_nm_lmbd(const char (&instr)[len]):
	n(splitter(got, instr))
    {};

    static constexpr auto got = []<typename... Its>(Its... its) constexpr -> int { ((std::clog << "--[ This is a named lambda call] -----------" << std::endl) << ... << its) << std::endl; return 0;};

    int n;
}; /* test_split_nm_lmbda */

/// The splitter_a test class w/unnamed (anonymous) lambda
template <std::size_t len>
class test_split_anon_lmbd
{
public:

    constexpr test_split_anon_lmbd(const char (&instr)[len]):
	n(splitter([]<typename... Its>(Its... its) constexpr -> int { ((std::clog << "--[ This is a anonymous lambda call] -----------" << std::endl) << ... << its) << std::endl; return 0;},
		instr))
    {};

    int n;
}; /* test_split_nm_lmbda */






#endif	// __CONSTCAT_HPP__
