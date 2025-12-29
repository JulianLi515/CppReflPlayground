#pragma once
#include <type_traits>

namespace my_reflect::static_refl
{
    /**
     * @brief A compile-time list of types.
     *
     * @tparam Args Types stored in the list.
     *
     * Usage:
     * @code
     * using L = my_reflect::type_list<int, double, char>;
     * static_assert(L::size == 3);
     * @endcode
     */

    template <typename... Args>
    struct type_list
    {
        static constexpr std::size_t size = sizeof...(Args);
    };

    namespace details
    {
        // head
        template <typename>
        struct head;

        template <typename T, typename... Remains>
        struct head<type_list<T, Remains...>>
        {
            using type = T;
        };

        // get
        template <typename, std::size_t>
        struct get;

        template <typename T, typename... Remains>
        struct get<type_list<T, Remains...>, 0>
        {
            using type = T;
        };

        template <typename T, typename... Remains, std::size_t N>
        struct get<type_list<T, Remains...>, N>
        {
            static_assert(N < sizeof...(Remains) + 1, "get<N>: index out of range");
            using type = typename get<type_list<Remains...>, N - 1>::type;
        };

        // tail (last type)
        template <typename>
        struct tail;

        template <typename T>
        struct tail<type_list<T>>
        {
            using type = T;
        };

        template <typename T, typename... Remains>
        struct tail<type_list<T, Remains...>>
        {
            using type = typename tail<type_list<Remains...>>::type;
        };

        // concat
        template <typename, typename>
        struct concat;

        template <typename... T1, typename... T2>
        struct concat<type_list<T1...>, type_list<T2...>>
        {
            using type = type_list<T1..., T2...>;
        };

        // push (front/back by matching)
        template <typename, typename>
        struct push;

        // push_back: push<type_list<Args...>, T>
        template <typename... Args, typename T>
        struct push<type_list<Args...>, T>
        {
            using type = type_list<Args..., T>;
        };

        // push_front: push<T, type_list<Args...>>
        template <typename T, typename... Args>
        struct push<T, type_list<Args...>>
        {
            using type = type_list<T, Args...>;
        };

        // count
        template <typename, template <typename> typename>
        struct count;

        template <template <typename> typename F>
        struct count<type_list<>, F> : std::integral_constant<int, 0> {};

        template <typename T, typename... Remains, template <typename> typename F>
        struct count<type_list<T, Remains...>, F>
            : std::integral_constant<int, (F<T>::value ? 1 : 0) + count<type_list<Remains...>, F>::value>
        {
        };

        // map
        template <typename, template <typename> typename>
        struct map;

        template <typename... Args, template <typename> typename F>
        struct map<type_list<Args...>, F>
        {
            using type = type_list<typename F<Args>::type...>;
        };

        // filter
        template <typename, template <typename> typename>
        struct filter;

        template <template <typename> typename F>
        struct filter<type_list<>, F>
        {
            using type = type_list<>;
        };

        template <typename T, typename... Remains, template <typename> typename F>
        struct filter<type_list<T, Remains...>, F>
        {
            using rest = typename filter<type_list<Remains...>, F>::type;
            using type = std::conditional_t<
                F<T>::value,
                typename push<T, rest>::type, // push_front
                rest>;
        };

        // reverse
        template <typename>
        struct reverse;

        template <>
        struct reverse<type_list<>>
        {
            using type = type_list<>;
        };

        template <typename T, typename... Remains>
        struct reverse<type_list<T, Remains...>>
        {
            using rest_rev = typename reverse<type_list<Remains...>>::type;
            using type = typename push<rest_rev, T>::type; // push_back
        };

        // build from tuple
        template <typename>
		struct from_tuple;

        template <typename... Args>
		struct from_tuple<std::tuple<Args...>>
        {
			using type = type_list<Args...>;
        };

        template <typename>
        struct to_tuple;

        template <typename... Args>
		struct to_tuple<type_list<Args...>>
		{
			using type = std::tuple<Args...>;
		};

    } // namespace details

