//============================================================================
// Name        : concat.hpp
// Author      : Andrey Solomatov
// Version     : 0.4
// Copyright   : Copyright (c) aso by 12.08.25.
// Description : Static concatenation ANSI-style string with std::array/std::string_view
//============================================================================

#ifndef __CONSTCAT_HPP__
#define __CONSTCAT_HPP__

#if 0
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
//	return {its..., '\0'};
	return actor->gather(buf[0], its...);
}; /*splitter */


//
// Template function "splitter" - split array into individual elements
// and return object TOut by calling actor->gather() procedure all splitted items
// Twin version - for pair of input array, split the array, that passed second
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
	return splitter<TActor, TItem, size1, TOut, Its...>(actor, buf1, buf[0], its...);
}; /*splitter */

//TODO create strsplit class - that drop the C-string trail terminator char for the first string
//TODO and create the strcat class in the future - for concatenate C-string.

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
#endif





//
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
template <class Act, typename TItem, std::size_t size, typename ... Its>
constexpr auto splitter(Act&& action, const TItem (&buf)[size], Its ...its)
{
    std::clog << "Processing item " << size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
    if constexpr (size > 1)
	return splitter<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
    else
//	return {its..., '\0'};
//	return actor->gather(buf[0], its...);
	return action(buf[0], its...);
}; /*splitter */



#if 0
template <class Act, typename TItem1, typename TItem2, std::size_t size1, std::size_t size2, typename ... Its>
constexpr auto split2(Act&& action, const TItem1 (&buf1)[size1], const TItem2 (&buf2)[size2], Its ...its)
{
    std::clog << "Processing item " << size-1 << ": '" << (buf[size-1]? buf[size-1]: '.') << '\'' <<  std::endl;
    if constexpr (size > 1)
	return splitter<Act, TItem, size-1, Its...>(std::forward<Act>(action), reinterpret_cast<const TItem (&)[size-1]>(buf), buf[size-1], its...);
    else
//	return {its..., '\0'};
//	return actor->gather(buf[0], its...);
	return action(buf[0], its...);
}; /*split2 */
#endif

//template <typename Item, std::size_t sz>
//typedef Item Buf[sz];

template <typename Item, std::size_t sz>
using Buff = Item[sz];


template <std::size_t sz, typename Item>
constexpr std::ostream& prntit(std::ostream& out, Item (&buf)[sz])
{
    out << " \"" << buf << "\": sizeof: " << sizeof(buf) << " ,";
    return out;
};


template <std::size_t sz, typename Item>
struct testprn
{
    constexpr testprn(Item (&inbuf)[sz]): buf{inbuf} {};

    constexpr std::ostream& operator ()(std::ostream& out) const {
	return out << " \"" << buf << "\": sizeof: " << sizeof(buf) << " ,";
    };

    Item (&buf)[sz];
};

template <std::size_t sz, typename Item>
std::ostream& operator << (std::ostream& out, const testprn<sz, Item> &tprn) {
    return tprn(out);
}

template </*class Act,*/ /*typename... Its,*/ /*std::size_t... szs*//*, char... Bufs[szs]*/typename... Bufs>
constexpr auto chainsplit(/*Act act,*/ const /*Its*//* char (&bufs)[szs...]...*//*char (&bufs)[szs]*/Bufs&... bufs)
{
    (std::clog << ... << bufs) << std::endl;
    (std::clog << ... << testprn(bufs)) << std::endl << std::endl;
    //[[maybe_unused]]
    int volatile stub[] = {((std::clog << "===>>[ Check with spacing stub ]: ") << bufs << " " << "sizeof: "<< sizeof(bufs), 0) ...};

    int i = stub[0];
    i++;
    return 0;
}; /* template <> chainsplit() */


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

//
// Идеальный (простейший) вариант:
// splitter: шаблон-функция, с шаблонными параметрами - вариадический шаблон-функцию
//           для финального набора единичных данных, возвращающая заданный/требуемый тип/класс
//           (тем самым реализуется возможность рекурсии) + тип элементов массива + размер массива +
//           вариадический набор шаблонных параметров.
//
//





template <typename item, std::size_t size>
inline std::ostream& operator << (std::ostream& out, const std::array<item, size> &arr) {
	return out << arr.data();
//	return out;
};



#endif	// __CONSTCAT_HPP__