    // ===== Public API (only *_t / *_v) =====

    /**
     * @brief Get the first type (head) of a type_list.
     *
     * @tparam T A my_reflect::type_list<...>
     *
     * Usage:
     * @code
     * using L = my_reflect::type_list<int, double, char>;
     * using H = my_reflect::head_t<L>; // int
     * static_assert(std::is_same_v<H, int>);
     * @endcode
     */
    template <typename T>
    using head_t = typename details::head<T>::type;

    /**
     * @brief Get the N-th type (0-based) from a type_list.
     *
     * @tparam T A my_reflect::type_list<...>
     * @tparam N 0-based index
     *
     * Notes:
     * - Out-of-range will trigger static_assert in details::get.
     *
     * Usage:
     * @code
     * using L = my_reflect::type_list<int, double, char>;
     * using T0 = my_reflect::get_t<L, 0>; // int
     * using T1 = my_reflect::get_t<L, 1>; // double
     * static_assert(std::is_same_v<T1, double>);
     * @endcode
     */
    template <typename T, std::size_t N>
    using get_t = typename details::get<T, N>::type;

    /**
     * @brief Get the last type (tail) of a type_list.
     *
     * @tparam T A my_reflect::type_list<...>
     *
     * Notes:
     * - Empty list will trigger static_assert in details::tail.
     *
     * Usage:
     * @code
     * using L = my_reflect::type_list<int, double, char>;
     * using Last = my_reflect::tail_t<L>; // char
     * static_assert(std::is_same_v<Last, char>);
     * @endcode
     */
    template <typename T>
    using tail_t = typename details::tail<T>::type;

    /**
     * @brief Concatenate two type_list into one.
     *
     * @tparam L1 type_list<A...>
     * @tparam L2 type_list<B...>
     *
     * Usage:
     * @code
     * using A = my_reflect::type_list<int, double>;
     * using B = my_reflect::type_list<char, float>;
     * using C = my_reflect::concat_t<A, B>; // type_list<int,double,char,float>
     * static_assert(C::size == 4);
     * @endcode
     */
    template <typename L1, typename L2>
    using concat_t = typename details::concat<L1, L2>::type;

    /**
     * @brief Push a type to the front OR back of a list, depending on argument order.
     *
     * Two supported forms:
     * 1) push_t<T, type_list<Args...>>     -> type_list<T, Args...>        (push_front)
     * 2) push_t<type_list<Args...>, T>     -> type_list<Args..., T>        (push_back)
     *
     * @tparam A Either a type_list<...> or a single type
     * @tparam B Either a type_list<...> or a single type
     *
     * Notes:
     * - If you do push_t<type_list<...>, type_list<...>>, both specializations match and
     *   it can become ambiguous. For list+list, use concat_t instead.
     *
     * Usage:
     * @code
     * using L = my_reflect::type_list<int, double>;
     * using F = my_reflect::push_t<char, L>;   // type_list<char,int,double>  (front)
     * using B = my_reflect::push_t<L, char>;   // type_list<int,double,char>  (back)
     * @endcode
     */
    template <typename A, typename B>
    using push_t = typename details::push<A, B>::type;

    /**
     * @brief Count how many types in a type_list satisfy a unary predicate F<T>::value.
     *
     * @tparam T A my_reflect::type_list<...>
     * @tparam F Predicate template with: static constexpr bool value
     *
     * Usage:
     * @code
     * template<class U> struct is_int { static constexpr bool value = std::is_same_v<U,int>; };
     * using L = my_reflect::type_list<int, double, int, char>;
     * static_assert(my_reflect::count_v<L, is_int> == 2);
     * @endcode
     */
    template <typename T, template <typename> typename F>
    constexpr int count_v = details::count<T, F>::value;

    /**
     * @brief Map each type in a type_list through a metafunction F<T>::type.
     *
     * @tparam T A my_reflect::type_list<...>
     * @tparam F Metafunction template with: using type = ...
     *
     * Usage:
     * @code
     * template<class U> struct double_to_int { using type = std::conditional_t<std::is_same_v<U,double>, int, U>; };
     * using L = my_reflect::type_list<int, double, char>;
     * using M = my_reflect::map_t<L, double_to_int>; // type_list<int,int,char>
     * @endcode
     */
    template <typename T, template <typename> typename F>
    using map_t = typename details::map<T, F>::type;

    /**
     * @brief Filter types in a type_list by predicate F<T>::value.
     *
     * @tparam T A my_reflect::type_list<...>
     * @tparam F Predicate template with: static constexpr bool value
     *
     * Usage:
     * @code
     * template<class U> struct is_int { static constexpr bool value = std::is_same_v<U,int>; };
     * using L = my_reflect::type_list<int, double, int, char>;
     * using R = my_reflect::filter_t<L, is_int>; // type_list<int,int>
     * static_assert(R::size == 2);
     * @endcode
     */
    template <typename T, template <typename> typename F>
    using filter_t = typename details::filter<T, F>::type;

    /**
     * @brief Reverse the order of types in a type_list.
     *
     * @tparam T A my_reflect::type_list<...>
     *
     * Usage:
     * @code
     * using L = my_reflect::type_list<int, double, char>;
     * using R = my_reflect::reverse_t<L>; // type_list<char,double,int>
     * @endcode
     */
    template <typename T>
    using reverse_t = typename details::reverse<T>::type;

    /**
	 * @brief Build a type_list from a std::tuple.
	 * 
	 * @tparam T A std::tuple<...> or (const) reference to it.
	 * 
	 * Usage:
	 * @code
	 * auto tup = std::make_tuple(1, 2, 2.0, 'a');
	 * using TL = my_reflect::type_list_from_tuple_t<decltype(tup)>; // type_list<int, int, double, char>
	 * @endcode 
     */
    template <typename T>
	using type_list_from_tuple_t = typename details::from_tuple<std::remove_cv_t<std::remove_reference_t<T>>>::type;

    /**
	 * @brief Build a std::tuple from a type_list.
	 * 
	 * @tparam T A my_reflect::type_list<...>
	 * 
	 * Usage:
	 * @code
	 * using myList = my_reflect::type_list<int, double, char>;
	 * using Tup = my_reflect::tuple_from_type_list_t<myList>; // std::tuple<int, double, char>
     * @endcode 
     */
	template <typename T>
	using tuple_from_type_list_t = typename details::to_tuple<T>::type;

} // namespace my_reflect::static_refl

namespace my_reflect::static_refl::test_type_list {
    // ===== Test helpers =====
    template <typename T>
    struct is_int
    {
        static constexpr bool value = std::is_same_v<T, int>;
    };

    template <typename T>
    struct double_to_int
    {
        using type = std::conditional_t<std::is_same_v<T, double>, int, T>;
    };

    template <typename T>
    struct is_floating
    {
        static constexpr bool value = std::is_floating_point_v<T>;
    };

    // Metafunction: F<T>::type
    template <typename T>
    struct add_ptr
    {
        using type = T*;
    };

	// simple testing, hover mouse on variables to see inferred types
    inline int Test_type_list()
    {
        using namespace my_reflect::static_refl;
        using L0 = type_list<>;
        using L1 = type_list<int>;
        using L3 = type_list<int, double, char>;
        using L4 = type_list<int, double, char, float>;

        [[maybe_unused]] constexpr std::size_t s0 = L0::size; // expect: 0
        [[maybe_unused]] constexpr std::size_t s1 = L1::size; // expect: 1
        [[maybe_unused]] constexpr std::size_t s3 = L3::size; // expect: 3
        [[maybe_unused]] constexpr std::size_t s4 = L4::size; // expect: 4

        // ------------------------------------------------------------
        // head_t<List>  -> first type
        // ------------------------------------------------------------
        using H3 = head_t<L3>; // expect: int
        using H4 = head_t<L4>; // expect: int

        // ------------------------------------------------------------
        // get_t<List, N> -> N-th type (0-based)
        // ------------------------------------------------------------
        using G30 = get_t<L3, 0>; // expect: int
        using G31 = get_t<L3, 1>; // expect: double
        using G32 = get_t<L3, 2>; // expect: char

        using G40 = get_t<L4, 0>; // expect: int
        using G41 = get_t<L4, 1>; // expect: double
        using G42 = get_t<L4, 2>; // expect: char
        using G43 = get_t<L4, 3>; // expect: float

        // ------------------------------------------------------------
        // tail_t<List> -> last type
        // ------------------------------------------------------------
        using T1 = tail_t<L1>; // expect: int
        using T3 = tail_t<L3>; // expect: char
        using T4 = tail_t<L4>; // expect: float

        // ------------------------------------------------------------
        // concat_t<L1, L2> -> concatenate two type_list
        // ------------------------------------------------------------
        using A = type_list<int, double>;
        using B = type_list<char, float>;

        using C = concat_t<A, B>;   // expect: type_list<int, double, char, float>
        using C1 = concat_t<L0, A>;  // expect: type_list<int, double>
        using C2 = concat_t<A, L0>;  // expect: type_list<int, double>

        // ------------------------------------------------------------
        // push_t<A, B>
        //   push_t<T, List>      -> push_front  => type_list<T, ...>
        //   push_t<List, T>      -> push_back   => type_list<..., T>
        // ------------------------------------------------------------
        using PF = push_t<bool, L3>; // expect: type_list<bool, int, double, char>
        using PB = push_t<L3, bool>; // expect: type_list<int, double, char, bool>


        // ------------------------------------------------------------
        // count_v<List, Pred> -> count of Pred<T>::value == true
        // ------------------------------------------------------------
        using Lmix = type_list<int, float, int, double, int, char>;

        [[maybe_unused]] constexpr int c0 = count_v<L0, is_int>;      // expect: 0
        [[maybe_unused]] constexpr int c1 = count_v<L1, is_int>;      // expect: 1
        [[maybe_unused]] constexpr int c2 = count_v<Lmix, is_int>;      // expect: 3
        [[maybe_unused]] constexpr int c3 = count_v<Lmix, is_floating>; // expect: 2  (float, double)

        // ------------------------------------------------------------
        // map_t<List, F> -> transform each type by F<T>::type
        // ------------------------------------------------------------
        using M1 = map_t<L3, add_ptr>;       // expect: type_list<int*, double*, char*>
        using M2 = map_t<L3, double_to_int>; // expect: type_list<int, int, char>

        // ------------------------------------------------------------
        // filter_t<List, Pred> -> keep types that satisfy Pred<T>::value
        // ------------------------------------------------------------
        using F0 = filter_t<L0, is_int>; // expect: type_list<>
        using F1 = filter_t<L3, is_int>; // expect: type_list<int>
        using F2 = filter_t<Lmix, is_int>; // expect: type_list<int, int, int>
        using F3 = filter_t<Lmix, is_floating>; // expect: type_list<float, double>

        // ------------------------------------------------------------
        // reverse_t<List> -> reverse order of types
        // ------------------------------------------------------------
        using R0 = reverse_t<L0>; // expect: type_list<>
        using R1 = reverse_t<L1>; // expect: type_list<int>
        using R3 = reverse_t<L3>; // expect: type_list<char, double, int>
        using R4 = reverse_t<L4>; // expect: type_list<float, char, double, int>

		// ------------------------------------------------------------
		// type_list_from_tuple_t<std::tuple<...>> -> type_list<...>
		// ------------------------------------------------------------
		auto tup = std::make_tuple(1, 2,  2.0, 'a');
		auto& tupRef = tup;
		using TL = type_list_from_tuple_t<decltype(tupRef)>; // expect: type_list<int, int, double, char>

		// ------------------------------------------------------------
		// tuple_from_type_list_t<type_list<...>> -> std::tuple<...>
		// ------------------------------------------------------------
		using myList = type_list<int, double, char>;
		using Tup = tuple_from_type_list_t<myList>; // expect: std::tuple<int, double, char>

        return 0;
    }
}
